#include "din_physical_signal.h"

#include <QDebug>

#include "data_model/dc_controller.h"

namespace {

std::optional<uint8_t> getValue(ParameterElement* element)
{
    if (element)
        return element->value().toUInt();

    return {};
}

void setValue(ParameterElement* element, uint8_t value)
{
    if (element)
        element->updateValue(QString::number(value));
}

} // namespace

DinPhysicalSignal::DinPhysicalSignal(const Config &config)
    : DinSignal{config}
{
}

bool DinPhysicalSignal::init(DcController *config)
{
    m_board = nullptr;
    uint16_t signalIdxBoard = 0;
    size_t boardSignalsCount = 0;
    for (auto &board : config->boards()) {
        if (subtypeID() >= boardSignalsCount + board->DinsCount()) {
            boardSignalsCount += board->DinsCount();
            continue;
        }

        m_board = board.get();
        signalIdxBoard = subtypeID() - boardSignalsCount;
        break;
    }

    if (board() && board()->ToBoard()) {
        m_drebezg << config->paramsRegistry().element(SP_DIN_INT_CNT, signalIdxBoard);
        m_drebezgRatio << config->paramsRegistry().element(SP_DIN_DEBPARS_DOUBLE, signalIdxBoard);
        m_drebezgTimer << config->paramsRegistry().element(SP_DIN_INT_WDT, signalIdxBoard);
        m_drebezgNoise << config->paramsRegistry().element(SP_DIN_NOISE_WDT, signalIdxBoard);
        m_inversion << config->paramsRegistry().element(SP_DIN_INVERS, signalIdxBoard);
    }

    m_drebezg << config->paramsRegistry().element(SP_DIN_INT_CNT, subtypeID());
    m_drebezgRatio << config->paramsRegistry().element(SP_DIN_DEBPARS_DOUBLE, subtypeID());
    m_drebezgTimer << config->paramsRegistry().element(SP_DIN_INT_WDT, subtypeID());
    m_drebezgNoise << config->paramsRegistry().element(SP_DIN_NOISE_WDT, subtypeID());
    m_inversion << config->paramsRegistry().element(SP_DIN_INVERS, subtypeID());

    return DinSignal::init(config);
}

std::optional<uint8_t> DinPhysicalSignal::drebezg() const
{
    return getValue(m_drebezg.value(0));
}

void DinPhysicalSignal::setDrebezg(uint8_t value)
{
    for(auto element: m_drebezg)
        setValue(element, value);
}

std::optional<uint8_t> DinPhysicalSignal::drebezgRatio() const
{
    return getValue(m_drebezgRatio.value(0));
}

void DinPhysicalSignal::setDrebezgRatio(uint8_t value)
{
    for(auto element: m_drebezgRatio)
        setValue(element, value);
}

std::optional<uint8_t> DinPhysicalSignal::drebezgTimer() const
{
    return getValue(m_drebezgTimer.value(0));
}

void DinPhysicalSignal::setDrebezgTimer(uint8_t value)
{
    for(auto element: m_drebezgTimer)
        setValue(element, value);
}

std::optional<uint8_t> DinPhysicalSignal::drebezgNoise() const
{
    return getValue(m_drebezgNoise.value(0));
}

void DinPhysicalSignal::setDrebezgNoise(uint8_t value)
{
    for(auto element: m_drebezgNoise)
        setValue(element, value);
}

std::optional<bool> DinPhysicalSignal::inversionFlag() const
{
    if (auto optVal = getValue(m_inversion.value(0)))
        return optVal.value();

    return {};
}

void DinPhysicalSignal::setInversionFlag(bool flag)
{
    for(auto element: m_inversion)
        setValue(element, flag);
}

const DcBoard *DinPhysicalSignal::board() const
{
    return m_board;
}
