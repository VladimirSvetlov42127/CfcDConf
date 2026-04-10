#ifndef AINSIGNAL_H
#define AINSIGNAL_H

#include <memory>

#include "service_manager/signals/signal.h"
#include "data_model/parameters/parameter_element.h"

class AinSignal : public Signal
{
public:
    using UPtr = std::unique_ptr<AinSignal>;

    AinSignal(const Config &config);
    virtual ~AinSignal() = default;

    Signal::Type type() const override { return Signal::Type::Ain; }
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

    std::optional<uint8_t> apertureType() const;
    void setApertureType(uint8_t value);

    std::optional<double> apertureVal() const;
    void setApertureVal(double value);

    std::optional<double> thresholdSensibility() const;
    void setThresholdSensibility(double value);

    std::optional<double> threshold0() const;
    void setThreshold0(double value);

    std::optional<double> threshold1() const;
    void setThreshold1(double value);

private:
    ElementBit m_iec101SelectFlag;
    ElementBit m_iec101Group1Flag;
    ElementBit m_iec101Group2Flag;
    ElementBit m_iec101BackgroundFlag;
    ElementBit m_spodesSelectFlag;

    ParameterElement* m_apertureType = nullptr;
    ParameterElement* m_apertureVal = nullptr;
    ParameterElement* m_thresholdSensibility = nullptr;
    ParameterElement* m_threshold0 = nullptr;
    ParameterElement* m_threshold1 = nullptr;
};

#endif // AINSIGNAL_H
