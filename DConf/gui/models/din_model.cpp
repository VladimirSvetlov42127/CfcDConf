#include "din_model.h"

#include "service_manager/signals/din_acp_signal.h"
#include "service_manager/signals/din_physical_signal.h"
#include "service_manager/signals/din_virtual_signal.h"

DinModel::DinModel(const std::vector<DinSignal*>& items, QObject* parent)
    : QAbstractTableModel{parent}
    , m_items{items}
{
}

int DinModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_items.size();
}

int DinModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Column::__Count;
}

QVariant DinModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case Column::Number:           return "№";
    case Column::Name:             return "Название";
    case Column::Oscill:           return "Осциллографирование";
    case Column::Journal:          return "Журнал";
    case Column::Archive:          return "Архив";
    case Column::Drebezg:          return "Постоянная обработки\n дребезга (мс)";
    case Column::DrebezgRatio:     return "Коэффициент обработки\n дребезга";
    case Column::DrebezgTimer:     return "Сторожевой таймер\n дребезга (мс)";
    case Column::DrebezgNoise:     return "Параметр отстройки\n от помех (мс)";
    case Column::Save:             return "Сохраняемый";
    case Column::Fix1:             return "Фиксация в 1";
    case Column::Inversion:        return "Инверсия";
    case Column::ThresholdChain:   return "Порог обравы цепи";
    case Column::Threshold0:       return "Порог лог. 0-ля";
    case Column::Threshold1:       return "Порог лог. 1-цы";
    case Column::ThresholdKZ:      return "Порог КЗ";
    case Column::DP:               return "DP";
    case Column::IEC101Select:     return "Опрос";
    case Column::IEC101Group1:     return "Группа 1";
    case Column::IEC101Group2:     return "Группа 2";
    case Column::IEC101Background: return "Циклическая передача";
    case Column::IEC103Select:     return "Опрос";
    case Column::SpodesSelect:     return "Опрос";
    }

    return QVariant();
}

Qt::ItemFlags DinModel::flags(const QModelIndex &index) const
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

QVariant DinModel::data(const QModelIndex &index, int role) const
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

    if (role == Qt::EditRole && column == DinModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote))
            return item->to<ISignalCustomName>()->customName();
    }

    if (role == Qt::DisplayRole || role == Qt::EditRole) {
        switch (column) {
        case Column::Number: return item->internalID();
        case Column::Name: return item->name();
        default: break;
        }

        if (item->subtype() & Signal::Subtype::Physical) {
            auto din = static_cast<DinPhysicalSignal*>(item);
            switch (column) {
            case Column::Drebezg: return toVariant(din->drebezg());
            case Column::DrebezgRatio: return toVariant(din->drebezgRatio());
            case Column::DrebezgTimer: return toVariant(din->drebezgTimer());
            case Column::DrebezgNoise: return toVariant(din->drebezgNoise());
            default: break;
            }
        }

        if (item->subtype() & Signal::Subtype::Acp) {
            auto din = static_cast<DinAcpSignal*>(item);
            switch (column) {
            case Column::Drebezg: return toVariant(din->drebezg());
            case Column::DrebezgTimer: return toVariant(din->drebezgTimer());
            case Column::DrebezgNoise: return toVariant(din->drebezgNoise());
            case Column::ThresholdChain: return toVariant(din->thresholdChain());
            case Column::Threshold0: return toVariant(din->threshold0());
            case Column::Threshold1: return toVariant(din->threshold1());
            case Column::ThresholdKZ: return toVariant(din->thresholdKZ());
            default: break;
            }
        }
    }

    if (role == Qt::CheckStateRole) {
        switch (column) {
        case Column::Oscill: return toVariant(item->oscillFlag());
        case Column::Journal: return toVariant(item->trendFlag());
        case Column::Archive: return toVariant(item->archiveFlag());
        case Column::DP: return toVariant(item->dpFlag());
        case Column::IEC101Select: return toVariant(item->iec101SelectFlag());
        case Column::IEC101Group1: return toVariant(item->iec101Group1Flag());
        case Column::IEC101Group2: return toVariant(item->iec101Group2Flag());
        case Column::IEC101Background: return toVariant(item->iec101BackgroundFlag());
        case Column::IEC103Select: return toVariant(item->iec103SelectFlag());
        case Column::SpodesSelect: return toVariant(item->spodesSelectFlag());
        default: break;
        }

        if (item->subtype() & Signal::Subtype::Virtual) {
            auto vdin = static_cast<DinVirtualSignal*>(item);
            switch (column) {
            case Column::Save: return toVariant(vdin->saveFlag());
            case Column::Fix1: return toVariant(vdin->fix1Flag());
            default: break;
            }
        }

        if (item->subtype() & Signal::Subtype::Physical && column == Column::Inversion)
            return toVariant(static_cast<DinPhysicalSignal*>(item)->inversionFlag());
        if (item->subtype() & Signal::Subtype::Acp && column == Column::Inversion)
            return toVariant(static_cast<DinAcpSignal*>(item)->inversionFlag());
    }

    return QVariant();
}

