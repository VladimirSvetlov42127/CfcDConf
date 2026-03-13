#include "base_tab_model.h"

#include <QBrush>
#include <QColor>

struct BaseTabModel::Item
{
    QString paramName;
    QString paramValue;
    QString description;
};

BaseTabModel::BaseTabModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent, const DescriptionFunc &descriptionFunc)
    : QAbstractTableModel(parent)
    , m_addr(addr)
    , m_profile(profile)
    , m_columnValue(columnValue)
    , m_descriptionFunc(descriptionFunc)
    , m_descExist(false)
{

}

BaseTabModel::~BaseTabModel()
{

}

int BaseTabModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_items.size();
}

int BaseTabModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_descExist ? Columns::ColumnsCount : Columns::ColumnsCount - 1;
}

QVariant BaseTabModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role)
    {
        switch(section)
        {
        case Parametr:
            return "Параметр";
            break;
        case Value:
            return m_columnValue;
            break;
        case Description:
            return "Описание";
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags BaseTabModel::flags(const QModelIndex &index) const
{
    auto fl = QAbstractTableModel::flags(index);

    return fl;
}

QVariant BaseTabModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto &item = m_items[index.row()];
    int col = index.column();

    if (Columns::Description == col && Qt::TextAlignmentRole == role)
        return Qt::AlignVCenter;

    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;

    if (Qt::ToolTipRole == role) {
        if (Description == col)
            return item.description;
    }

    if (Qt::DisplayRole == role)
    {
        switch(col)
        {
        case Parametr:
            return item.paramName;
            break;
        case Value:
            return item.paramValue;
            break;
        case Description:
            return item.description;
            break;
        }
    }

    if (Qt::BackgroundRole == role) {
        if (!item.description.isEmpty())
            return QBrush(QColor(Qt::red));
    }

    return QVariant();
}

void BaseTabModel::updateInfo(const InformationOperation::ParamsContainer &container)
{
    auto it = container.find(m_addr);
    if (it == container.end())
        return;

    if (it->second.size() <= m_profile)
        return;

    beginResetModel();

    m_items.clear();
    m_descExist = false;

    for (const auto &element : it->second.at(m_profile)) {
        Item item;

        item.paramName = element.second.name;
        item.paramValue = element.second.value;
        if (m_descriptionFunc) {
            item.description = m_descriptionFunc(element.first, element.second.value.toUInt());

            if (!item.description.trimmed().isEmpty())
                m_descExist = true;
        }

        m_items.append(item);
    }

    endResetModel();
}

bool BaseTabModel::hasError()
{
    return m_descExist;
}

