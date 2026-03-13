#pragma once

#include <dpc/sybus/channel/async/manager/IRequest.h>
#include <dpc/sybus/ParamPack.h>

namespace Dpc::Sybus
{
	class DPC_EXPORT ReadParamRequest : public IRequest
	{
		Q_OBJECT

	public:
		explicit ReadParamRequest(const ChannelRequest& r, QObject* parent = nullptr);
		explicit ReadParamRequest(uint16_t addr, QObject* parent = nullptr);
		explicit ReadParamRequest(uint16_t addr, uint16_t index, QObject* parent = nullptr);
		explicit ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, QObject* parent = nullptr);
		explicit ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, int board, QObject* parent = nullptr);
		explicit ReadParamRequest(uint16_t addr, uint16_t index, uint16_t count, int board, uint16_t subAddr, QObject* parent = nullptr);

		ChannelRequest channelRequest() const { return m_channelRequest; }
		ParamPackPtr pack() const { return m_pack; }

		virtual QString name() const override;

	protected:
		virtual void run() override;

	private slots:
		void onReplyFinished();

	private:
		void init();
		uint16_t readedCount() const;
		void getMoreData();

	private:
		ChannelRequest m_channelRequest;
		ChannelReply* m_reply;
		ParamPackPtr m_pack;	
	};
} // namespace