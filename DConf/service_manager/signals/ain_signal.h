#ifndef __AIN_SIGNAL_H__
#define __AIN_SIGNAL_H__


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QString>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "data_model/dc_controller.h"
#include "service_manager/signals/signal.h"


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
//	Базовый класс аналогов
//  Базовые параметры:
//  - Тип апертуры (_aperture_type)
//  - Значение апертура (_aperture_value)
//  - Порог чувствительности (_aperture_sens)
//  - Порог логической единицы (_aperture_high) - выводится в QString т.к. доступно не для всех сигналов
//  - Порог логического нуля (_aperture_low) - выводится в QString т.к. доступно не для всех сигналов
//===================================================================================================================================================
class AinSignal : public Signal
{
public:
    //===============================================================================================================================================
    //	Конструктор класса
    //===============================================================================================================================================
    AinSignal(DcController* controller, int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID = 0);

    //===============================================================================================================================================
    //	Открытые методы класса
    //===============================================================================================================================================
    uint8_t apertureType() const { return _aperture_type->value().toInt(); }
    float apertureValue() const { return _aperture_value->value().toFloat(); }
    float apertureSens() const { return _aperture_sens->value().toFloat(); }
    QString apertureHigh() const { return _aperture_high ? _aperture_high->value() : QString(); }
    QString apertureLow() const { return _aperture_low ? _aperture_low->value() : QString(); }

    void setApertureType(uint8_t value) { _aperture_type->updateValue(QString::number(value)); }
    void setApertureValue(float value) { _aperture_value->updateValue(QString::number(value)); }
    void setSpertureSens(float value) { _aperture_sens->updateValue(QString::number(value)); }
    void setApertureHigh(const QString value) { if (_aperture_high) _aperture_high->updateValue(value); }
    void setApertureLow(const QString value) { if (_aperture_low) _aperture_low->updateValue(value); }

private:
    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    ParameterElement* _aperture_type;
    ParameterElement* _aperture_value;
    ParameterElement* _aperture_sens;
    ParameterElement* _aperture_high;
    ParameterElement* _aperture_low;
};

#endif // __AIN_SIGNAL_H__
