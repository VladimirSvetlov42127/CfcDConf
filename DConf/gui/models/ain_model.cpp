#include "ain_model.h"

#include "service_manager/signals/signal_icustom_name.h"

AinModel::AinModel(const std::vector<AinSignal*>& items, QObject* parent)
    : QAbstractTableModel{parent}
    , m_items{items}
{
}

int AinModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int AinModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Column::__Count;
}

QVariant AinModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case Column::Number:               return "№";
    case Column::Name:                 return "Название";
    case Column::ApertureType:         return "Тип апертуры";
    case Column::ApertureVal:          return "Значение апертуры";
    case Column::ThresholdSensibility: return "Порог чувствительности";
    case Column::Threshold0:           return "Порог лог. 0-ля";
    case Column::Threshold1:           return "Порог лог. 1-цы";
    case Column::IEC101Select:         return "Опрос";
    case Column::IEC101Group1:         return "Группа 1";
    case Column::IEC101Group2:         return "Группа 2";
    case Column::IEC101Background:     return "Циклическая передача";
    case Column::SpodesSelect:         return "Опрос";
    }

    return QVariant();
}

Qt::ItemFlags AinModel::flags(const QModelIndex &index) const
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

QVariant AinModel::data(const QModelIndex &index, int role) const
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

    if (role == Qt::EditRole && column == AinModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote))
            return item->to<ISignalCustomName>()->customName();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case Column::Number: return item->internalID();
        case Column::Name: return item->name();
        case Column::ApertureType: return toVariant(item->apertureType());
        case Column::ApertureVal: return toVariant(item->apertureVal());
        case Column::ThresholdSensibility: return toVariant(item->thresholdSensibility());
        case Column::Threshold0: return toVariant(item->threshold0());
        case Column::Threshold1: return toVariant(item->threshold1());
        default: break;
        }
    }

    if (role == Qt::CheckStateRole) {
        switch (column) {
        case Column::IEC101Select: return toVariant(item->iec101SelectFlag());
        case Column::IEC101Group1: return toVariant(item->iec101Group1Flag());
        case Column::IEC101Group2: return toVariant(item->iec101Group2Flag());
        case Column::IEC101Background: return toVariant(item->iec101BackgroundFlag());
        case Column::SpodesSelect: return toVariant(item->spodesSelectFlag());
        default: break;
        }
    }

    return QVariant();
}

bool AinModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    auto item = items().at(index.row());
    auto column = index.column();    
    bool changed = false;

    if (role == Qt::EditRole && column == AinModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote)) {
            item->to<ISignalCustomName>()->setCustomName(value.toString().trimmed());
            changed = true;
        }
    }

    auto flag = value.toBool();
    if (Column::IEC101Select == column) {
        item->setIec101SelectFlag(flag);
        changed = true;
    }
    if (Column::IEC101Group1 == column) {
        item->setIec101Group1Flag(flag);
        changed = true;
    }
    if (Column::IEC101Group2 == column) {
        item->setIec101Group2Flag(flag);
        changed = true;
    }
    if (Column::IEC101Background == column) {
        item->setIec101BackgroundFlag(flag);
        changed = true;
    }
    if (Column::SpodesSelect == column) {
        item->setSpodes101SelectFlag(flag);
        changed = true;
    }

    if (Column::ApertureType == column && role == Qt::EditRole) {
        item->setApertureType(value.toUInt());
        changed = true;
    }
    if (Column::ApertureVal == column && role == Qt::EditRole) {
        item->setApertureVal(value.toDouble());
        changed = true;
    }
    if (Column::ThresholdSensibility == column && role == Qt::EditRole) {
        item->setThresholdSensibility(value.toDouble());
        changed = true;
    }
    if (Column::Threshold0 == column && role == Qt::EditRole) {
        item->setThreshold0(value.toDouble());
        changed = true;
    }
    if (Column::Threshold1 == column && role == Qt::EditRole) {
        item->setThreshold1(value.toDouble());
        changed = true;
    }

    if (changed) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
