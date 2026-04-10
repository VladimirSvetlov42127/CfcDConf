#ifndef CINSIGNAL_H
#define CINSIGNAL_H

#include <memory>

#include "service_manager/signals/signal.h"
#include "data_model/parameters/parameter_element.h"

class CinSignal : public Signal
{
public:
    using UPtr = std::unique_ptr<CinSignal>;

    CinSignal(const Config &config);
    virtual ~CinSignal() = default;

    Signal::Type type() const override { return Signal::Type::Cin; }
    Signal::Subtype subtype() const override { return Signal::Subtype::Undef; }
    bool init(DcController* config) override;

    auto iec101SelectFlag() const { return m_iec101SelectFlag.value(); }
    void setIec101SelectFlag(bool flag)  { m_iec101SelectFlag.setValue(flag); }

    auto iec101Group1Flag() const { return m_iec101Group1Flag.value(); }
    void setIec101Group1Flag(bool flag)  { m_iec101Group1Flag.setValue(flag); }

    auto iec101Group2Flag() const { return m_iec101Group2Flag.value(); }
    void setIec101Group2Flag(bool flag)  { m_iec101Group2Flag.setValue(flag); }

    auto iec101BackgroundFlag() const { return m_iec101BackgroundFlag.value(); }
    void setIec101BackgroundFlag(bool flag)  { m_iec101BackgroundFlag.setValue(flag); }

    auto spodesSelectFlag() const { return m_spodesSelectFlag.value(); }
    void setSpodes101SelectFlag(bool flag)  { m_spodesSelectFlag.setValue(flag); }

private:
    ElementBit m_iec101SelectFlag;
    ElementBit m_iec101Group1Flag;
    ElementBit m_iec101Group2Flag;
    ElementBit m_iec101BackgroundFlag;
    ElementBit m_spodesSelectFlag;
};

#endif // CINSIGNAL_H
