#include "sm_flags_model.h"

SmFlagsModel::SmFlagsModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent, const DescriptionFunc &descriptionFunc)
    : BaseTabModel(addr, profile, columnValue, parent, descriptionFunc)
{

}

int SmFlagsModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return Columns::ColumnsCount - 1;
}

QVariant SmFlagsModel::data(const QModelIndex &index, int role) const
{
    if (BaseTabModel::Columns::Value == index.column()) {
        if (Qt::CheckStateRole == role) {
            auto value = BaseTabModel::data(index, Qt::DisplayRole);
            return value.toUInt() != 0 ? Qt::Checked : Qt::Unchecked;
        }

        if (Qt::DisplayRole == role)
            return QVariant();
    }

    return BaseTabModel::data(index, role);
}
