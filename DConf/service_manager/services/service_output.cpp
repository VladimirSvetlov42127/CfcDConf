#include "service_output.h"

#include "data_model/parameters/parameter_element.h"

namespace {
    const uint16_t g_NoSignal = 0xFFFF;
} // namespace

ServiceOutput::ServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service)
    : ServiceIO{id, pin}
    , m_service{service}
    , m_bindElement{bindElement}
{
}

const Service *ServiceOutput::service() const
{
    return m_service;
}

QString ServiceOutput::name() const
{
    return m_bindElement->name();
}

void ServiceOutput::setTarget(VirtualInputSignal *newTarget)
{
    if (target() == newTarget)
        return;

    auto prevTarget = target();
    m_target = newTarget;
    if (prevTarget && prevTarget->source() == this)
        prevTarget->setSource(nullptr);

    if (newTarget)
        newTarget->setSource(this);

    updateBinding(newTarget);
    onTargetChanged(newTarget, prevTarget);
}

void ServiceOutput::onTargetChanged(VirtualInputSignal*, VirtualInputSignal*)
{
}

VirtualInputSignal *ServiceOutput::target() const
{
    return m_target;
}

const ParameterElement *ServiceOutput::bindElement() const
{
    return m_bindElement;
}

ParameterElement *ServiceOutput::bindElement()
{
    return m_bindElement;
}

void ServiceOutput::updateBinding(VirtualInputSignal *new_target)
{
    auto newValue = new_target ? new_target->internalID() : g_NoSignal;
    bindElement()->updateValue(QString::number(newValue));
}

