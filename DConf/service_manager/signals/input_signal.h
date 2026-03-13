#ifndef INPUTSIGNAL_H
#define INPUTSIGNAL_H

#include "service_manager/signals/signal.h"

// InputSignal - Дискретный вход. Может быть привязан ко многим элементам цель - Дискретным выходам!!! и Входам сервисов!!!.

class TargetElement;

class InputSignal : public Signal
{
public:
    InputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype,const QString &name, uint16_t subtypeID);
    virtual ~InputSignal() = default;

    const QList<TargetElement*>& targets() const;
    void addTarget(TargetElement* target);
    void removeTarget(TargetElement* target);

private:
    QList<TargetElement*> m_targets;
};

#endif // INPUTSIGNAL_H
