#include "AsyncChannelManager.h"

#include <qdebug.h>

#include <dpc/sybus/channel/async/manager/IRequest.h>
#include <dpc/sybus/channel/async/manager/requests/ReadParamRequest.h>
#include <dpc/sybus/channel/async/manager/requests/WriteParamRequest.h>
#include <dpc/sybus/channel/async/manager/requests/metadata/DimensionRequest.h>

#include <dpc/sybus/channel/async/manager/requests/filesystem/IFileSystemRequest.h>
#include <dpc/sybus/channel/async/manager/requests/filesystem/FileSystemCommandRequest.h>

//#define TRACE

#ifdef TRACE
#define TRACE_MSG(msg) qDebug().noquote() << DEBUG_PREFIX << msg;
#else
#define TRACE_MSG(msg) 
#endif // TRACE

namespace {
	const QString DEBUG_PREFIX = "[ChannelManager]";

	// Константы	
	const uint16_t DEFAULT_BUFFER_SIZE = 0x40;

} // namespace

namespace Dpc::Sybus
{
	AsyncChannelManager::AsyncChannelManager(QObject* parent) :
		QObject(parent),
		m_isConnected(false),
		m_isConnecting(false),
		m_readBufferSize(DEFAULT_BUFFER_SIZE),
		m_writeBufferSize(DEFAULT_BUFFER_SIZE),
		m_fileDataBufferSize(DEFAULT_BUFFER_SIZE),
		m_gotRWBuffersSize(false),
		m_gotFileDataBuffersSize(false),
		m_fsLoginState(NotLoggedInState)
	{
		resetBuffers();
	}

	AsyncChannelManager::~AsyncChannelManager()
	{
	}

	AsyncChannelPtr AsyncChannelManager::channel() const
	{
		return m_channel;
	}

	void AsyncChannelManager::setChannel(const AsyncChannelPtr& newChannel)
	{
		if (channel()) {
			disconnect(channel().get(), &AsyncChannel::connectedToDevice, this, &AsyncChannelManager::onChannelConnected);
			disconnect(channel().get(), &AsyncChannel::disconnectedFromDevice, this, &AsyncChannelManager::onChannelDisconnected);
			disconnect(channel().get(), &AsyncChannel::error, this, &AsyncChannelManager::onChannelError);
		}

		m_channel = newChannel;		
		m_isConnected = false;
		m_isConnecting = false;
		resetBuffers();
		disconnectFromDevice();

		connect(channel().get(), &AsyncChannel::connectedToDevice, this, &AsyncChannelManager::onChannelConnected);
		connect(channel().get(), &AsyncChannel::disconnectedFromDevice, this, &AsyncChannelManager::onChannelDisconnected);
		connect(channel().get(), &AsyncChannel::error, this, &AsyncChannelManager::onChannelError);
		
		emit channelChanged();
	}

	uint16_t AsyncChannelManager::readBufferSize() const
	{
		return m_readBufferSize;
	}	

	uint16_t AsyncChannelManager::writeBufferSize() const
	{
		return m_writeBufferSize;
	}	

	uint16_t AsyncChannelManager::fileDataBufferSize() const
	{
		return m_fileDataBufferSize;
	}

	bool AsyncChannelManager::isConnectedToDevice() const
	{
		return m_isConnected;
	}

	void AsyncChannelManager::connectToDevice()
	{
		if (isConnectedToDevice() || isConnecting())
			return;
		
		TRACE_MSG("connectToDevice");
		m_isConnecting = true;
		QMetaObject::invokeMethod(channel().get(), &AsyncChannel::connectToDevice);
	}

	void AsyncChannelManager::disconnectFromDevice()
	{
		QMetaObject::invokeMethod(channel().get(), &AsyncChannel::disconnectFromDevice);
	}

	void AsyncChannelManager::exec(IRequest* req, bool forwardMsgSignals)
	{
		if (forwardMsgSignals) {
			connect(req, &IRequest::infoMsg, this, &AsyncChannelManager::infoMsg, Qt::UniqueConnection);
			connect(req, &IRequest::errorMsg, this, &AsyncChannelManager::errorMsg, Qt::UniqueConnection);
			connect(req, &IRequest::warningMsg, this, &AsyncChannelManager::warningMsg, Qt::UniqueConnection);
		}	

		if (auto fsr = dynamic_cast<IFileSystemCtrlRequest*>(req); fsr) {
			addFSCtrlRequest(fsr);
			return;
		}

		if (auto fsr = dynamic_cast<IFileSystemDataRequest*>(req); fsr) {
			addFSDataRequest(fsr);
			return;
		}

		start(req);
	}

