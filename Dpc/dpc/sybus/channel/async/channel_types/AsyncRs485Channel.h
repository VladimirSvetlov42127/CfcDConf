#pragma once

#include <dpc/sybus/channel/async/channel_types/AsyncTcpChannel.h>
#include <dpc/sybus/channel/async/channel_types/AsyncVComChannel.h>

namespace Dpc::Sybus
{
	template<class Gate, ChannelType T>
	class DPC_EXPORT AsyncRs485OverGateChannel : public Gate
	{
	public:
		AsyncRs485OverGateChannel(const SettingsType &settings, QObject *parent = nullptr);

		virtual ChannelType type() const override { return T; }
		virtual SettingsType settings() const override;		

	protected:
		virtual QByteArray packBeforeSend(const QByteArray &data) const override;
		virtual QByteArray unpackAfterRead(const QByteArray &data) const override;
		virtual bool isAlwaysResponseNecessary() const override { return true; }

	private:
		uint8_t m_rs485Port;
		uint8_t m_rs485Device;
	};

	// сделано так, потому что using обЪявления нельзя сделать forward declaration
	class DPC_EXPORT AsyncRs485OverTcp : public AsyncRs485OverGateChannel<AsyncTcpChannel, RS485vTCP> 
	{
		using AsyncRs485OverGateChannel::AsyncRs485OverGateChannel; 
	};

	class DPC_EXPORT AsyncRs485OverVCom : public AsyncRs485OverGateChannel<AsyncVComChannel, RS485vVCOM> 
	{ 
		using AsyncRs485OverGateChannel::AsyncRs485OverGateChannel; 
	};

} // namespace