#ifndef __SERVICE_OUTPUT_H__
#define __SERVICE_OUTPUT_H__


#include <QString>

#include "service_manager/services/service_io.h"
#include "service_manager/signals/din_virtual_signal.h"

// ServiceOutput - Выход сервиса.

class ParameterElement;
class Service;

class ServiceOutput : public ServiceIO
{
public:
    ServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service);
    virtual ~ServiceOutput() = default;

    // Сервис которому принадлежит выход
    const Service* service() const;

    virtual QString name() const;

    void setTarget(DinVirtualSignal* newTarget);
    DinVirtualSignal* target() const;

    const ParameterElement* bindElement() const;

protected:
    ParameterElement* bindElement();
    virtual void updateBinding(DinVirtualSignal* new_target);
    virtual void onTargetChanged(DinVirtualSignal* newTarget, DinVirtualSignal* prevTarget);

private:
    Service* m_service = nullptr;
    ParameterElement* m_bindElement = nullptr;
    DinVirtualSignal* m_target = nullptr;
};

#endif // __SERVICE_OUTPUT_H__
