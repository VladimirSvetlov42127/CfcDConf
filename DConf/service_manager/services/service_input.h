#ifndef SERVICEINPUT_H
#define SERVICEINPUT_H

#include <QString>
#include <QVariant>

#include "service_manager/services/service_io.h"
#include "service_manager/signals/target_element.h"

// ServiceInput - Вход сервиса.

class ServiceInput : public ServiceIO, public TargetElement
{
public:
    ServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service);
    virtual ~ServiceInput() = default;

    QString name() const override;
};

#endif // SERVICEINPUT_H
