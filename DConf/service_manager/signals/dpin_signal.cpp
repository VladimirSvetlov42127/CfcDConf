#include "dpin_signal.h"

#include "data_model/dc_controller.h"

namespace {

enum DpAlgBit {
    BitActive = 0,
    BitState1,
    BitInversion,
    BitState2,
    BitNotGenerateUntrue
};

template<typename T>
std::optional<T> getValue(ParameterElement* element)
{
    if (element)
        return element->value().toUInt();

    return {};
}

template<typename T>
void setValue(ParameterElement* element, const T& value)
{
    if (element)
        element->updateValue(QString::number(value));
}

} // namespace

DPinSignal::DPinSignal(uint16_t index)
    : m_index{index}
{
}

bool DPinSignal::init(DcController *config)
{
    m_rpo = config->paramsRegistry().element(SP_DIN_DP_COUPLES, index(), 0);
    m_rpv = config->paramsRegistry().element(SP_DIN_DP_COUPLES, index(), 1);
    m_intermediateTime = config->paramsRegistry().element(SP_DIN_DP_INTERMID_TIME, index());
    m_intermediateTimeRatio = config->paramsRegistry().element(SP_DIN_DP_INTERMID_TIME_COEF, index());

    m_activeFlag = config->paramsRegistry().elementBit(SP_DIN_DP_ALG, BitActive, 0, index());
    m_stateFlag1 = config->paramsRegistry().elementBit(SP_DIN_DP_ALG, BitState1, 0, index());
    m_stateFlag2 = config->paramsRegistry().elementBit(SP_DIN_DP_ALG, BitState2, 0, index());
    m_notGenUndefFlag = config->paramsRegistry().elementBit(SP_DIN_DP_ALG, BitNotGenerateUntrue, 0, index());
    m_stateInversionFlag = config->paramsRegistry().elementBit(SP_DIN_DP_ALG, BitInversion, 0, index());
    return true;
}

DPinSignal::State DPinSignal::state() const
{
    auto st = State::Unknown;
    if (m_stateFlag1.element && m_stateFlag2.element) {
        auto bit1Value = m_stateFlag1.value().value();
        auto bit2Value = m_stateFlag2.value().value();
        if (bit1Value && !bit2Value)
            st = State2;
        else if (!bit1Value && bit2Value)
            st = State3;
        else if (!bit1Value && !bit2Value)
            st = State4;
    }

    return st;
}

void DPinSignal::setState(State state)
{
    auto bit1Value = true;
    auto bit2Value = true;
    switch (state) {
    case State2:
        bit2Value = false;
        break;
    case State3:
        bit1Value = false;
        break;
    case State4:
        bit1Value = false;
        bit2Value = false;
        break;
    default:
        break;
    }

    m_stateFlag1.setValue(bit1Value);
    m_stateFlag2.setValue(bit2Value);
}

std::optional<uint16_t> DPinSignal::rpo() const
{
    return getValue<uint16_t>(m_rpo);
}

void DPinSignal::setRpo(uint16_t value)
{
    setValue(m_rpo, value);
}

std::optional<uint16_t> DPinSignal::rpv() const
{
    return getValue<uint16_t>(m_rpv);
}

void DPinSignal::setRpv(uint16_t value)
{
    setValue(m_rpv, value);
}

std::optional<uint8_t> DPinSignal::intermediateTime() const
{
    return getValue<uint8_t>(m_intermediateTime);
}

void DPinSignal::setIntermediateTime(uint8_t value)
{
    setValue(m_intermediateTime, value);
}

std::optional<uint8_t> DPinSignal::intermediateTimeRatio() const
{
    return getValue<uint8_t>(m_intermediateTimeRatio);
}

void DPinSignal::setIntermediateTimeRatio(uint8_t value)
{
    setValue(m_intermediateTimeRatio, value);
}

std::optional<bool> DPinSignal::activeFlag() const
{
    return m_activeFlag.value();
}

void DPinSignal::setActiveFlag(bool flag)
{
    m_activeFlag.setValue(flag);
}

std::optional<bool> DPinSignal::notGenUndefFlag() const
{
    return m_notGenUndefFlag.value();
}

void DPinSignal::setNotGenUndefFlag(bool flag)
{
    m_notGenUndefFlag.setValue(flag);
}

std::optional<bool> DPinSignal::stateInversionFlag() const
{
    return m_stateInversionFlag.value();
}

void DPinSignal::setStateInversionFlag(bool flag)
{
    m_stateInversionFlag.setValue(flag);
}
