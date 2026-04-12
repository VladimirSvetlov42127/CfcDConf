#include "din_acp_signal.h"

#include <QDebug>

#include "data_model/dc_controller.h"

namespace {

template<typename T>
std::optional<T> getValue(ParameterElement* element)
{
    if (element) {
        if constexpr (std::is_same_v<T, uint8_t>)
                return element->value().toUInt();
        else if constexpr (std::is_same_v<T, double>)
                return element->value().toDouble();
    }

    return {};
}

template<typename T>
void setValue(ParameterElement* element, const T& value)
{
    if (element) {
        if constexpr (std::is_same_v<T, uint8_t>)
                element->updateValue(QString::number(value));
        else if constexpr (std::is_same_v<T, double>)
                element->updateValue(QString::number(value));
    }
}

} // namespace

DinAcpSignal::DinAcpSignal(const Config &config)
    : DinSignal{config}
{
}

bool DinAcpSignal::init(DcController *config)
{
    m_drebezg = config->paramsRegistry().element(SP_DIN_TABLEPARS, internalID(), 0);
    m_drebezgTimer = config->paramsRegistry().element(SP_DIN_TABLEPARS, internalID(), 1);
    m_drebezgNoise = config->paramsRegistry().element(SP_DIN_TABLEPARS, internalID(), 2);
    m_inversion = config->paramsRegistry().element(SP_DIN_INVERS, internalID());

    m_thresholdChain = config->paramsRegistry().element(SP_DIN_BREAK, internalID(), 0);
    m_threshold0 = config->paramsRegistry().element(SP_DIN_BREAK, internalID(), 1);
    m_threshold1 = config->paramsRegistry().element(SP_DIN_BREAK, internalID(), 2);
    m_thresholdKZ = config->paramsRegistry().element(SP_DIN_BREAK, internalID(), 3);

    return DinSignal::init(config);
}

std::optional<uint8_t> DinAcpSignal::drebezg() const
{
    return getValue<uint8_t>(m_drebezg);
}

void DinAcpSignal::setDrebezg(uint8_t value)
{
    setValue(m_drebezg, value);
}

std::optional<uint8_t> DinAcpSignal::drebezgTimer() const
{
    return getValue<uint8_t>(m_drebezgTimer);
}

void DinAcpSignal::setDrebezgTimer(uint8_t value)
{
    setValue(m_drebezgTimer, value);
}

std::optional<uint8_t> DinAcpSignal::drebezgNoise() const
{
    return getValue<uint8_t>(m_drebezgNoise);
}

void DinAcpSignal::setDrebezgNoise(uint8_t value)
{
    setValue(m_drebezgNoise, value);
}

std::optional<bool> DinAcpSignal::inversionFlag() const
{
    if (auto optVal = getValue<uint8_t>(m_inversion))
        return optVal.value();

    return {};
}

void DinAcpSignal::setInversionFlag(bool flag)
{
    setValue(m_inversion, static_cast<uint8_t>(flag));
}

std::optional<double> DinAcpSignal::thresholdChain() const
{
    return getValue<double>(m_thresholdChain);
}

void DinAcpSignal::setThresholdChain(double value)
{
    setValue(m_thresholdChain, value);
}

std::optional<double> DinAcpSignal::threshold0() const
{
    return getValue<double>(m_threshold0);
}

void DinAcpSignal::setThreshold0(double value)
{
    setValue(m_threshold0, value);
}

std::optional<double> DinAcpSignal::threshold1() const
{
    return getValue<double>(m_threshold1);
}

void DinAcpSignal::setThreshold1(double value)
{
    setValue(m_threshold1, value);
}

std::optional<double> DinAcpSignal::thresholdKZ() const
{
    return getValue<double>(m_thresholdKZ);
}

void DinAcpSignal::setThresholdKZ(double value)
{
    setValue(m_thresholdKZ, value);
}
