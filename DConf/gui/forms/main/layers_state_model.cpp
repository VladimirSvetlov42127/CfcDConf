#include "layers_state_model.h"

LayersStateModel::LayersStateModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent, const DescriptionFunc &descriptionFunc)
    : BaseTabModel(addr, profile, columnValue, parent, descriptionFunc)
{
}

QVariant LayersStateModel::data(const QModelIndex &index, int role) const
{
    auto dataValue = BaseTabModel::data(index, role);
    if (Qt::DisplayRole == role && BaseTabModel::Columns::Value == index.column())
        return QString("0x%1").arg(dataValue.toString().toUInt(), 8, 16, QChar('0'));

    return dataValue;
}
