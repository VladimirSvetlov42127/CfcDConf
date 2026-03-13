#include "target_element.h"

#include "data_model/parameters/parameter_element.h"

namespace {

const uint16_t g_NoSignal = 0xFFFF;

} // namespace

TargetElement::TargetElement(ParameterElement *bindElement, Service* service)
    : m_service{service}
    , m_bindElement{bindElement}
{
}

const Service *TargetElement::service() const
{
    return m_service;
}

void TargetElement::setSource(InputSignal *newSource)
{
    if (source() == newSource)
        return;

    auto prevSource = source();
    m_source = newSource;
    if (prevSource)
        prevSource->removeTarget(this);    

    if (newSource)
        newSource->addTarget(this);

    auto singalInternalId = newSource ? newSource->internalID() : g_NoSignal;
    m_bindElement->updateValue(QString::number(singalInternalId));

    onSourceChanged(newSource, prevSource);
}

InputSignal *TargetElement::source() const
{
    return m_source;
}

const ParameterElement *TargetElement::bindElement() const
{
    return m_bindElement;
}

ParameterElement *TargetElement::bindElement()
{
    return m_bindElement;
}

void TargetElement::onSourceChanged(InputSignal *, InputSignal *)
{
}

