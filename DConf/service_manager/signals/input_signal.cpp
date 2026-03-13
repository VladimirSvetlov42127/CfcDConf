#include "input_signal.h"

#include "service_manager/signals/target_element.h"

InputSignal::InputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID)
    : Signal{id, internalid, direction, type, subtype, name, subtypeID}
{
}

const QList<TargetElement *> &InputSignal::targets() const
{
    return m_targets;
}

void InputSignal::addTarget(TargetElement *target)
{
    auto idx = m_targets.indexOf(target);
    if (idx > -1)
        return;

    m_targets.append(target);
    if (target->source() != this)
        target->setSource(this);
}

void InputSignal::removeTarget(TargetElement *target)
{
    auto idx = m_targets.indexOf(target);
    if (idx < 0)
        return;

    auto temp = m_targets[idx];
    m_targets.removeAt(idx);
    if (temp->source() == this)
        temp->setSource(nullptr);
}