	void AsyncChannelManager::onChannelConnected()
	{
		addInfoMsg("Соединение с устройством установлено");

		m_isConnected = true;
		m_isConnecting = false;

		if (!m_gotRWBuffersSize) {
			ChannelRequest r{ ChannelRequest::ReadRequest, SP_MAX_BLOCK, 0, 2 };
			r.toMaster = true;
			auto masterReq = new ReadParamRequest(r, this);
			start(masterReq, [=] {
				auto errMsg = QString("Не удалось получить размеры буферов обмена: %1");
				if (masterReq->error()) {
					addErrorMsg(errMsg.arg(masterReq->errorFormated()));
					return;
				}

				setWriteBufferSize(std::max(DEFAULT_BUFFER_SIZE, masterReq->pack()->value<uint16_t>(0)));
				setReadBufferSize(std::max(DEFAULT_BUFFER_SIZE, masterReq->pack()->value<uint16_t>(1)));

				auto slaveReq = new ReadParamRequest(SP_MAX_BLOCK, 0, 2, this);
				start(slaveReq, [=] {
					if (slaveReq->error()) {
						addErrorMsg(errMsg.arg(slaveReq->errorFormated()));
						return;
					}

					setWriteBufferSize(std::min(writeBufferSize(), slaveReq->pack()->value<uint16_t>(0)));
					setReadBufferSize(std::min(readBufferSize(), slaveReq->pack()->value<uint16_t>(1)));
					m_gotRWBuffersSize = true;
					addInfoMsg(QString("Размер буфера чтения: %1").arg(readBufferSize()));
					addInfoMsg(QString("Размер буфера записи: %1").arg(writeBufferSize()));
				});
			});
		}

		if (!m_gotFileDataBuffersSize) {
			auto req = new DimensionRequest(SP_FILE_DATA, this);
			start(req, [=] {
				if (req->error()) {
					addErrorMsg(QString("Не удалось получить размер буфера файлового обмена: %1").arg(req->errorFormated()));
					return;
				}

				setFileDataBufferSize(std::max(DEFAULT_BUFFER_SIZE, req->dimension().subProfileCount()));
				m_gotFileDataBuffersSize = true;
				addInfoMsg(QString("Размер буфера файлового обмена: %1").arg(fileDataBufferSize()));
			});
		}

		while (!m_pendingRequestQueue.empty()) {
			auto req = m_pendingRequestQueue.front();
			m_pendingRequestQueue.pop();

			start(req);
		}

		emit connectedToDevice();
	}

	void AsyncChannelManager::onChannelDisconnected()
	{
		addInfoMsg("Соединение с устройством разорвано");

		TRACE_MSG("onChannelDisconnected");
		m_isConnected = false;
		emit disconnectedFromDevice();
	}

	void AsyncChannelManager::onChannelError(ChannelReply::Error err, const QString& errMsg)
	{
		if (!isConnecting())
			return;

		m_isConnecting = false;
		while (!m_pendingRequestQueue.empty()) {
			auto req = m_pendingRequestQueue.front();
			m_pendingRequestQueue.pop();
			req->setError(err, QString("%1(%2)").arg(errMsg).arg(toHex(err)));
			req->finish();
		}
	}

	void AsyncChannelManager::onFSCtrlRequestFinished()
	{
		m_fsCtrlQueue.pop();
		execNextFSRequest(FSCtrlRequestType);
	}

	void AsyncChannelManager::onFSDataRequestFinished()
	{
		m_fsDataQueue.pop();
		execNextFSRequest(FSDataRequestType);
	}

	bool AsyncChannelManager::isConnecting() const
	{
		return m_isConnecting;
	}

	void AsyncChannelManager::addInfoMsg(const QString& msg)
	{
		emit infoMsg(msg);
	}

	void AsyncChannelManager::addErrorMsg(const QString& msg)
	{
		emit errorMsg(msg);
	}

	void AsyncChannelManager::addWarningMsg(const QString& msg)
	{
		emit warningMsg(msg);
	}

