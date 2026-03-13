#pragma once

#include <dpc/sybus/channel/Channel.h>
#include <dpc/sybus/channel/async/AsyncChannel.h>

namespace Dpc::Sybus
{
	namespace ChannelsFactory
	{
		DPC_EXPORT ChannelPtr createChannel(const SettingsType& settings);
		DPC_EXPORT AsyncChannelPtr createAsyncChannel(const SettingsType& settings);
	};
} // namespace