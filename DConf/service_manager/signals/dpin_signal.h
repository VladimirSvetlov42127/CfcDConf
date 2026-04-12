#ifndef DPINSIGNAL_H
#define DPINSIGNAL_H

#include <memory>
#include <optional>

#include "data_model/parameters/parameter_element.h"

class DcController;

class DPinSignal
{
public:
    enum State {
        Unknown,
        State2,
        State3,
        State4
    };

    using UPtr = std::unique_ptr<DPinSignal>;

    DPinSignal(uint16_t index);

    virtual bool init(DcController *config);

    uint16_t index() const { return m_index; }

    State state() const;
    void setState(State state);

    std::optional<uint16_t> rpo() const;
    void setRpo(uint16_t value);

    std::optional<uint16_t> rpv() const;
    void setRpv(uint16_t value);

    std::optional<uint8_t> intermediateTime() const;
    void setIntermediateTime(uint8_t value);

    std::optional<uint8_t> intermediateTimeRatio() const;
    void setIntermediateTimeRatio(uint8_t value);

    std::optional<bool> activeFlag() const;
    void setActiveFlag(bool flag);

    std::optional<bool> notGenUndefFlag() const;
    void setNotGenUndefFlag(bool flag);

    std::optional<bool> stateInversionFlag() const;
    void setStateInversionFlag(bool flag);

private:
    uint16_t m_index;

    ParameterElement *m_rpo = nullptr;
    ParameterElement *m_rpv = nullptr;
    ParameterElement *m_intermediateTime = nullptr;
    ParameterElement *m_intermediateTimeRatio = nullptr;

    ElementBit m_notGenUndefFlag;
    ElementBit m_activeFlag;
    ElementBit m_stateInversionFlag;
    ElementBit m_stateFlag1;
    ElementBit m_stateFlag2;
};

#endif // DPINSIGNAL_H
