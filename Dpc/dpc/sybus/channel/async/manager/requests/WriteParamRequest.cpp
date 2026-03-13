#include "WriteParamRequest.h"

namespace {
	static const uint8_t DEFAULT_MAX_DATA_PER_REQUEST = 0x40;
} // namespace

namespace Dpc::Sybus
{
	WriteParamRequest::WriteParamRequest(ParamPackPtr pack, const ChannelRequest& r, QObject* parent) :
		IRequest(parent),
		m_channelRequest(r),
		m_reply(nullptr),
		m_pack(pack),
		m_writtenCount(0),
		m_maxDataPerRequest(DEFAULT_MAX_DATA_PER_REQUEST)
	{
		init();
	}

	WriteParamRequest::WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::WriteRequest, addr, index, 0 },
		m_reply(nullptr),
		m_pack(pack),
		m_writtenCount(0),
		m_maxDataPerRequest(DEFAULT_MAX_DATA_PER_REQUEST)
	{
		init();
	}

	WriteParamRequest::WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, int board, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ ChannelRequest::WriteRequest, addr, index, 0, board },
		m_reply(nullptr),
		m_pack(pack),
		m_writtenCount(0),
		m_maxDataPerRequest(DEFAULT_MAX_DATA_PER_REQUEST)
	{
		init();
	}

	WriteParamRequest::WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, int board, uint16_t subAddr, QObject* parent) :
		IRequest(parent),
		m_channelRequest{ChannelRequest::WriteRequest, addr, index, 0, board, subAddr },
		m_reply(nullptr),
		m_pack(pack),
		m_writtenCount(0),
		m_maxDataPerRequest(DEFAULT_MAX_DATA_PER_REQUEST)
	{
		init();
	}	

	QString WriteParamRequest::name() const
	{
		return m_channelRequest.toString();;
	}

	void WriteParamRequest::run()
	{
		m_maxDataPerRequest = manager()->writeBufferSize() - manager()->channel()->writeHeaderSize(channelRequest().board);
		m_writtenCount = 0;

		writeMoreData();
	}

	void WriteParamRequest::onReplyFinished()
	{
		QScopedPointer<ChannelReply, QScopedPointerDeleteLater> sp(m_reply);

		auto prefix = QString("Запись %1").arg(m_reply->request().toString());

		auto p = ParamPack::create(m_reply->request().dataType);
		p->appendData(m_reply->request().data);
		auto postFix = QString("%1{%2}").arg(m_reply->request().count).arg(p->toString());

		if (m_reply->error()) {
			setError(m_reply->error(), m_reply->errorString(), prefix, postFix);
			finish();
			return;
		}

		if (m_reply->returnCode()) {
			auto msg = QString("%1: %2(%3): %4").arg(prefix).arg(m_reply->returnCodeString()).arg(toHex(m_reply->returnCode())).arg(postFix);

			// обработать ситуации, когда идет ошибка busy
			/*if (ChannelReply::Busy == m_reply->returnCode()) {

			}*/

			if (ChannelReply::InvalidIndex == m_reply->returnCode() && writtenCount()) {
				addWarningMsg(msg);
				finish();
				return;
			}

			setError(m_reply->returnCode(), m_reply->returnCodeString(), prefix, postFix);
			finish();
			return;
		}

		addInfoMsg(QString("%1: %2: %3").arg(prefix).arg(successString()).arg(postFix));
		m_writtenCount += m_reply->request().count;
		writeMoreData();
	}

	void WriteParamRequest::init()
	{
		m_channelRequest.type = ChannelRequest::WriteRequest;
		m_channelRequest.count = m_pack->count();
		m_channelRequest.dataType = m_pack->typeId();		
		m_channelRequest.data.clear();	
	}

	void WriteParamRequest::writeMoreData()
	{
		if (writtenCount() >= m_pack->count()) {
			finish();
			return;
		}

		//auto p = ParamPack::create(m_pack->typeId());
		auto req = channelRequest();
		for (req.count = 0; writtenCount() + req.count < m_pack->count(); req.count++) {
			auto data = m_pack->data(writtenCount() + req.count);
			if (req.data.size() + data.size() > maxDataPerRequest())
				break;

			req.data.append(data);
			//p->appendData(data);
		}
		req.index += writtenCount();

		//addInfoMsg(QString("Запись занчений %1: %2{%3}").arg(req.toString()).arg(p->count()).arg(p->toString()));

		m_reply = manager()->channel()->exec(req);
		connect(m_reply, &ChannelReply::finished, this, &WriteParamRequest::onReplyFinished);
	}

} // namespace