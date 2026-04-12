#ifndef DINACPSIGNAL_H
#define DINACPSIGNAL_H

#include "service_manager/signals/din_signal.h"

class DinAcpSignal : public DinSignal
{
public:
    DinAcpSignal(const Config &config);
    virtual ~DinAcpSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Acp; }
    bool init(DcController* config) override;

    std::optional<uint8_t> drebezg() const;
    void setDrebezg(uint8_t value);

    std::optional<uint8_t> drebezgTimer() const;
    void setDrebezgTimer(uint8_t value);

    std::optional<uint8_t> drebezgNoise() const;
    void setDrebezgNoise(uint8_t value);

    std::optional<bool> inversionFlag() const;
    void setInversionFlag(bool flag);

    std::optional<double> thresholdChain() const;
    void setThresholdChain(double value);

    std::optional<double> threshold0() const;
    void setThreshold0(double value);

    std::optional<double> threshold1() const;
    void setThreshold1(double value);

    std::optional<double> thresholdKZ() const;
    void setThresholdKZ(double value);

private:
    ParameterElement* m_drebezg = nullptr;
    ParameterElement* m_drebezgTimer = nullptr;
    ParameterElement* m_drebezgNoise = nullptr;
    ParameterElement* m_inversion = nullptr;
    ParameterElement* m_thresholdChain = nullptr;
    ParameterElement* m_threshold0 = nullptr;
    ParameterElement* m_threshold1 = nullptr;
    ParameterElement* m_thresholdKZ = nullptr;
};

#endif // DINACPSIGNAL_H
