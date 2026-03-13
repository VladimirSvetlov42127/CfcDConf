#include "ReadParamRequest.h"

namespace {
	static const uint8_t MAX_COUNT_PER_READ = 0xFF;
} // namespace

namespace Dpc::Sybus
{
	ReadParamRequest::ReadParamRequest(const ChannelRequest& r, QObject* parent) :
		IRequest(parent),
		m_channelRequest(r),
		m_reply(nullptr)
	{
		init();
	}

	ReadParamRequest::ReadParamRequest(uint16_t addr, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::ReadRequest, addr },
		m_reply(nullptr)
	{
		init();
	}

	ReadParamRequest::ReadParamRequest(uint16_t addr, uint16_t index, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::ReadRequest, addr, index },
		m_reply(nullptr)
	{
		init();
	}

	ReadParamRequest::ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::ReadRequest, addr, index, count },
		m_reply(nullptr)
	{
		init();
	}

	ReadParamRequest::ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, int board, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::ReadRequest, addr, index, count, board },
		m_reply(nullptr)
	{
		init();
	}
	ReadParamRequest::ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, int board, uint16_t subAddr, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::ReadRequest, addr, index, count, board, subAddr },
		m_reply(nullptr)
	{
		init();
	}

	QString ReadParamRequest::name() const
	{
		return m_channelRequest.toString();
	}

	void ReadParamRequest::run()
	{
		m_pack.reset();
		getMoreData();
	}

	void ReadParamRequest::onReplyFinished()
	{
		QScopedPointer<ChannelReply, QScopedPointerDeleteLater> sp(m_reply);
		auto prefix = QString("Значения %1").arg(m_reply->request().toString());

		if (m_reply->error()) {
			m_pack.reset();
			setError(m_reply->error(), m_reply->errorString(), prefix);
			finish();
			return;
		}

		if (m_reply->returnCode()) {	
			auto msg = QString("%1: %2(%3)").arg(prefix).arg(m_reply->returnCodeString()).arg(toHex(m_reply->returnCode()));

			// обработать ситуации, когда идет ошибка busy
			/*if (ChannelReply::Busy == m_reply->returnCode()) {

			}*/
			
			if (ChannelReply::InvalidIndex == m_reply->returnCode() && m_pack) {				
				addWarningMsg(msg);
				finish();
				return;
			}
			
			setError(m_reply->returnCode(), m_reply->returnCodeString(), prefix);
			finish();
			return;
		}

		{
			// Для вывода сообщения о полученых данных
			auto p = ParamPack::create(m_reply->dataType());
			p->appendData(m_reply->data());
			addInfoMsg(QString("%1: %2{%3}")
				.arg(prefix)
				.arg(p->count())
				.arg(p->toString()));
		}

		if (!m_pack) {
			m_pack = ParamPack::create(m_reply->dataType());
		}

		m_pack->appendData(m_reply->data());
		getMoreData();
	}

	void ReadParamRequest::init()
	{
		m_channelRequest.type = ChannelRequest::ReadRequest;
	}

	uint16_t ReadParamRequest::readedCount() const
	{
		return m_pack ? m_pack->count() : 0;
	}

	void ReadParamRequest::getMoreData()
	{
		if (readedCount() >= channelRequest().count) {
			finish();
			return;
		}

		auto req = channelRequest();
		req.count = readedCount() + MAX_COUNT_PER_READ < channelRequest().count ? MAX_COUNT_PER_READ : channelRequest().count - readedCount();
		req.index += readedCount();

		//addInfoMsg(QString("Чтение значений %1").arg(req.toString()));

		m_reply = manager()->channel()->exec(req);
		connect(m_reply, &ChannelReply::finished, this, &ReadParamRequest::onReplyFinished);
	}

} // namespace