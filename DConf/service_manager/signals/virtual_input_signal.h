#ifndef VIRTUALINPUTSIGNAL_H
#define VIRTUALINPUTSIGNAL_H

#include "service_manager/signals/input_signal.h"

// VirtualInputSignal - Виртуальный дискретный вход, расширение InputSignal(Дискретного входа), который может иметь привязку источник - Выход сервиса!!!

class ServiceOutput;

class VirtualInputSignal : public InputSignal
{
public:
    VirtualInputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype,const QString &name, uint16_t subtypeID);
    virtual ~VirtualInputSignal() = default;

    QString name() const override;
    QString text() const override;
    QString fullText() const override;

    QString baseText() const;

    void setSource(ServiceOutput* newSource);
    ServiceOutput* source() const;

private:
    ServiceOutput* m_source = nullptr;
};

#endif // VIRTUALINPUTSIGNAL_H
