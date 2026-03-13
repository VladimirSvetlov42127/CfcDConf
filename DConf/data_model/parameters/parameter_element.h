#ifndef PARAMETERELEMENT_H
#define PARAMETERELEMENT_H

#include <memory>

#include <QString>
#include <QObject>

#include <dpc/sybus/smparlist.h>
#include <dpc/sybus/ParamAttribute.h>

class __Helper : public QObject
{
    Q_OBJECT
public:
signals:
    void valueChanged(const QString &value);
};

// ParameterElement - Элемент параметра (parameter) в определенной позиции (position). Имеет имя name и значение value.
// Cвойства type, addr, attributes, dataSize - такие же как у параметра в котором содержится.

class Parameter;

class ParameterElement
{
public:
    using UPtr = std::unique_ptr<ParameterElement>;
    using UPtrVector = std::vector<UPtr>;

    ParameterElement(uint16_t position, const QString &name, const QString &value, Parameter *parameter = nullptr);
//    ParameterElement(const ParameterElement &) = default;
//    ParameterElement(ParameterElement &&) = default;
//    ParameterElement& operator=(const ParameterElement &) = default;
//    ParameterElement& operator=(ParameterElement &&) = default;
    virtual ~ParameterElement() = default;

    // Идентификатор реестра параметров к которому принадлежит элемент параметра
    // Соответсвует uid() параметра или 0, если parameter == nullptr
    int32_t uid() const;

    // Позиция элемента в рамках параметра
    uint16_t position() const;

    // Имя элемента
    QString name() const;
    void setName(const QString &name);

    // Значение элемента
    QString value() const;
    void updateValue(const QString &value);

    void update(const QString &name, const QString &value);

    // Данные методы возвращают те же данные что и у параметра.
    uint8_t type() const;
    uint16_t addr() const;
    Dpc::Sybus::ParamAttribute attributes() const;
    uint16_t dataSize() const;

    // Изменения значения элемента
    bool hasChanged() const;
    void resetChangedFlag();

    // Указатель на параметр, к которому относится элемент
    const Parameter* parameter() const;
    void setParameter(Parameter *parameter);

    // методы для работы с базой данных
    virtual void insert();
    virtual void update();
    virtual void remove();

    // Создание копии данного элемента
    virtual ParameterElement::UPtr clone() const;

    // Отладка
    virtual QString toString() const;
    virtual bool isEqual(ParameterElement* other) const;

    __Helper* helper() { return &m_helper; }

protected:
    void setHasChanged(bool changed);

private:
    Parameter* m_parameter;

    uint16_t m_position;
    QString m_name;
    QString m_value;
    bool m_hasChanged;

    __Helper m_helper;
};

#endif // PARAMETERELEMENT_H
