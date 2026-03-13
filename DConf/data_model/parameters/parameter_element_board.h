#ifndef PARAMETERELEMENTBOARD_H
#define PARAMETERELEMENTBOARD_H

#include "data_model/parameters/parameter_element.h"
#include "data_model/dc_properties.h"

// ParameterElementBoard - Элемент параметра(Parameter) под определенным индексом, который находится на отдельной плате.
// Отдельный класс, так как данные этого элемента имеют другую структуру хранения в базе данных, хотя являются теми же элементами системы параметров.

class ParameterElementBoard : public ParameterElement
{
public:
    ParameterElementBoard(int32_t board, uint16_t position, const QString &name, const QString &value, const QString &properties, Parameter *parameter = nullptr);

    // Индекс платы к которой относится элемент
    int32_t board() const;

    // Работа со свойствами элемента.
    QString properties() const;
    QString property(const QString &name) const;
    bool updateProperty(const QString &name, const QString &value);

    // методы для работы с базой данных
    virtual void insert() override;
    virtual void update() override;
    virtual void remove() override;

    // Создание клона данного элемента
    virtual ParameterElement::UPtr clone() const override;

    // Отладка
    QString toString() const override;
    bool isEqual(ParameterElement* other) const override;

private:
    int32_t m_board;
    DcProperties m_properties;
};

#endif // PARAMETERELEMENTBOARD_H
