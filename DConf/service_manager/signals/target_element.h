#ifndef __TARGET_ELEMENT_H__
#define __TARGET_ELEMENT_H__

#include <QString>

#include "service_manager/signals/din_signal.h"

// TargetElement - Базовый элемент привязки типа "Цель" - элемент привязки которой может участвовать в привязках, только как цель - Вход сервиса!!! или Дискретный выход!!!
// У него может быть задан только один "Источник (сигнал)" - Дискретный вход.
// При установлении/сбросе привязок выставляет параметру bindElement - корректное значение

class ParameterElement;
class Service;

class TargetElement
{
public:
    // bindElement - физический параметр с значением привязки
    // Если TargetElement - это вход сервиса, то service должен указывать на сервис, в противном случае nullptr.
    TargetElement();
    TargetElement(ParameterElement* bindElement, Service* service);
    virtual ~TargetElement() = default;

    virtual QString name() const = 0;

    // Если TargetElement - это вход сервиса, то возвращает сервис которому принадлежит этот вход, в противном случае nullptr.
    const Service* service() const;

    void setSource(DinSignal* newSource);
    DinSignal* source() const;

    const ParameterElement* bindElement() const;

protected:
    ParameterElement* bindElement();
    void setBindElement(ParameterElement* bindElement);

    virtual void onSourceChanged(DinSignal *newSource, DinSignal *prevSource);

private:
    Service *m_service = nullptr;
    ParameterElement* m_bindElement = nullptr;
    DinSignal* m_source = nullptr;
};

#endif // __TARGET_ELEMENT_H__
