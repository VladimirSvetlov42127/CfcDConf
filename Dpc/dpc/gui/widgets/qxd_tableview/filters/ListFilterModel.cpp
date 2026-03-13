#include "ListFilterModel.h"

#include <QDebug>
#include <QTime>
#include <QFont>

static const QString emptyString = "(Пустые)";

ListFilterModel::ListFilterModel(ListFilter *filter, OtherFiltersAcceptFunc accept, QObject *parent) :
    QSortFilterProxyModel(parent),
    m_filter(filter),
    m_mask(filter->mask()),
    m_caseSens(filter->sensitivity()),
    m_exactMatch(filter->exactMatch()),
    m_value1(filter->value1()),
    m_value2(filter->value2()),
    m_comp1(filter->operator1()),
    m_comp2(filter->operator2()),
    m_comb(filter->combination()),
    m_otherAccept(accept)
{
}

ListFilterModel::~ListFilterModel()
{
}

Qt::ItemFlags ListFilterModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags fgs = Qt::ItemIsUserCheckable;
    if (m_items[mapToSource(index).data().toString()].isOtherAccepted)
        fgs |= Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    return fgs;

}

QVariant ListFilterModel::data(const QModelIndex &proxyIndex, int role) const
{
    if (!proxyIndex.isValid())
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);

    if (role == Qt::FontRole) {
        auto it = getIt(proxyIndex);
        if (it->first.isEmpty()) {
            QFont ft;
            ft.setItalic(true);
            return ft;
        }
    }

    if (role == Qt::DisplayRole) {
        auto it = getIt(proxyIndex);
        return !it->first.isEmpty() ? it->first : emptyString;
    }

    if (role == Qt::CheckStateRole)
        return getIt(proxyIndex)->second.checked ? Qt::Checked : Qt::Unchecked;

    return QVariant();
}

bool ListFilterModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::CheckStateRole) {
        auto it = getIt(index);
        if (it->second.checked == value.toBool())
            return false;

        it->second.checked = value.toBool();
        emit allCheckStateChanged(allCheckState());
        return true;
    }

    return QSortFilterProxyModel::setData(index, value, role);
}

void ListFilterModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QAbstractProxyModel::setSourceModel(sourceModel);
    connect(sourceModel, &QAbstractItemModel::dataChanged, this, [=](const QModelIndex &source_top_left,
            const QModelIndex &source_bottom_right) {
        if (!source_top_left.isValid() || !source_bottom_right.isValid())
                return;

        update();
    });
}

Qt::CheckState ListFilterModel::allCheckState() const
{
    bool isAllChecked = true;
    bool isAllUnchecked = true;
    for(auto it = m_items.begin(); it != m_items.end(); it++) {
        if (!it->second.isMatchingMask || !it->second.isOtherAccepted)
            continue;

        if (isAllChecked)
            isAllChecked = it->second.checked;
        if (isAllUnchecked)
            isAllUnchecked = !it->second.checked;
        if (!isAllChecked && !isAllUnchecked)
            return Qt::PartiallyChecked;
    }

    return isAllUnchecked ? Qt::Unchecked : Qt::Checked;
}

void ListFilterModel::setAllCheckState(bool checked)
{
    layoutAboutToBeChanged();
    for(auto it = m_items.begin(); it != m_items.end(); it++)
        if (it->second.isMatchingMask && it->second.isOtherAccepted)
            it->second.checked = checked;
    layoutChanged();
    emit allCheckStateChanged(checked ? Qt::Checked : Qt::Unchecked);
}

