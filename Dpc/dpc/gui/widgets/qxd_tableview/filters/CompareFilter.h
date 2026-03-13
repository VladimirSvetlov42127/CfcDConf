#ifndef COMPAREFILTER_H
#define COMPAREFILTER_H

#include <QVariant>

#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>

class CompareFilter : public QxdIFilter
{
public:
	CompareFilter();
	virtual ~CompareFilter();

	inline void setValue1(const QVariant &d) { m_value1 = d; }
	inline void setValue2(const QVariant &d) { m_value2 = d; }

	inline QVariant value1() const { return m_value1; }
	inline QVariant value2() const { return m_value2; }

    inline Qxd::Combination combination() const { return m_comb; }
    inline void setCombination(Qxd::Combination comb) { m_comb = comb; }

    inline Qxd::CompareOperator operator1() const { return m_comp1; }
    inline Qxd::CompareOperator operator2() const { return m_comp2; }

    inline void setOperator1(Qxd::CompareOperator op) { m_comp1 = op; }
    inline void setOperator2(Qxd::CompareOperator op) { m_comp2 = op; }

    virtual bool accept(const QModelIndex &index, int filterRole) const override;    
    virtual void reset() override;

protected:
    virtual void onSetEqualTo(const QList<QVariant> &list) override;

private:
	QVariant m_value1;
	QVariant m_value2;

    Qxd::CompareOperator m_comp1;
    Qxd::CompareOperator m_comp2;

    Qxd::Combination m_comb;
};

class NumericFilter : public CompareFilter
{
public:
    virtual int type() const override { return Qxd::Numeric; }
};

class TimeFilter : public CompareFilter
{
public:
    virtual int type() const override { return Qxd::Time; }
};

class DateFilter : public CompareFilter
{
public:
    virtual int type() const override { return Qxd::Date; }
};

class DateTimeFilter : public CompareFilter
{
public:
    virtual int type() const override { return Qxd::DateTime; }
};

#endif // COMPAREFILTER_H

