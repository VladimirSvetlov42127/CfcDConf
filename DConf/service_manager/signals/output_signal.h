#ifndef OUTPUTSIGNAL_H
#define OUTPUTSIGNAL_H

#include "service_manager/signals/signal.h"
#include "service_manager/signals/target_element.h"

// OutputSignal - Дискретный выход. Может иметь только одну привязку источник - Дискретный вход!!!
// При установлении/сбросе привязок редактирует таблицу matrix_signals

class OutputSignal : public Signal, public TargetElement
{
public:
    OutputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype,const QString &name, ParameterElement* bindElement, uint16_t subtypeID);
    virtual ~OutputSignal() = default;

    QString name() const override;

protected:
    void onSourceChanged(InputSignal *newSource, InputSignal *prevSource) override;
};

#endif // OUTPUTSIGNAL_H