void ListFilterModel::saveToFilter()
{
    m_filter->setMask(m_mask);
    m_filter->setSensitivity(m_caseSens);
    m_filter->setExactMatch(m_exactMatch);
    m_filter->setValue1(m_value1);
    m_filter->setValue2(m_value2);
    m_filter->setOperator1(m_comp1);
    m_filter->setOperator2(m_comp2);
    m_filter->setCombination(m_comb);

    m_filter->checkedStrings().clear();
    m_filter->acceptedStrings().clear();
    for(auto it = m_items.begin(); it != m_items.end(); it++)
        if (it->second.checked) {
            m_filter->checkedStrings().insert(it->first);
            if (it->second.isMatchingMask || !it->second.isOtherAccepted)
                m_filter->acceptedStrings().insert(it->first);
        }
    m_filter->setAllChecked(m_items.size() == m_filter->checkedStrings().size());
    m_filter->setEnabled(m_items.size() != m_filter->acceptedStrings().size());
}

void ListFilterModel::setMask(const QString &mask)
{
    m_mask = mask;
    update();
}

void ListFilterModel::setCaseSensitivity(Qt::CaseSensitivity cs)
{
    m_caseSens = cs;
    if (!m_mask.isEmpty())
        update();
}

void ListFilterModel::setExactMatch(bool match)
{
    m_exactMatch = match;
    if (!m_mask.isEmpty())
        update();
}

void ListFilterModel::setValue1(const QVariant &val)
{
    m_value1 = val;
    if (compareIsValid())
        update();
}

void ListFilterModel::setValue2(const QVariant &val)
{
    m_value2 = val;
    if (compareIsValid())
        update();
}

void ListFilterModel::setOperator1(Qxd::CompareOperator op)
{
    m_comp1 = op;
    update();
}

void ListFilterModel::setOperator2(Qxd::CompareOperator op)
{
    m_comp2 = op;
    update();
}

void ListFilterModel::setCombination(Qxd::Combination comb)
{
    m_comb = comb;
    if (compareIsValid())
        update();
}

bool ListFilterModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    return source_column == filterKeyColumn();
}

bool ListFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);
    QString str = index.data().toString();
    auto insertIt = m_items.insert(std::make_pair(str, ListFilterItem()));
    auto itemIt = insertIt.first;
    if (insertIt.second)
        itemIt->second.checked = m_filter->checked(str);

    if (!itemIt->second.isOtherAccepted)
        itemIt->second.isOtherAccepted = m_otherAccept(source_row);

    if (itemIt->second.isAlreadyFiltered)
        return false;

    itemIt->second.isAlreadyFiltered = true;
    itemIt->second.isMatchingMask = matchMask(str, index);
    return itemIt->second.isMatchingMask;
}

ListFilterItemsHash::iterator ListFilterModel::getIt(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.row()) {
        auto it = m_items.find(QString());
        if (it != m_items.end() && it->second.isMatchingMask)
            return it;
    }

    QString str = mapToSource(proxyIndex).data().toString();
    if (str.isEmpty())
        return m_items.find(mapToSource(index(0, proxyIndex.column())).data().toString());

    return m_items.find(str);
}

bool ListFilterModel::compareIsValid() const
{
    if (m_comp1 || m_comp2)
        return true;

    return false;
}

bool ListFilterModel::matchCompares(const QModelIndex &index) const
{
    if (!compareIsValid())
        return true;

    QVariant value = index.data(filterRole());
    if (m_comp1 == Qxd::Unknown)
        return Qxd::compare(m_comp2, value, m_value2);
    if (m_comp2 == Qxd::Unknown)
        return Qxd::compare(m_comp1, value, m_value1);

    bool expression1 = Qxd::compare(m_comp1, value, m_value1);
    bool expression2 = Qxd::compare(m_comp2, value, m_value2);

    if (m_comb == Qxd::AND)
        return expression1 && expression2;

    return expression1 || expression2;
}

bool ListFilterModel::matchMask(const QString &str, const QModelIndex &index) const
{
    if (!matchCompares(index))
        return false;

    if (m_mask.isEmpty())
        return true;

    if (m_exactMatch)
        return str.compare(m_mask, m_caseSens) == 0;

    return str.contains(m_mask, m_caseSens);
}

void ListFilterModel::update()
{
    for(auto it = m_items.begin(); it != m_items.end(); it++)
        it->second.isAlreadyFiltered = false;
    invalidate();
    emit allCheckStateChanged(allCheckState());
}
