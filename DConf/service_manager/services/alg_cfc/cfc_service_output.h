#ifndef CFCSERVICEOUTPUT_H
#define CFCSERVICEOUTPUT_H

#include "service_manager/services/service_output.h"

// СfcServiceOutput - Выход алгоритмов гибкой логики.
// При установлении/сбросе привязок редактирует таблицу matrix_alg_сfc. В таблице alg_cfc_io, должны быть выходы с соответсвующим id

class CfcAlgService;

class CfcServiceOutput : public ServiceOutput
{
public:    
    //  id - Глобальный сквозной номер Входа-Выхода в рамках алгоритмов гибкой логики. Нужен для сохранения обратной совместимости.
    //  pin - Номер входа/выхода в рамках одного алгоритма
    CfcServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, CfcAlgService *service);
    virtual ~CfcServiceOutput() = default;

    QString name() const override;
    QString text() const;

protected:
    void onTargetChanged(VirtualInputSignal *newTarget, VirtualInputSignal *prevTarget) override;

private:
    uint8_t _alg_id;
    CfcAlgService *m_parent = nullptr;
};

#endif // CFCSERVICEOUTPUT_H
