#include "Rs485Channel.h"

#include <qdebug.h>

namespace Dpc::Sybus
{
	template class Rs485OverGateChannel<TcpChannel, RS485vTCP>;
	template class Rs485OverGateChannel<VComChannel, RS485vVCOM>;

	template<class Gate, ChannelType T>
	Rs485OverGateChannel<Gate, T>::Rs485OverGateChannel(const SettingsType& settings, QObject *parent) :
		Gate(settings, parent),
		m_rs485Port(settings.value(Rs485PortSetting).toUInt()),
		m_rs485Device(settings.value(Rs485DeviceSetting).toUInt()),
		m_currentSyncProtocol(-1)
	{
		m_isActive = true;
		Gate::setWaitAfterReset(true);
	}

	template<class Gate, ChannelType T>
	SettingsType Rs485OverGateChannel<Gate, T>::settings() const
	{
		auto res = Gate::settings();
		res[Rs485PortSetting] = m_rs485Port;
		res[Rs485DeviceSetting] = m_rs485Device;
		return res;
	}

	template<class Gate, ChannelType T>
	QByteArray Rs485OverGateChannel<Gate, T>::packBeforeSend(const QByteArray & data) const
	{
		if (!m_isActive)
			return data;

		return Gate::encapsulate(m_rs485Port, m_rs485Device, data);
	}

	template<class Gate, ChannelType T>
	QByteArray Rs485OverGateChannel<Gate, T>::unpackAfterRead(const QByteArray & data) const
	{
		return Gate::unpackAfterRead(data);
	}

	template<class Gate, ChannelType T>
	std::pair<uint16_t, uint16_t> Rs485OverGateChannel<Gate, T>::onGetBuffersSize()
	{
		auto slaveBuffers = Gate::onGetBuffersSize();
		if (!slaveBuffers.first && !slaveBuffers.second)
			return std::make_pair(0, 0);

		m_isActive = false;
		auto masterBuffers = Gate::onGetBuffersSize();
		m_isActive = true;
		if (!masterBuffers.first && !masterBuffers.second)
			return std::make_pair(0, 0);

		auto rb = std::min(slaveBuffers.first, masterBuffers.first);
		auto wr = std::min(slaveBuffers.second, masterBuffers.second);
		return std::make_pair(rb, wr);
	}

	template<class Gate, ChannelType T>
	bool Rs485OverGateChannel<Gate, T>::beginChangeDateTime()
	{
		m_currentSyncProtocol = -1;
		auto syncProtocol = Gate::getSyncProtocol();
		if (syncProtocol < 0)
			return false;

		switch (syncProtocol) {
		case Channel::SYBUS_TSYNC:
		case Channel::IEC101_TSYNC:
		case Channel::MODBUSRTU_TSYNC:
		case Channel::SNTPGPRS_TSYNC:
		case Channel::IEC_103_TSYNC:
			return true;
		default:break;
		}

		if (!Gate::setSyncProtocol(Channel::SYBUS_TSYNC))
			return false;

		m_currentSyncProtocol = syncProtocol;
		return true;
	}

	template<class Gate, ChannelType T>
	bool Rs485OverGateChannel<Gate, T>::endChangeDateTime()
	{
		if (m_currentSyncProtocol < 0)
			return true;

		return Gate::setSyncProtocol(m_currentSyncProtocol);
	}
} // namespace
