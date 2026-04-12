#ifndef SIGNALFACTORY_H
#define SIGNALFACTORY_H

#include "service_manager/signals/din_signal.h"
#include "service_manager/signals/ain_signal.h"
#include "service_manager/signals/cin_signal.h"
#include "service_manager/signals/dout_signal.h"

class SignalFactory
{
public:
    SignalFactory();

    DinSignal::UPtr createDinSignal(Signal::Subtype subtype, const Signal::Config &config) const;
    AinSignal::UPtr createAinSignal(Signal::Subtype subtype, const Signal::Config &config) const;
    CinSignal::UPtr createCinSignal(Signal::Subtype subtype, const Signal::Config &config) const;
    DoutSignal::UPtr createDoutSignal(Signal::Subtype subtype, const Signal::Config &config) const;

private:
    using DinCreatorFunc = std::function<DinSignal::UPtr(const Signal::Config &config)>;
    using AinCreatorFunc = std::function<AinSignal::UPtr(const Signal::Config &config)>;
    using CinCreatorFunc = std::function<CinSignal::UPtr(const Signal::Config &config)>;
    using DoutCreatorFunc = std::function<DoutSignal::UPtr(const Signal::Config &config)>;

    template<typename T>
    void registerCreator(Signal::Subtype subtype);

    std::unordered_map<size_t, DinCreatorFunc> m_dinCreators;
    std::unordered_map<size_t, AinCreatorFunc> m_ainCreators;
    std::unordered_map<size_t, CinCreatorFunc> m_cinCreators;
    std::unordered_map<size_t, DoutCreatorFunc> m_doutCreators;
};

#endif // SIGNALFACTORY_H
