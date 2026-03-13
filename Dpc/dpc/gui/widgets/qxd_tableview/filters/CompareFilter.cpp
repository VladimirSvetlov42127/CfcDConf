#include "CompareFilter.h"

#include <QDebug>
#include <QModelIndex>

CompareFilter::CompareFilter() :
    m_comp1(Qxd::Unknown),
    m_comp2(Qxd::Unknown),
    m_comb(Qxd::AND)
{
}

CompareFilter::~CompareFilter()
{
}

bool CompareFilter::accept(const QModelIndex &index, int filterRole) const
{
    QVariant value = index.data(filterRole);
    if (operator1() == Qxd::Unknown)
        return Qxd::compare(operator2(), value, m_value2);
    if (operator2() == Qxd::Unknown)
        return Qxd::compare(operator1(), value, m_value1);

    bool expression1 = Qxd::compare(operator1(), value, m_value1);
    bool expression2 = Qxd::compare(operator2(), value, m_value2);

    if (combination() == Qxd::AND)
        return expression1 && expression2;

    return expression1 || expression2;
}

void CompareFilter::reset()
{
    m_comp1 = Qxd::Unknown;
    m_comp2 = Qxd::Unknown;
    m_comb = Qxd::AND;
	m_value1 = QVariant();
    m_value2 = QVariant();
}

void CompareFilter::onSetEqualTo(const QList<QVariant> &list)
{
    for(size_t i = 0; i < 2; i++) {
        if (!i) {
            m_comp1 = Qxd::Equal;
            m_value1 = list.at(i);
        }
        else {
            m_comp2 = Qxd::Equal;
            m_value2 = list.at(i);
        }
    }
}
