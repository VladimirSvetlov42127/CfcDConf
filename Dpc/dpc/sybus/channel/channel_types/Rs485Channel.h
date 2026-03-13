#pragma once

#include <dpc/sybus/channel/channel_types/TcpChannel.h>
#include <dpc/sybus/channel/channel_types/VComChannel.h>

namespace Dpc::Sybus
{
	template<class Gate, ChannelType T>
	class DPC_EXPORT Rs485OverGateChannel : public Gate
	{
	public:
		Rs485OverGateChannel(const SettingsType &settings, QObject *parent = nullptr);

		virtual ChannelType type() const override { return T; }
		virtual SettingsType settings() const override;

	protected:
		virtual QByteArray packBeforeSend(const QByteArray &data) const override;
		virtual QByteArray unpackAfterRead(const QByteArray &data) const override;
		virtual std::pair<uint16_t, uint16_t> onGetBuffersSize() override;

		virtual bool beginChangeDateTime() override;
		virtual bool endChangeDateTime() override;

	private:
		uint8_t m_rs485Port;
		uint8_t m_rs485Device;
		bool m_isActive;

		int8_t m_currentSyncProtocol;
	};

	// сделано так, потому что using обЪявления нельзя сделать forward declaration
	class DPC_EXPORT Rs485OverTcp : public Rs485OverGateChannel<TcpChannel, RS485vTCP> { using Rs485OverGateChannel::Rs485OverGateChannel; };
	class DPC_EXPORT Rs485OverVCom : public Rs485OverGateChannel<VComChannel, RS485vVCOM> { using Rs485OverGateChannel::Rs485OverGateChannel; };
} // namespace