#include "AsyncRs485Channel.h"

#include <qdebug.h>

namespace Dpc::Sybus
{
	template class AsyncRs485OverGateChannel<AsyncTcpChannel, RS485vTCP>;
	template class AsyncRs485OverGateChannel<AsyncVComChannel, RS485vVCOM>;

	template<class Gate, ChannelType T>
	AsyncRs485OverGateChannel<Gate, T>::AsyncRs485OverGateChannel(const SettingsType& settings, QObject *parent) :
		Gate(settings, parent),
		m_rs485Port(settings.value(Rs485PortSetting).toUInt()),
		m_rs485Device(settings.value(Rs485DeviceSetting).toUInt())
	{
	}

	template<class Gate, ChannelType T>
	SettingsType AsyncRs485OverGateChannel<Gate, T>::settings() const
	{
		auto res = Gate::settings();
		res[Rs485PortSetting] = m_rs485Port;
		res[Rs485DeviceSetting] = m_rs485Device;
		return res;
	}

	template<class Gate, ChannelType T>
	QByteArray AsyncRs485OverGateChannel<Gate, T>::packBeforeSend(const QByteArray & data) const
	{
		return Gate::encapsulate(m_rs485Port, m_rs485Device, data);
	}

	template<class Gate, ChannelType T>
	QByteArray AsyncRs485OverGateChannel<Gate, T>::unpackAfterRead(const QByteArray & data) const
	{
		return Gate::unpackAfterRead(data);
	}
} // namespace
