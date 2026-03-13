#include "AnalogsModel.h"

#include "data_model/dc_controller.h"

struct AnalogsModel::Item
{
    QString name;
    ParameterElement* ainOscParam = nullptr;
};

AnalogsModel::AnalogsModel(DcController *controller, QObject *parent)
    : QAbstractTableModel(parent)
{
    auto ainsToOscParam = controller->paramsRegistry().parameter(SP_AINS_TO_OSC);
    if (!ainsToOscParam)
        return;

    uint16_t ainsOscMax = 0;
    if (auto param = controller->paramsRegistry().element(SP_OSC_LIMITS, 0))
        ainsOscMax = param->value().toUInt();

    uint16_t ainsOscUsed = 0;
    if (auto param = controller->paramsRegistry().element(SP_OSC_LIMITS, 1))
        ainsOscUsed = param->value().toUInt();

    if(DcController::Type::Deprotec == controller->type())
    {
        ainsOscUsed = 0;
        for(auto &board: controller->boards())
            ainsOscUsed += board->ainsOscCount();
    }

    if (ainsOscUsed > ainsOscMax)
        return;

    uint16_t freeAinsOscCount = ainsOscMax - ainsOscUsed;
    freeAinsOscCount = std::min(freeAinsOscCount, ainsToOscParam->elementsCount());

    for(size_t i = 0; i < freeAinsOscCount; ++i)
    {
        auto param = controller->paramsRegistry().element(SP_AINS_TO_OSC, i);
        if (!param)
            continue;

        m_items.append({QString("Тренд %1").arg(ainsOscUsed + i + 1), param});
    }
}

int AnalogsModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_items.size();
}

int AnalogsModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return AnalogsModel::Columns::ColumnsCount;
}

QVariant AnalogsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role)
    {
        switch(section)
        {
            case Trand: return "Тренд";
            case Analog: return "Аналог";
            default: break;
        }
    }

    return QVariant();
}

Qt::ItemFlags AnalogsModel::flags(const QModelIndex &index) const
{
    auto fl = QAbstractTableModel::flags(index);

    if(Columns::Analog == index.column())
        return fl | Qt::ItemIsEditable;

    return fl;
}

QVariant AnalogsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto item = m_items[index.row()];
    int col = index.column();

    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;

    if (Qt::EditRole == role || Qt::DisplayRole == role)
    {
        switch(col)
        {
            case Trand: return item.name;
            case Analog: return item.ainOscParam->value();
            default: break;
        }
    }

    return QVariant();
}

bool AnalogsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    int col = index.column();
    auto &item = m_items[row];

    if (!index.isValid())
        return false;

    if (Qt::EditRole == role)
    {
        if (col == Analog)
            item.ainOscParam->updateValue(value.toString());

        emit dataChanged(index, index);
        return true;
    }

    return false;
}
