#include "dpin_model.h"

DPinModel::DPinModel(const std::vector<DPinSignal*>& items, QObject* parent)
    : QAbstractTableModel{parent}
    , m_items{items}
{
}

int DPinModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int DPinModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Column::__Count;
}

QVariant DPinModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case Column::Number:                return "№";
    case Column::RPO:                   return "Номер канала,\n замкнутогов состоянии\n ОТКЛ (РПО)";
    case Column::RPV:                   return "Номер канала,\n замкнутого в состоянии\n ВКЛ (РПВ)";
    case Column::IntermediateTime:      return "Время в\n промежуточном\nсостоянии, мс";
    case Column::NotGenUndef:           return "Не формировать\n недостоверность при\n превышении времени";
    case Column::IntermediateTimeRatio: return "Масштабный коэффициент\n времени в\n промежуточном состоянии";
    case Column::Active:                return "Активен";
    case Column::State:                 return "Состояния";
    case Column::Inversion:             return "Инверсия состояния";
    default:
        break;
    }

    return QVariant();
}

Qt::ItemFlags DPinModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags fl = QAbstractTableModel::flags(index);
    if (index.column() < 1)
        return fl;

    if (index.data().isValid())
        fl |= Qt::ItemIsEditable;

    QVariant val = index.data(Qt::CheckStateRole);
    if (val == Qt::Checked || val == Qt::Unchecked)
        fl |= Qt::ItemIsUserCheckable;

    return fl;
}

QVariant DPinModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto item = items().at(index.row());
    int column = index.column();

    if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter;
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case Column::Number: return item->index();
        case Column::RPO: return toVariant(item->rpo());
        case Column::RPV: return toVariant(item->rpv());
        case Column::IntermediateTime: return toVariant(item->intermediateTime());
        case Column::IntermediateTimeRatio: return toVariant(item->intermediateTimeRatio());
        case Column::State: return item->state();
        default: break;
        }
    }

    if (role == Qt::CheckStateRole) {
        switch (column) {
        case Column::NotGenUndef: return toVariant(item->notGenUndefFlag());
        case Column::Active: return toVariant(item->activeFlag());
        case Column::Inversion: return toVariant(item->stateInversionFlag());
        default: break;
        }
    }

    return QVariant();
}

bool DPinModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    auto item = items().at(index.row());
    auto column = index.column();
    bool changed = false;

    auto flag = value.toBool();
    if (Column::NotGenUndef == column) {
        item->setNotGenUndefFlag(flag);
        changed = true;
    }
    if (Column::Active == column) {
        item->setActiveFlag(flag);
        changed = true;
    }
    if (Column::Inversion == column) {
        item->setStateInversionFlag(flag);
        changed = true;
    }
    if (Column::State == column) {
        item->setState(static_cast<DPinSignal::State>(value.toUInt()));
        changed = true;
    }

    auto uintValue = value.toUInt();
    if (Column::RPO == column) {
        item->setRpo(uintValue);
        changed = true;
    }
    if (Column::RPV == column) {
        item->setRpv(uintValue);
        changed = true;
    }
    if (Column::IntermediateTime == column) {
        item->setIntermediateTime(uintValue);
        changed = true;
    }
    if (Column::IntermediateTimeRatio == column) {
        item->setIntermediateTimeRatio(uintValue);
        changed = true;
    }

    if (changed) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
