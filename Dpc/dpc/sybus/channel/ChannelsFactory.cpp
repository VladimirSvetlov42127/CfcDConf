#include "ChannelsFactory.h"

#include <dpc/sybus/channel/channel_types/VComChannel.h>
#include <dpc/sybus/channel/channel_types/TcpChannel.h>
#include <dpc/sybus/channel/channel_types/Rs485Channel.h>

#include <dpc/sybus/channel/async/channel_types/AsyncVComChannel.h>
#include <dpc/sybus/channel/async/channel_types/AsyncTcpChannel.h>
#include <dpc/sybus/channel/async/channel_types/AsyncRs485Channel.h>

namespace Dpc::Sybus
{
	namespace ChannelsFactory 
	{
		ChannelPtr createChannel(const SettingsType & settings)
		{
			switch (settings.value(TypeSetting).toUInt())
			{
			case VCOM: return std::make_shared<VComChannel>(settings);
			case TCP: return std::make_shared<TcpChannel>(settings);
			case RS485vTCP: return std::make_shared<Rs485OverTcp>(settings);
			case RS485vVCOM: return std::make_shared<Rs485OverVCom>(settings);
			default: return ChannelPtr();
			}
		}

		AsyncChannelPtr createAsyncChannel(const SettingsType& settings)
		{
			switch (settings.value(TypeSetting).toUInt())
			{
			case VCOM: return std::make_shared<AsyncVComChannel>(settings);
			case TCP: return std::make_shared<AsyncTcpChannel>(settings);
			case RS485vTCP: return std::make_shared<AsyncRs485OverTcp>(settings);
			case RS485vVCOM: return std::make_shared<AsyncRs485OverVCom>(settings);
			default: return AsyncChannelPtr();
			}
		}

	} // namespace ChannelsFactory

} // namespace Dpc::Sybus