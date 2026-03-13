#include "virtual_functions_model.h"

#include "service_manager/services/func/func_service.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
VirtualFunctionsModel::VirtualFunctionsModel(FuncService *service, QObject* parent)
    : QAbstractTableModel(parent)
    , _service{service}
{
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
int VirtualFunctionsModel::rowCount(const QModelIndex& parent) const
{
    return service()->funcList().size();
}

int VirtualFunctionsModel::columnCount(const QModelIndex& parent) const
{
    return TOTAL_COLUMNS;
}

QVariant VirtualFunctionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        if (section == NUMBER_COLUMN)
            return "№";
        if (section == OUTPUT_COLUMN)
            return "Выход";
        if (section == FUNCTION_COLUMN)
            return "Функция";
        if (section == PARAMETER_COLUMN)
            return "Параметер";
    }

    return QVariant();
}

Qt::ItemFlags VirtualFunctionsModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemFlag();
    int column = index.column();
    int row = index.row();

    Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if (column < FUNCTION_COLUMN)
        return flag;

    if (column == FUNCTION_COLUMN)
        return flag |= Qt::ItemIsEditable;

    //  Флаги для вывода параметров
    if (column == PARAMETER_COLUMN) {
        int type = service()->funcList().at(row)->type();
        if (type == VFunc::NOTUSE || type ==VFunc::TEST_CNTRL || type ==VFunc::OSCILL_START || type ==VFunc::QUIT_CMD || type ==VFunc::EXEC_EMBEDED_ALG ||
            type ==VFunc::NETWUSE || type ==VFunc::CHANGE_SIM || type ==VFunc::BLOCK_TU)
            return flag;
    }

    return flag |= Qt::ItemIsEditable;
}

QVariant VirtualFunctionsModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    int column = index.column();
    int row = index.row();

    //  Выравнивание
    if (role == Qt::TextAlignmentRole)
        return column == NUMBER_COLUMN ? Qt::AlignCenter : QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    //  Вывод данных
    if (role == Qt::EditRole) {
        if (column == FUNCTION_COLUMN)
            return service()->funcList().at(row)->typeId();
        if (column == PARAMETER_COLUMN)
            return service()->funcList().at(row)->argValue();
    }

    if (role == Qt::DisplayRole) {
        if (column == NUMBER_COLUMN)
            return row + 1;
        if (column == OUTPUT_COLUMN)
            return QString("Виртуальный выход %1").arg(row + 1);
        if (column == FUNCTION_COLUMN)
            return service()->funcList().at(row)->typeId();
        if (column == PARAMETER_COLUMN) {
            int type = service()->funcList().at(row)->type();
            if (type == VFunc::NOTUSE || type ==VFunc::TEST_CNTRL || type ==VFunc::OSCILL_START || type ==VFunc::QUIT_CMD || type ==VFunc::EXEC_EMBEDED_ALG ||
                type ==VFunc::NETWUSE || type ==VFunc::CHANGE_SIM || type ==VFunc::BLOCK_TU)
                return QVariant();
            else return service()->funcList().at(row)->argValue();
        }
    }

    if (role == Qt::UserRole && column == PARAMETER_COLUMN)
        return service()->funcList().at(row)->type();

    return QVariant();
}

bool VirtualFunctionsModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;
    int column = index.column();
    int row = index.row();

    if (column == FUNCTION_COLUMN && role == Qt::EditRole) {
        service()->setType(row, value.toInt());
        service()->setArgValue(row, service()->funcList().at(row)->argValue());
        emit dataChanged(index, this->index(row, Columns::PARAMETER_COLUMN));
    }

    if (column == PARAMETER_COLUMN) {
        if (role == Qt::EditRole)
            service()->setArgValue(row, value.toInt());
    }

    return true;
}
