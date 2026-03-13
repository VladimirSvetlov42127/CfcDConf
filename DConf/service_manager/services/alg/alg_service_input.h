#ifndef INTERNALSERVICEINPUT_H
#define INTERNALSERVICEINPUT_H

#include "service_manager/services/service_input.h"

// AlgServiceInput - Вход встроеного сервиса.
// При установлении/сбросе привязок редактирует таблицу matrix_alg. В таблице alg_io, должны быть входы с соответсвующим id

class AlgServiceInput : public ServiceInput
{
public:
    // id - Глобальный сквозной номер Входа-Выхода в рамках встроенных сервисов. Нужен для сохранения обратной совместимости.
    //  pin - Номер входа/выхода в рамках одного алгоритма
    AlgServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service);
    virtual ~AlgServiceInput() = default;

protected:
    void onSourceChanged(InputSignal *newSource, InputSignal *prevSource) override;
};

#endif // INTERNALSERVICEINPUT_H
