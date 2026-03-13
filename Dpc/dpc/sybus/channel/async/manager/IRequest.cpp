#include "IRequest.h"

#include <qdebug.h>

//#define TRACE

#ifdef TRACE
#define TRACE_MSG(msg) qDebug().noquote() << DEBUG_PREFIX << msg;
#else
#define TRACE_MSG(msg) 
#endif // TRACE

namespace {
	const QString DEBUG_PREFIX = "[IRequest]";

	int gCount = 0;

} // namespace

namespace Dpc::Sybus
{
	IRequest::IRequest(QObject* parent) : 
		QObject(parent), 
		m_state(Idle),
		m_manager(nullptr),
		m_err(0)
	{
		gCount++;
	}

	IRequest::~IRequest()
	{
		TRACE_MSG(QString("Destructor, left = %1").arg(--gCount));
	}

	IRequest::RequestState IRequest::state() const
	{
		return m_state;
	}

	uint16_t IRequest::error() const
	{
		return m_err;
	}

	QString IRequest::errorString() const
	{
		return m_errStr;
	}

	QString IRequest::errorFormated() const
	{
		if (!error())
			return QString();

		auto hexErr = error() > std::numeric_limits<std::underlying_type<ChannelReply::ReturnCode>::type>::max() ? 
			toHex(error()) : toHex(static_cast<ChannelReply::ReturnCode>(error()));
		return QString("%1(%2)").arg(errorString()).arg(hexErr);
	}

	void IRequest::start()
	{
		TRACE_MSG(QString("Start: %1").arg(name()));

		m_err = 0;
		m_errStr = QString();
		m_state = Running;

		run();
	}

	QString IRequest::name() const
	{
		return QString();
	}

	AsyncChannelManager* IRequest::manager() const
	{
		return m_manager;
	}

	void IRequest::exec(IRequest* req, bool forwardMsgSignals)
	{
		if (forwardMsgSignals) {
			connect(req, &IRequest::infoMsg, this, &IRequest::infoMsg, Qt::UniqueConnection);
			connect(req, &IRequest::errorMsg, this, &IRequest::errorMsg, Qt::UniqueConnection);
			connect(req, &IRequest::infoMsg, this, &IRequest::infoMsg, Qt::UniqueConnection);
		}

		req->setManager(manager());
		req->start();
	}

	void IRequest::setError(uint16_t err, const QString& errStr, const QString& msgPrefix, const QString& msgPostfix)
	{
		m_err = err;
		m_errStr = errStr;
		
		QString msg = msgPrefix;
		if (!msg.isEmpty()) {
			msg.append(": ");
		}
		else if (auto nm = name(); !nm.isEmpty()) {
			msg = nm;
			msg.append(": ");
		}

		msg.append(errorFormated());
		if (!msgPostfix.isEmpty())
			msg.append(QString(": %1").arg(msgPostfix));

		addErrorMsg(msg);
	}

	void IRequest::finish()
	{
		TRACE_MSG(QString("Finished: %1").arg(name()));

		m_state = Finished;
		emit finished();
	}

	void IRequest::addProgressValue(int value)
	{
		emit progress(value);
	}

	void IRequest::addInfoMsg(const QString& msg)
	{
		emit infoMsg(msg);
	}

	void IRequest::addErrorMsg(const QString& msg)
	{
		emit errorMsg(msg);
	}

	void IRequest::addWarningMsg(const QString& msg)
	{
		emit warningMsg(msg);
	}

	QString IRequest::successString()
	{
		return "Выполнено";
	}

	void IRequest::setState(RequestState state)
	{
		m_state = state;
	}

	void IRequest::setManager(AsyncChannelManager* manager)
	{
		m_manager = manager;
	}

} // namespace