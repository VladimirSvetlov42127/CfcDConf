#pragma once

#include <dpc/sybus/channel/async/manager/IRequest.h>
#include <dpc/sybus/ParamPack.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT WriteParamRequest : public IRequest
	{
		Q_OBJECT

	public:
		explicit WriteParamRequest(ParamPackPtr pack, const ChannelRequest& r, QObject* parent = nullptr);
		explicit WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, QObject* parent = nullptr);
		explicit WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, int board, QObject* parent = nullptr);
		explicit WriteParamRequest(ParamPackPtr pack, uint16_t addr, uint16_t index, int board, uint16_t subAddr, QObject *parent = nullptr);		
		
		ChannelRequest channelRequest() const { return m_channelRequest; }
		ParamPackPtr pack() const { return m_pack; }
		uint16_t writtenCount() const { return m_writtenCount; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onReplyFinished();

	private:
		void init();
		void writeMoreData();
		uint16_t maxDataPerRequest() const { return m_maxDataPerRequest; }

	private:
		ChannelRequest m_channelRequest;
		ChannelReply* m_reply;

		ParamPackPtr m_pack;
		uint16_t m_writtenCount;
		uint16_t m_maxDataPerRequest;
	};
} // namespace