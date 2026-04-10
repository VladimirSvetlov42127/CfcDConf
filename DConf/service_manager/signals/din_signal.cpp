#include "din_signal.h"

#include "service_manager/signals/target_element.h"
#include "data_model/dc_controller.h"

DinSignal::DinSignal(const Config &config)
    : Signal{config}
{
}

const QList<TargetElement *> &DinSignal::targets() const
{
    return m_targets;
}

void DinSignal::addTarget(TargetElement *target)
{
    auto idx = m_targets.indexOf(target);
    if (idx > -1)
        return;

    m_targets.append(target);
    if (target->source() != this)
        target->setSource(this);
}

void DinSignal::removeTarget(TargetElement *target)
{
    auto idx = m_targets.indexOf(target);
    if (idx < 0)
        return;

    auto temp = m_targets[idx];
    m_targets.removeAt(idx);
    if (temp->source() == this)
        temp->setSource(nullptr);
}

bool DinSignal::init(DcController *config)
{
    m_oscillBit = config->paramsRegistry().elementBit(SP_DIN_DINOSCMASK, internalID());
    m_trendBit = config->paramsRegistry().elementBit(SP_TREND_INITIALISE, internalID());
    m_archiveBit = config->paramsRegistry().elementBit(SP_DIN_CASH_REQMASK, internalID());
    m_dpBit = config->paramsRegistry().elementBit(SP_DIN_DPSREPRESENT, internalID());

    m_iec101SelectFlag = config->paramsRegistry().elementBit(SP_SELECT_MASK, internalID(), 0);
    m_iec101Group1Flag = config->paramsRegistry().elementBit(SP_GROUP1LIST, internalID(), 0);
    m_iec101Group2Flag = config->paramsRegistry().elementBit(SP_GROUP2LIST, internalID(), 0);
    m_iec101BackgroundFlag = config->paramsRegistry().elementBit(SP_BACKGROUNDLIST, internalID(), 0);
    m_iec103SelectFlag = config->paramsRegistry().elementBit(SP_IEC_103_MASK_DINS, internalID());
    m_spodesSelectFlag = config->paramsRegistry().elementBit(SP_SPODES_MASK, internalID(), 0);

    return Signal::init(config);
}
