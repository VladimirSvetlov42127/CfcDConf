#ifndef CFCSERVICEINPUT_H
#define CFCSERVICEINPUT_H

#include "service_manager/services/service_input.h"

// СfcServiceInput - Входы алгоритмов гибкой логики.
// При установлении/сбросе привязок редактирует таблицу matrix_alg_сfc. В таблице alg_cfc_io, должны быть входы с соответсвующим id

class CfcAlgService;

class CfcServiceInput : public ServiceInput
{
public:
    //  id - Глобальный сквозной номер Входа-Выхода в рамках алгоритмов гибкой логики. Нужен для сохранения обратной совместимости.
    //  pin - Номер входа/выхода в рамках одного алгоритма
    CfcServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, CfcAlgService *service);
    virtual ~CfcServiceInput() = default;

    QString name() const override;
    QString text() const;

protected:
    void onSourceChanged(InputSignal *newSource, InputSignal *prevSource) override;

private:
    uint8_t _alg_id;
    CfcAlgService* m_parent = nullptr;
};

#endif // CFCSERVICEINPUT_H
