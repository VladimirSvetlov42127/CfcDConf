#include "ain_signal.h"

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

AinSignal::AinSignal(const Config &config)
    : Signal{config}
{
}

bool AinSignal::init(DcController *config)
{
    m_iec101SelectFlag = config->paramsRegistry().elementBit(SP_SELECT_MASK, internalID(), 1);
    m_iec101Group1Flag = config->paramsRegistry().elementBit(SP_GROUP1LIST, internalID(), 1);
    m_iec101Group2Flag = config->paramsRegistry().elementBit(SP_GROUP2LIST, internalID(), 1);
    m_iec101BackgroundFlag = config->paramsRegistry().elementBit(SP_BACKGROUNDLIST, internalID(), 1);
    m_spodesSelectFlag = config->paramsRegistry().elementBit(SP_SPODES_MASK, internalID(), 1);

    m_apertureType = config->paramsRegistry().element(SP_AIN_APERTURE_TYPE, internalID());
    m_apertureVal = config->paramsRegistry().element(SP_AIN_APERTURE_VAL, internalID());
    m_thresholdSensibility = config->paramsRegistry().element(SP_AIN_SENSIBILITY, internalID());

    // Костыль, из за того что один и тот же параметр в разных устойствах имеет разный функционал.
    if (DcController::GSM == config->type()) {
        // У первых 4х сигналов не может быть логических порогов.
        auto elementIdx = internalID() - 4;
        if (elementIdx >= 0) {
            m_threshold0 = config->paramsRegistry().element(SP_DIN_TO_LOW, elementIdx);
            m_threshold1 = config->paramsRegistry().element(SP_DIN_TO_HIGH, elementIdx);
        }
    }
    else if (DcController::LT == config->type()) {
        // Логические пороги могут быть только у сигналов 3, 4, 5.
        // Причём соотвествующие им элементы параметров, расположены в одном параметре, друг за дргом,
        // начиная с порога логической единицы
        auto baseElementIdx = internalID() - 3;
        if (0 <= baseElementIdx && baseElementIdx < 3) {
            m_threshold1 = config->paramsRegistry().element(SP_AIN_PHYLVL1, baseElementIdx * 2);
            m_threshold0 = config->paramsRegistry().element(SP_AIN_PHYLVL1, baseElementIdx * 2 + 1);
        }
    }

    return Signal::init(config);
}

std::optional<uint8_t> AinSignal::apertureType() const
{
    return getValue<uint8_t>(m_apertureType);
}

void AinSignal::setApertureType(uint8_t value)
{
    setValue(m_apertureType, value);
}

std::optional<double> AinSignal::apertureVal() const
{
    return getValue<double>(m_apertureVal);
}

void AinSignal::setApertureVal(double value)
{
    setValue(m_apertureVal, value);
}

std::optional<double> AinSignal::thresholdSensibility() const
{
    return getValue<double>(m_thresholdSensibility);
}

void AinSignal::setThresholdSensibility(double value)
{
    setValue(m_thresholdSensibility, value);
}

std::optional<double> AinSignal::threshold0() const
{
    return getValue<double>(m_threshold0);
}

void AinSignal::setThreshold0(double value)
{
    setValue(m_threshold0, value);
}

std::optional<double> AinSignal::threshold1() const
{
    return getValue<double>(m_threshold1);
}

void AinSignal::setThreshold1(double value)
{
    setValue(m_threshold1, value);
}
