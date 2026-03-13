#include "parameter_element.h"

#include "db/dc_db_manager.h"
#include "data_model/parameters/parameter.h"

ParameterElement::ParameterElement(uint16_t position, const QString &name, const QString &value, Parameter *parameter)
    : m_parameter{parameter}
    , m_position{position}
    , m_name{name}
    , m_value{value}
    , m_hasChanged{false}
{
}

int32_t ParameterElement::uid() const
{
    return parameter() ? parameter()->uid() : 0;
}

uint16_t ParameterElement::position() const
{
    return m_position;
}

QString ParameterElement::name() const
{
    return m_name;
}

void ParameterElement::setName(const QString &name)
{
    if (name == m_name)
        return;

    m_name = name;
    setHasChanged(true);
    update();
}

QString ParameterElement::value() const
{
    return m_value;
}

void ParameterElement::updateValue(const QString &value)
{
    if (value == m_value)
        return;

    m_value = value;
    setHasChanged(true);
    emit m_helper.valueChanged(m_value);
    update();
}

void ParameterElement::update(const QString &name, const QString &value)
{
    if (name == m_name && value == m_value)
        return;

    m_name = name;
    m_value = value;
    setHasChanged(true);
    emit m_helper.valueChanged(m_value);
    update();
}

uint8_t ParameterElement::type() const
{
    return parameter() ? parameter()->type() : 0;
}

uint16_t ParameterElement::addr() const
{
    return parameter() ? parameter()->addr() : 0;
}

Dpc::Sybus::ParamAttribute ParameterElement::attributes() const
{
    return parameter() ? parameter()->attributes() : Dpc::Sybus::ParamAttribute{};
}

uint16_t ParameterElement::dataSize() const
{
    return parameter() ? parameter()->dataSize() : 0;
}

bool ParameterElement::hasChanged() const
{
    return m_hasChanged;
}

void ParameterElement::resetChangedFlag()
{
    m_hasChanged = false;
}

const Parameter *ParameterElement::parameter() const
{
    return m_parameter;
}

void ParameterElement::setParameter(Parameter *parameter)
{
    m_parameter = parameter;
}

void ParameterElement::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO cfg_parameters_items(name, address, param_index, value) VALUES('%1', %2, %3, '%4');";
    QString insertStr = insertStrTmp.arg(name(), QString::number(addr()), QString::number(position()), value());
    gDbManager.execute(uid(), insertStr);
}

void ParameterElement::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE cfg_parameters_items SET name = '%1', value = '%2' WHERE address = %3 and param_index = %4;";
    QString updateStr = updateStrTmp.arg(name(), value(), QString::number(addr()), QString::number(position()));
    gDbManager.execute(uid(), updateStr);
}

void ParameterElement::remove()
{
    if (!uid())
        return;

    QString deleteStrTmp = "DELETE FROM cfg_parameters_items WHERE address = %1 and param_index = %2;";
    QString deleteStr = deleteStrTmp.arg(addr()).arg(position());
    gDbManager.execute(uid(), deleteStr);
}

ParameterElement::UPtr ParameterElement::clone() const
{
    return std::make_unique<ParameterElement>( position(), name(), value() );
}

QString ParameterElement::toString() const
{
    return QString("0x%1[%2] Name(%3), Value(%4)").arg(QString::number(addr(), 16)).arg(position()).arg(name(), value());
}

bool ParameterElement::isEqual(ParameterElement *other) const
{
    return position() == other->position() && name() == other->name();
}

void ParameterElement::setHasChanged(bool changed)
{
    m_hasChanged = changed;
}
