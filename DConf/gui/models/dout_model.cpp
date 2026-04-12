#include "dout_model.h"

#include "service_manager/signals/signal_icustom_name.h"

DoutModel::DoutModel(const std::vector<DoutSignal*>& items, QObject* parent)
    : QAbstractTableModel{parent}
    , m_items{items}
{
}

int DoutModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int DoutModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Column::__Count;
}

QVariant DoutModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case Column::Number:  return "№";
    case Column::Name:    return "Название";
    case Column::Impulse: return "Длительность импульсов управления";
    }

    return QVariant();
}

Qt::ItemFlags DoutModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags flag = QAbstractTableModel::flags(index);
    if (index.column()) {
        auto item = items().at(index.row());
        if (index.column() == Column::Name) {
            if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote))
                flag |= Qt::ItemIsEditable;
        }

        if (index.column() > 1 && index.data().isValid()) {
            flag |= Qt::ItemIsEditable;
        }

        QVariant value = index.data(Qt::CheckStateRole);
        if (value == Qt::Checked || value == Qt::Unchecked) {
            flag |= Qt::ItemIsUserCheckable;
        }
    }

    return flag;
}

QVariant DoutModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto item = items().at(index.row());
    int column = index.column();

    if (column == Column::Name && role == Qt::TextAlignmentRole) {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
    }

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    if (role == Qt::EditRole && column == DoutModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote))
            return item->to<ISignalCustomName>()->customName();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case Column::Number: return item->internalID();
        case Column::Name: return item->name();
        case Column::Impulse: return toVariant(item->impulse());
        default: break;
        }
    }

    if (role == Qt::CheckStateRole) {

    }

    return QVariant();
}

bool DoutModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    auto item = items().at(index.row());
    auto column = index.column();    
    bool changed = false;

    if (role == Qt::EditRole && column == DoutModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote)) {
            item->to<ISignalCustomName>()->setCustomName(value.toString().trimmed());
            changed = true;
        }
    }

    if (column == Column::Impulse) {
        item->setImpulse(value.toUInt());
        changed = true;
    }

    if (changed) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
