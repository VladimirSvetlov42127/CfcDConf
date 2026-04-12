#include "signal_manager.h"

#include <db/dc_db_manager.h>

namespace {

template<typename Container>
bool removeFromContainer(Container& container, Signal* signal)
{
    auto it = std::find_if(container.begin(), container.end(),
                           [signal](const auto& pair) { return pair.second.get() == signal; });

    if (it != container.end()) {
        container.erase(it);
        return true;
    }

    return false;
}

} // namespace

SignalManager::SignalManager(DcController *config)
    : m_config{config}
{
}

bool SignalManager::init()
{
    auto uid = config()->uid();
    if (uid) {
        auto query = QString("DELETE FROM matrix_signals");
        gDbManager.execute(uid, query);
    }

    auto dpParameter = config()->paramsRegistry().parameter(SP_DIN_DP_COUPLES);
    if (dpParameter)
        for(size_t i = 0; i < dpParameter->profilesCount(); i++) {
            auto dpinSignal = std::make_unique<DPinSignal>(i);
            if (dpinSignal->init(config()))
                m_dpins.emplace(i, std::move(dpinSignal));
        }

    rebind();
    return true;
}

void SignalManager::clearBindings()
{
    for(auto &[id, dout]: m_douts) {
        dout->setSource(nullptr);
    }
}

void SignalManager::rebind()
{
    for(auto &[id, dout]: m_douts) {
        auto bindElement = static_cast<const DoutSignal*>(dout.get())->bindElement();
        if (!bindElement)
            continue;

        dout->setSource(getSignal<DinSignal>(bindElement->value().toUInt()));
    }
}

std::vector<DinVirtualSignal *> SignalManager::busyVDins() const
{
    auto result = getSignals<DinVirtualSignal>();
    auto newEnd = std::remove_if(result.begin(), result.end(), [](const auto *s) { return !s->source(); });
    result.erase(newEnd, result.end());
    return result;
}

std::vector<DinVirtualSignal *> SignalManager::freeVDins() const
{
    auto result = getSignals<DinVirtualSignal>();
    auto newEnd = std::remove_if(result.begin(), result.end(), [](const auto *s) { return s->source(); });
    result.erase(newEnd, result.end());
    return result;
}

void SignalManager::addSignal(DinSignal::UPtr signal)
{
    if (!signal || !signal->init(config()))
        return;

    m_dins.emplace(signal->internalID(), std::move(signal));
}

void SignalManager::addSignal(AinSignal::UPtr signal)
{
    if (!signal || !signal->init(config()))
        return;

    m_ains.emplace(signal->internalID(), std::move(signal));
}

void SignalManager::addSignal(CinSignal::UPtr signal)
{
    if (!signal || !signal->init(config()))
        return;

    m_cins.emplace(signal->internalID(), std::move(signal));
}

void SignalManager::addSignal(DoutSignal::UPtr signal)
{
    if (!signal || !signal->init(config()))
        return;

    m_douts.emplace(signal->internalID(), std::move(signal));
}

bool SignalManager::removeSignal(Signal *signal)
{
    if (!signal)
        return false;

    return removeFromContainer(m_dins, signal) ||
            removeFromContainer(m_ains, signal) ||
            removeFromContainer(m_cins, signal) ||
            removeFromContainer(m_douts, signal);
}

std::vector<DPinSignal *> SignalManager::getDPinSignals() const
{
    std::vector<DPinSignal*> result;
    result.reserve(m_dpins.size());
    for(auto &[id, dpin]: m_dpins)
        result.emplace_back(dpin.get());

    return result;
}

DcController *SignalManager::config() const
{
    return m_config;
}