	void AsyncChannelManager::resetBuffers()
	{
		m_readBufferSize = DEFAULT_BUFFER_SIZE;
		m_writeBufferSize = DEFAULT_BUFFER_SIZE;
		m_fileDataBufferSize = DEFAULT_BUFFER_SIZE;
		m_gotRWBuffersSize = false;
		m_gotFileDataBuffersSize = false;
	}

	void AsyncChannelManager::setReadBufferSize(uint16_t size)
	{
		m_readBufferSize = size;
	}

	void AsyncChannelManager::setWriteBufferSize(uint16_t size)
	{
		m_writeBufferSize = size;
	}

	void AsyncChannelManager::setFileDataBufferSize(uint16_t size)
	{
		m_fileDataBufferSize = size;
	}

	void AsyncChannelManager::start(IRequest* req)
	{
		if (!isConnectedToDevice()) {
			m_pendingRequestQueue.push(req);
			connectToDevice();
			return;
		}

		TRACE_MSG("start");
		req->setManager(this);
		req->start();
	}

	template<class Functor>
	void AsyncChannelManager::start(IRequest* req, Functor function)
	{
		connect(req, &IRequest::finished, this, [=] { function(); req->deleteLater(); });
		start(req);
	}

	void AsyncChannelManager::addFSCtrlRequest(IFileSystemCtrlRequest* req)
	{		
		m_fsCtrlQueue.push(req);
		if (m_fsCtrlQueue.front() != req)
			return;

		execNextFSRequest(FSCtrlRequestType);
	}

	void AsyncChannelManager::addFSDataRequest(IFileSystemDataRequest* req)
	{
		m_fsDataQueue.push(req);
		if (m_fsDataQueue.front() != req)
			return;

		execNextFSRequest(FSDataRequestType);
	}

	void AsyncChannelManager::execNextFSRequest(FSRequestType type)
	{
		// Если обе очереди пусты, отправляем запрос на выход из файловой системы
		if (m_fsCtrlQueue.empty() && m_fsDataQueue.empty()) {
			// Обязательно сбрасываем состояние авторизации еще до результатов запроса. 
			// Не важно как выполнится текущий запрос. При любом последующем запросе к файловой системе
			// сначала отправится запрос на авторизацию в файловую систему.
			m_fsLoginState = NotLoggedInState;
			auto req = new FileSystemCommandRequest("QUIT", this);
			start(req, [=] {
				if (req->error()) {
					addErrorMsg(QString("Не удалось выполнить выход из файловой системы устройства: %1").arg(req->errorFormated()));
					return;
				}

				addInfoMsg(QString("Выход из файловой системы устройства выполнен успешно"));
			});
			return;
		}

		if (LoggedInState != m_fsLoginState) {
			// Если в данный момент уже отправлен запрос на авторизацию, ничего не делаем.
			if (LoggingInState == m_fsLoginState) {
				return;
			}

			m_fsLoginState = LoggingInState;
			auto req = new FileSystemCommandRequest(QString("LGIN %1 %2").arg("admin", "admin"), this);
			start(req, [=] {
				if (!req->error()) {
					m_fsLoginState = LoggedInState;
					addInfoMsg(QString("Вход в файловую систему устройства выполнен успешно"));
					execNextFSRequest(FSAnyRequestType);
					return;
				}
				
				m_fsLoginState = NotLoggedInState;
				const QString prefix = "Не удалось выполнить вход в файловую сиситему устройства";
				while (!m_fsCtrlQueue.empty()) {
					auto r = m_fsCtrlQueue.front();
					m_fsCtrlQueue.pop();
					r->setError(req->error(), req->errorString(), prefix);
					r->finished();
				}

				while (!m_fsDataQueue.empty()) {
					auto r = m_fsDataQueue.front();
					m_fsDataQueue.pop();
					r->setError(req->error(), req->errorString(), prefix);
					r->finished();
				}
			});
			return;
		}

		if ((FSCtrlRequestType & type) && !m_fsCtrlQueue.empty()) {
			auto req = m_fsCtrlQueue.front();
			connect(req, &IRequest::finished, this, &AsyncChannelManager::onFSCtrlRequestFinished);
			start(req);
		}

		if ((FSDataRequestType & type) && !m_fsDataQueue.empty()) {
			auto req = m_fsDataQueue.front();
			connect(req, &IRequest::finished, this, &AsyncChannelManager::onFSDataRequestFinished);
			start(req);
		}
	}
} // namespace
