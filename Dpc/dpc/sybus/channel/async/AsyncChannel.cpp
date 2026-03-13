#include "AsyncChannel.h"

#include <qdebug.h>
#include <qthread.h>

#include <dpc/sybus/channel/async/DeviceDefinition.h>

//#define TRACE

#ifdef TRACE
#define TRACE_MSG(msg) TraceObject __to(msg);
#else
#define TRACE_MSG(msg) 
#endif // TRACE

namespace {
	const QString DEBUG_PREFIX = "[Channel]";

	struct TraceObject
	{
		TraceObject(const QString& msg) {
			QString offset;
			for (size_t i = 0; i < tab; i++)
				offset.append("    ");
			qDebug().noquote() << QString("%1 %2%3").arg(DEBUG_PREFIX, offset, msg);
			tab++;
		}
		~TraceObject() { tab--; }

		static int tab;
	};

	int TraceObject::tab = 0;
} // namespace

namespace Dpc::Sybus
{
	AsyncChannel::AsyncChannel(QObject* parent) :
		QObject(parent),
		m_isScheduledNext(false)
	{
	}

	AsyncChannel::~AsyncChannel()
	{
		TRACE_MSG("destructor");
	}

	SettingsType AsyncChannel::settings() const
	{
		SettingsType res;
		res[TypeSetting] = type();
		return res;
	}

	void AsyncChannel::connectToDevice()
	{
		if (isConnectedToDevice())
			return;

		addInfoMsg("Connecting...");
		TRACE_MSG("Connecting...");
		doConnectToDevice();
	}

	void AsyncChannel::disconnectFromDevice()
	{
		if (!isConnectedToDevice())
			return;

		addInfoMsg("Disconnecting...");
		TRACE_MSG("Disconnection...");
		doDisconnectFromDevice();		
	}

	ChannelReply* AsyncChannel::exec(const ChannelRequest& req)
	{
		return addToQueue(req);
	}

	uint16_t AsyncChannel::writeHeaderSize(int board) const
	{
		auto size = packBeforeSend(Device::WriteRequest().toByteArray()).size();
		if (board > -1)
			size += Device::EncapsulatedRequest().toByteArray().size();
		return size;
	}

	void AsyncChannel::setError(ChannelReply::Error err, const QString& errMsg)
	{
		// Испустить сигнал error
		auto formatedErrorMsg = QString("%1(%2)").arg(errMsg).arg(toHex(err));
		addInfoMsg(QString("Error: %1").arg(formatedErrorMsg));
		TRACE_MSG(QString("Error: %1").arg(formatedErrorMsg));
		emit error(err, errMsg);

		// Завершить текущий запрос с текущей ошибкой
		finishNext(err, errMsg);

		// Закрыть соединение при транспортной ошибке
		if (ChannelReply::TransportError == err) {
			TRACE_MSG("DoDisconnectFromDevice");
			doDisconnectFromDevice();
		}	
	}

	void AsyncChannel::onConnectedToDevice()
	{
		addInfoMsg("Connected to device");
		TRACE_MSG("Connected to device");
		emit connectedToDevice();
	}

	void AsyncChannel::onDisconnectedFromDevice()
	{
		addInfoMsg("Disconnected from device");
		TRACE_MSG("Disconnected from device");
		
		while (!m_queue.empty())
			finishNext(ChannelReply::NotConnected);
		emit disconnectedFromDevice();
	}

	void AsyncChannel::onReceivedData(const QByteArray& data)
	{
		if (m_queue.empty())
			return;

		TRACE_MSG(QString("onRecivedData %1").arg(m_queue.size()));
		addInfoMsg(QString("RECV (%1) <- ").arg(data.size()).append(data.toHex(':')).toUpper());
		auto reply = m_queue.front();
		auto receivedData = !reply->request().toMaster ? unpackAfterRead(data) : data;

		auto handleBaseResponse = [reply](const Device::BaseResponse& response) {
			reply->setReturnCode(response.retCode);
			reply->setBusy(response.isBusy);
			reply->setNeedRestart(response.needRestart);
			if (response.msgType != Device::MSG_TYPE_RESPONSE)
				return ChannelReply::InvalidResponseMsgType;

			return ChannelReply::NoError;
		};

		ChannelReply::Error err = ChannelReply::NoError;
		if (reply->request().type == ChannelRequest::ReadRequest) {
			Device::ReadResponse response(receivedData);
			if (err = handleBaseResponse(response); !err) {
				reply->setDataType(response.type);
				reply->setData(response.data);

				if (ChannelReply::Good == reply->returnCode() && !reply->dataType())
					err = ChannelReply::UnexpectedParamType;
			}		
		}
		else if (reply->request().type == ChannelRequest::WriteRequest) {
			Device::WriteResponse response(receivedData);
			if (err = handleBaseResponse(response); !err) {
				reply->setDelay(response.msec);
			}
		}			

		if (!err) {
			if (reply->isBusy()) {
				if (reply->delay())
					addInfoMsg(QString("Device is busy for %1 msec").arg(reply->delay()));
				else
					addInfoMsg(QString("Device is busy"));
			}

			if (reply->needRestart()) {
				addInfoMsg("Device need restart to apply changes");
			}
		}
		
		finishNext(err);
	}

