#include "parameter_element_board.h"

#include <QDebug>

#include "db/dc_db_manager.h"
#include "data_model/parameters/parameter.h"

ParameterElementBoard::ParameterElementBoard(int32_t board, uint16_t position, const QString &name, const QString &value, const QString &properties, Parameter* parameter)
    : ParameterElement{position, name, value, parameter}
    , m_board{board}
{
    m_properties.fromJson(properties);
}

int32_t ParameterElementBoard::board() const
{
    return m_board;
}

QString ParameterElementBoard::properties() const
{
    return m_properties.toJson();
}

QString ParameterElementBoard::property(const QString &name) const
{
    return m_properties.get(name);
}

bool ParameterElementBoard::updateProperty(const QString &name, const QString &value)
{
    if (name.isEmpty())
        return false;
    if (value.isEmpty())
        return false;

    m_properties.set(name, value);
    setHasChanged(true);
    update();
    return true;
}

void ParameterElementBoard::insert()
{
    if (!uid())
        return;

    QString insertStrTmp = "INSERT INTO board_cfg_parameters(board_id, addr, param, name, data_type, \
    flags, value, properties) VALUES(%1, %2, %3, '%4', %5, '%6', '%7', '%8');";

    QString insertStr = insertStrTmp.arg(QString::number(board()), QString::number(addr()), QString::number(position()),
        name(), QString::number(type()), attributes().toString(), value(), properties());
    gDbManager.execute(uid(), insertStr);
}

void ParameterElementBoard::update()
{
    if (!uid())
        return;

    QString updateStrTmp = "UPDATE board_cfg_parameters SET name = '%1', properties = '%2', value = '%3' \
    WHERE board_id = %4 and addr = %5 and param = %6;";

    QString updateStr = updateStrTmp.arg(name(), properties(), value(), QString::number(board()), QString::number(addr()), QString::number(position()));
    gDbManager.execute(uid(), updateStr);
}

void ParameterElementBoard::remove()
{
    if (!uid())
        return;

    QString removeStrTmp = "DELETE FROM board_cfg_parameters WHERE board_id = %1 and addr = %2 and param = %3;";
    QString removeStr = removeStrTmp.arg(board()).arg(addr()).arg(position());
    gDbManager.execute(uid(), removeStr);
}

ParameterElement::UPtr ParameterElementBoard::clone() const
{
    return std::make_unique<ParameterElementBoard>(board(), position(), name(), value(), properties());
}

QString ParameterElementBoard::toString() const
{
    return QString("%1, Board(%2), Properties(%3)").arg(ParameterElement::toString()).arg(board()).arg(properties());
}

bool ParameterElementBoard::isEqual(ParameterElement *other) const
{
    if (!ParameterElement::isEqual(other))
        return false;

    auto cast = dynamic_cast<ParameterElementBoard*>(other);
    if (!cast)
        return false;

    return board() == cast->board() && properties() == cast->properties();
}
