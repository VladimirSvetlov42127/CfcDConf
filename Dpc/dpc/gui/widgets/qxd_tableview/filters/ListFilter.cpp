#include "ListFilter.h"

#include <QModelIndex>

ListFilter::ListFilter(QVariant::Type dataType) :
    m_dataType(dataType),
    m_caseSens(Qt::CaseInsensitive),
    m_exactMatch(false),
    m_allChecked(true),
    m_comp1(Qxd::Unknown),
    m_comp2(Qxd::Unknown),
    m_comb(Qxd::AND)
{
}

int ListFilter::type() const
{
    return Qxd::List;
}

bool ListFilter::accept(const QModelIndex &index, int filterRole) const
{
    return m_acceptedStrings.contains(index.data().toString());
}

void ListFilter::reset()
{
    m_caseSens = Qt::CaseInsensitive;
    m_exactMatch = false;
    m_acceptedStrings.clear();
    m_checkedStrings.clear();
    m_allChecked = true;
    m_value1 = QVariant();
    m_value2 = QVariant();
    m_comp1 = Qxd::Unknown;
    m_comp2 = Qxd::Unknown;
    m_comb = Qxd::AND;
}

bool ListFilter::checked(const QString &str) const
{
    return m_allChecked ? true : m_checkedStrings.contains(str);
}

void ListFilter::setAllChecked(bool checked)
{
    m_allChecked = checked;
    if (checked)
        m_checkedStrings.clear();
}

void ListFilter::onSetEqualTo(const QList<QVariant> &list)
{
    for(auto &it: list) {
        m_acceptedStrings.insert(it.toString());
        m_checkedStrings.insert(it.toString());
    }

    m_allChecked = false;
}
