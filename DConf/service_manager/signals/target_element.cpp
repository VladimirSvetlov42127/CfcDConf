#include "target_element.h"

#include "data_model/parameters/parameter_element.h"

namespace {

const uint16_t g_NoSignal = 0xFFFF;

} // namespace

TargetElement::TargetElement()
{

}

TargetElement::TargetElement(ParameterElement *bindElement, Service* service)
    : m_service{service}
    , m_bindElement{bindElement}
{
}

const Service *TargetElement::service() const
{
    return m_service;
}

void TargetElement::setSource(DinSignal *newSource)
{
    if (source() == newSource)
        return;

    auto prevSource = source();
    m_source = newSource;
    if (prevSource)
        prevSource->removeTarget(this);    

    if (newSource)
        newSource->addTarget(this);

    if (bindElement()) {
        auto singalInternalId = newSource ? newSource->internalID() : g_NoSignal;
        bindElement()->updateValue(QString::number(singalInternalId));
    }

    onSourceChanged(newSource, prevSource);
}

DinSignal *TargetElement::source() const
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

void TargetElement::setBindElement(ParameterElement *bindElement)
{
    m_bindElement = bindElement;
}

void TargetElement::onSourceChanged(DinSignal *, DinSignal *)
{
}