	QByteArray AsyncChannel::packBeforeSend(const QByteArray& data) const
	{
		return data;
	}

	QByteArray AsyncChannel::unpackAfterRead(const QByteArray& data) const
	{
		return data;
	}

	bool AsyncChannel::isAlwaysResponseNecessary() const
	{
		return false;
	}

	void AsyncChannel::addInfoMsg(const QString& msg)
	{
		//TRACE_MSG(msg);
		emit infoMsg(QString("%1 %2").arg(DEBUG_PREFIX).arg(msg));
	}

	QByteArray AsyncChannel::encapsulate(uint8_t port, uint8_t abNum, const QByteArray& data)
	{
		return Device::EncapsulatedRequest(port, abNum, data).toByteArray();
	}

	ChannelReply* AsyncChannel::addToQueue(const ChannelRequest& request)
	{
		auto reply = new ChannelReply(request);
		reply->setState(ChannelReply::Pending);
		m_queue.push(reply);
		TRACE_MSG(QString("addToQueue %1").arg(m_queue.size()));
		scheduleNext();
		return reply;
	}

	void AsyncChannel::scheduleNext()
	{		
		if (m_isScheduledNext)
			return;

		if (m_queue.empty())
			return;

		TRACE_MSG(QString("scheduleNext"));
		m_isScheduledNext = true;
		QMetaObject::invokeMethod(this, &AsyncChannel::execNext, Qt::QueuedConnection);
	}

	void AsyncChannel::execNext()
	{
		m_isScheduledNext = false;
		if (m_queue.empty())
			return;

		auto reply = m_queue.front();
		if (ChannelReply::Running == reply->state())
			return;			

		TRACE_MSG(QString("execNext %1 %2").arg(m_queue.size()).arg(reply->request().toString()));

		if (!isConnectedToDevice()) {
			finishNext(ChannelReply::NotConnected);
			return;
		}

		// Начинаем выполнение следующего запроса.		
		reply->setState(ChannelReply::Running);
		auto req = reply->request();

		// Определение нужно ли ждать ответа на запрос
		// Если запрос идет через шлюз или к плате, всегда ждем ответа на запрос, даже если указанно что он не нужен.
		if (!req.needResponse && (isAlwaysResponseNecessary() || req.board > -1))
			req.needResponse = true;

		// Создаем массив байтов для передачи на устройство, в зависимости от типа запроса.
		QByteArray reqData;
		switch (req.type)
		{
		case ChannelRequest::ReadRequest:
			reqData = req.subAddr ? Device::ExtendedReadRequest(req.addr, req.subAddr, req.index, req.count).toByteArray() :
				Device::ReadRequest(req.addr, req.index, req.count).toByteArray();
			break;
		case ChannelRequest::WriteRequest:
			reqData = req.subAddr ? Device::ExtendedWriteRequest(req.addr, req.subAddr, req.index, req.dataType, req.count, req.data).toByteArray() :
				Device::WriteRequest(req.addr, req.index, req.dataType, req.count, req.data).toByteArray();
			break;
		default:
			break;
		}

		// Если запрос идет к плате на 0-й шине rs485, упаковываем данные в заголовок, с соответсвующими параметрами.
		if (req.board > -1)
			reqData = encapsulate(0, req.board, reqData);

		// Если запрос идет не к основному устройству, упаковываем данные в заголовок определеный в наслединке.
		if (!req.toMaster)
			reqData = packBeforeSend(reqData);

		// Вызов функции отправки данных в устройство, определенной в наследнике.
		addInfoMsg(QString("SEND (%1) -> ").arg(reqData.size()).append(reqData.toHex(':')).toUpper());
		doSendData(reqData);

		// Если в запросе указанно что не нужен ответ, то завершаем его.
		if (!req.needResponse)
			finishNext();
	}

	void AsyncChannel::finishNext(ChannelReply::Error err, const QString& errMsg)
	{
		if (m_queue.empty())
			return;
		
		auto r = m_queue.front();
		m_queue.pop();
		TRACE_MSG(QString("finishNext(%1), Left(%2)").arg(err).arg(m_queue.size()));
		scheduleNext();

		r->setError(err, errMsg);
		r->setState(ChannelReply::Finished);		
	}

} // namespace