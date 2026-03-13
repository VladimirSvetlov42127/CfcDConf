#ifndef LISTFILTER_H
#define LISTFILTER_H

#include <QSet>
#include <QVariant>

#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>

class ListFilter :public QxdIFilter
{
public:
    ListFilter(QVariant::Type dataType);

	virtual int type() const override;
    virtual bool accept(const QModelIndex &index, int filterRole) const override;    
    virtual void reset() override;

    bool checked(const QString &str) const;

    QVariant::Type dataType() const { return m_dataType; }

    QString mask() const { return m_mask; }
    void setMask(const QString &mask) { m_mask = mask; }

    Qt::CaseSensitivity sensitivity() const { return m_caseSens; }
    void setSensitivity(Qt::CaseSensitivity cs) { m_caseSens = cs; }

    bool exactMatch() const { return m_exactMatch; }
    void setExactMatch(bool match) { m_exactMatch = match; }

    QSet<QString>& acceptedStrings() { return m_acceptedStrings; }
    QSet<QString>& checkedStrings() { return m_checkedStrings; }

    void setAllChecked(bool checked);

    void setValue1(const QVariant &d) { m_value1 = d; }
    void setValue2(const QVariant &d) { m_value2 = d; }

    QVariant value1() const { return m_value1; }
    QVariant value2() const { return m_value2; }

    Qxd::Combination combination() const { return m_comb; }
    void setCombination(Qxd::Combination comb) { m_comb = comb; }

    Qxd::CompareOperator operator1() const { return m_comp1; }
    Qxd::CompareOperator operator2() const { return m_comp2; }

    void setOperator1(Qxd::CompareOperator op) { m_comp1 = op; }
    void setOperator2(Qxd::CompareOperator op) { m_comp2 = op; }

protected:
    virtual void onSetEqualTo(const QList<QVariant> &list) override;

private:
    QVariant::Type m_dataType;
    QString m_mask;
    Qt::CaseSensitivity m_caseSens;
    bool m_exactMatch;
    QSet<QString> m_acceptedStrings;
    QSet<QString> m_checkedStrings;
    bool m_allChecked;
    QVariant m_value1;
    QVariant m_value2;
    Qxd::CompareOperator m_comp1;
    Qxd::CompareOperator m_comp2;
    Qxd::Combination m_comb;
};

#endif // LISTFILTER_H
