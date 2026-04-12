#ifndef DINPHYSICALSIGNAL_H
#define DINPHYSICALSIGNAL_H

#include "service_manager/signals/din_signal.h"

class DcBoard;

class DinPhysicalSignal : public DinSignal
{
public:
    DinPhysicalSignal(const Config &config);
    virtual ~DinPhysicalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Physical; }
    bool init(DcController* config) override;

    std::optional<uint8_t> drebezg() const;
    void setDrebezg(uint8_t value);

    std::optional<uint8_t> drebezgRatio() const;
    void setDrebezgRatio(uint8_t value);

    std::optional<uint8_t> drebezgTimer() const;
    void setDrebezgTimer(uint8_t value);

    std::optional<uint8_t> drebezgNoise() const;
    void setDrebezgNoise(uint8_t value);

    std::optional<bool> inversionFlag() const;
    void setInversionFlag(bool flag);

    const DcBoard* board() const;

private:
    DcBoard* m_board = nullptr;

    // Физический сигнал может принадлежать плате, если плата интелектуальная,
    // то в её системе параметров есть такие же параметры и их элементы имееют приоритет.
    // Элементы параметров из основного реестера должны быть синхронизированы с соответсвующими элементами параметров из плат.
    QList<ParameterElement*> m_drebezg;
    QList<ParameterElement*> m_drebezgRatio;
    QList<ParameterElement*> m_drebezgTimer;
    QList<ParameterElement*> m_drebezgNoise;
    QList<ParameterElement*> m_inversion;
};

#endif // DINPHYSICALSIGNAL_H
