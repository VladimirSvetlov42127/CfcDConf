#include "signal_factory.h"

#include "service_manager/signals/ain_acp_signal.h"
#include "service_manager/signals/ain_physical_signal.h"
#include "service_manager/signals/ain_virtual_archive_signal.h"
#include "service_manager/signals/ain_virtual_signal.h"
#include "service_manager/signals/ain_remote_signal.h"
#include "service_manager/signals/ain_remote_internal_signal.h"

#include "service_manager/signals/din_acp_signal.h"
#include "service_manager/signals/din_physical_signal.h"
#include "service_manager/signals/din_logical_signal.h"
#include "service_manager/signals/din_virtual_signal.h"
#include "service_manager/signals/din_remote_signal.h"
#include "service_manager/signals/din_remote_internal_signal.h"

#include "service_manager/signals/cin_physical_signal.h"
#include "service_manager/signals/cin_virtual_signal.h"
#include "service_manager/signals/cin_remote_signal.h"
#include "service_manager/signals/cin_remote_internal_signal.h"

#include "service_manager/signals/dout_led_signal.h"
#include "service_manager/signals/dout_physical_signal.h"
#include "service_manager/signals/dout_virtual_signal.h"
#include "service_manager/signals/dout_remote_signal.h"
#include "service_manager/signals/dout_remote_internal_signal.h"

namespace {

template<typename ContianerType>
auto createSignal(ContianerType container, Signal::Subtype subtype, const Signal::Config& config) -> typename ContianerType::mapped_type::result_type
{
    auto key = static_cast<size_t>(subtype);
    auto creatorIt = container.find(key);
    if (creatorIt != container.end())
        return creatorIt->second(config);

    return nullptr;
}

} // namespace

SignalFactory::SignalFactory()
{
    registerCreator<AinAcpSignal>(Signal::Subtype::Acp);
    registerCreator<AinPhysicalSignal>(Signal::Subtype::Physical);
    registerCreator<AinVirtualArchiveSignal>(Signal::Subtype::ArchiveVirtual);
    registerCreator<AinVirtualSignal>(Signal::Subtype::Virtual);
    registerCreator<AinRemoteSignal>(Signal::Subtype::Remote);
    registerCreator<AinRemoteInternalSignal>(Signal::Subtype::RemoteInternal);

    registerCreator<DinAcpSignal>(Signal::Subtype::Acp);
    registerCreator<DinPhysicalSignal>(Signal::Subtype::Physical);
    registerCreator<DinLogicalSignal>(Signal::Subtype::Logical);
    registerCreator<DinVirtualSignal>(Signal::Subtype::Virtual);
    registerCreator<DinRemoteSignal>(Signal::Subtype::Remote);
    registerCreator<DinRemoteInternalSignal>(Signal::Subtype::RemoteInternal);

    registerCreator<CinPhysicalSignal>(Signal::Subtype::Physical);
    registerCreator<CinVirtualSignal>(Signal::Subtype::Virtual);
    registerCreator<CinRemoteSignal>(Signal::Subtype::Remote);
    registerCreator<CinRemoteInternalSignal>(Signal::Subtype::RemoteInternal);

    registerCreator<DoutLedSignal>(Signal::Subtype::Led);
    registerCreator<DoutPhysicalSignal>(Signal::Subtype::Physical);
    registerCreator<DoutVirtualSignal>(Signal::Subtype::Virtual);
    registerCreator<DoutRemoteSignal>(Signal::Subtype::Remote);
    registerCreator<DoutRemoteInternalSignal>(Signal::Subtype::RemoteInternal);
}

DinSignal::UPtr SignalFactory::createDinSignal(Signal::Subtype subtype, const Signal::Config &config) const
{
    return createSignal(m_dinCreators, subtype, config);
}

AinSignal::UPtr SignalFactory::createAinSignal(Signal::Subtype subtype, const Signal::Config &config) const
{
    return createSignal(m_ainCreators, subtype, config);
}

CinSignal::UPtr SignalFactory::createCinSignal(Signal::Subtype subtype, const Signal::Config &config) const
{
    return createSignal(m_cinCreators, subtype, config);
}

DoutSignal::UPtr SignalFactory::createDoutSignal(Signal::Subtype subtype, const Signal::Config &config) const
{
    return createSignal(m_doutCreators, subtype, config);
}

template<typename SignalType>
void SignalFactory::registerCreator(Signal::Subtype subtype)
{
    auto key = static_cast<size_t>(subtype);
    if constexpr (std::is_base_of_v<DinSignal, SignalType>)
        m_dinCreators[key] = [](const Signal::Config& config) { return std::make_unique<SignalType>(config); };
    else if constexpr (std::is_base_of_v<AinSignal, SignalType>)
        m_ainCreators[key] = [](const Signal::Config& config) { return std::make_unique<SignalType>(config); };
    else if constexpr (std::is_base_of_v<CinSignal, SignalType>)
        m_cinCreators[key] = [](const Signal::Config& config) { return std::make_unique<SignalType>(config); };
    else if constexpr (std::is_base_of_v<DoutSignal, SignalType>)
        m_doutCreators[key] = [](const Signal::Config& config) { return std::make_unique<SignalType>(config); };
}
