#include "service_input.h"

#include "data_model/parameters/parameter_element.h"

ServiceInput::ServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service)
    : ServiceIO{id, pin}
    , TargetElement{bindElement, service}
{
}

QString ServiceInput::name() const
{
    return bindElement()->name();
}
