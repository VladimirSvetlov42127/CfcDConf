#include "counter_model.h"

CounterModel::CounterModel(DcController *controller, QObject *parent)
    : QAbstractTableModel(parent)
    , m_controller(controller)
{
    for (const auto& signal : controller->getSignalList(DEF_SIG_TYPE_COUNTER)) {
        m_items.append(signal);
    }
}

CounterModel::~CounterModel()
{

}

int CounterModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int CounterModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid())
        return 0;

    return ColumnsCount;
}

QVariant CounterModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role) {
        switch(section) {
        case NumberColumn:
            return "№";
            break;
        case NameColumn:
            return "Название";
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags CounterModel::flags(const QModelIndex &index) const {
    Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if (index.column() < 1)
        return flag;

    if (index.data().isValid() && index.column() != NameColumn)
        flag |= Qt::ItemIsEditable;

    auto subType = m_items.at(index.row())->subtype();
    if (index.column() == NameColumn && (subType == DEF_SIG_SUBTYPE_VIRTUAL || subType == DEF_SIG_SUBTYPE_REMOTE))
        return flag | Qt::ItemIsEditable;

    return flag;
}

QVariant CounterModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid())
        return QVariant();

    auto signal = m_items[index.row()];
    int col = index.column();

    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;

    if (Qt::EditRole == role || Qt::DisplayRole == role) {
        switch(col) {
        case NumberColumn:
            return signal->internalId();
            break;
        case NameColumn:
            return signal->name();
            break;
        }
    }

    return QVariant();
}

bool CounterModel::setData(const QModelIndex &index, const QVariant &value, int role) {

    auto signal = m_items.at(index.row());
    int col = index.column();

    if (!index.isValid())
        return false;

    if (Qt::EditRole == role) {
        if (col == NameColumn)
            signal->updateName(value.toString());

        emit dataChanged(index, index);
        return true;
    }

    return false;
}
