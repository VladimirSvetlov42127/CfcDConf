#ifndef INTERNALSERVICEOUTPUT_H
#define INTERNALSERVICEOUTPUT_H

#include "service_manager/services/service_output.h"

// AlgServiceOutput - Выход встроеного сервиса.
// При установлении/сбросе привязок редактирует таблицу matrix_alg. В таблице alg_io, должны быть выходы с соответсвующим id

class AlgServiceOutput : public ServiceOutput
{
public:
    // id - Глобальный сквозной номер Входа-Выхода в рамках встроенных сервисов. Нужен для сохранения обратной совместимости.
    //  pin - Номер входа/выхода в рамках одного алгоритма
    AlgServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service);
    virtual ~AlgServiceOutput() = default;

protected:
    void onTargetChanged(VirtualInputSignal *newTarget, VirtualInputSignal *prevTarget) override;
};

#endif // INTERNALSERVICEOUTPUT_H
