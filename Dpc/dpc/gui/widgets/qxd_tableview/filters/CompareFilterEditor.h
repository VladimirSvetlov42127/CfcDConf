#ifndef COMPAREFILTEREDITOR_H
#define COMPAREFILTEREDITOR_H

#include <dpc/gui/widgets/qxd_tableview/filters/FilterEditor.h>
#include <dpc/gui/widgets/qxd_tableview/filters/CompareFilter.h>

class CompareFilterEditor :	public FilterEditor
{
public:
    CompareFilterEditor(CompareFilter *filter, const QVariant &min = QVariant(), const QVariant &max = QVariant(), QWidget *parent = nullptr);
	virtual ~CompareFilterEditor();

protected:
    virtual void apply() override;

private:
    CompareFilter *m_filter;

    Qxd::CompareOperator m_op1;
    Qxd::CompareOperator m_op2;
    Qxd::Combination m_comb;

    QVariant m_value1;
    QVariant m_value2;
};

#endif // COMPAREFILTEREDITOR_H