bool DinModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid())
        return false;

    auto item = items().at(index.row());
    auto column = index.column();    
    bool changed = false;

    if (role == Qt::EditRole && column == DinModel::Column::Name) {
        if (item->subtype() & (Signal::Subtype::Virtual | Signal::Subtype::Remote)) {
            item->to<ISignalCustomName>()->setCustomName(value.toString().trimmed());
            changed = true;
        }
    }

    auto flag = value.toBool();
    if (Column::Oscill == column) {
        item->setOscillFlag(flag);
        changed = true;
    }
    if (Column::Journal == column) {
        item->setTrendFlag(flag);
        changed = true;
    }
    if (Column::Archive == column) {
        item->setArchiveFlag(flag);
        changed = true;
    }
    if (Column::DP == column) {
        item->setDPFlag(flag);
        changed = true;
    }
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
    if (Column::IEC103Select == column) {
        item->setIec103SelectFlag(flag);
        changed = true;
    }
    if (Column::SpodesSelect == column) {
        item->setSpodes101SelectFlag(flag);
        changed = true;
    }
    if (item->subtype() & Signal::Subtype::Virtual) {
        auto vdin = static_cast<DinVirtualSignal*>(item);
        if (Column::Save == column) {
            vdin->setSaveFlag(flag);
            changed = true;
        }
        if (Column::Fix1 == column) {
            vdin->setFix1Flag(flag);
            changed = true;
        }
    }
    if (item->subtype() & Signal::Subtype::Physical && column == Column::Inversion) {
        static_cast<DinPhysicalSignal*>(item)->setInversionFlag(flag);
        changed = true;
    }
    if (item->subtype() & Signal::Subtype::Acp && column == Column::Inversion) {
        static_cast<DinAcpSignal*>(item)->setInversionFlag(flag);
        changed = true;
    }

    if (item->subtype() & Signal::Subtype::Physical) {
        uint8_t uintVal = value.toUInt();
        auto din = static_cast<DinPhysicalSignal*>(item);
        switch (column) {
        case Column::Drebezg:
            din->setDrebezg(uintVal);
            changed = true;
            break;
        case Column::DrebezgRatio:
            din->setDrebezgRatio(uintVal);
            changed = true;
            break;
        case Column::DrebezgTimer:
            din->setDrebezgTimer(uintVal);
            changed = true;
            break;
        case Column::DrebezgNoise:
            din->setDrebezgNoise(uintVal);
            changed = true;
            break;
        default:
            break;
        }
    }

    if (item->subtype() & Signal::Subtype::Acp) {
        uint8_t uintVal = value.toUInt();
        double doubleVal = value.toDouble();
        auto din = static_cast<DinAcpSignal*>(item);
        switch (column) {
        case Column::Drebezg:
            din->setDrebezg(uintVal);
            changed = true;
            break;
        case Column::DrebezgTimer:
            din->setDrebezgTimer(uintVal);
            changed = true;
            break;
        case Column::DrebezgNoise:
            din->setDrebezgNoise(uintVal);
            changed = true;
            break;
        case Column::ThresholdChain:
            din->setThresholdChain(doubleVal);
            changed = true;
            break;
        case Column::Threshold0:
            din->setThreshold0(doubleVal);
            changed = true;
            break;
        case Column::Threshold1:
            din->setThreshold1(doubleVal);
            changed = true;
            break;
        case Column::ThresholdKZ:
            din->setThresholdKZ(doubleVal);
            changed = true;
            break;
        default:
            break;
        }
    }

    if (changed) {
        emit dataChanged(index, index);
        return true;
    }

    return false;
}
