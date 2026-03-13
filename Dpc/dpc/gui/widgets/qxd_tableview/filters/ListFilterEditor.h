#ifndef LISTFILTEREDITOR_H
#define LISTFILTEREDITOR_H

#include <dpc/gui/widgets/qxd_tableview/filters/FilterEditor.h>
#include <dpc/gui/widgets/qxd_tableview/filters/ListFilterModel.h>

class QAbstractItemModel;

class ListFilterEditor : public FilterEditor
{
	Q_OBJECT

public:
    ListFilterEditor(ListFilter *filter, QAbstractItemModel *source_model, int column, int filterRole, OtherFiltersAcceptFunc accept, QWidget *parent = nullptr);
    virtual void apply() override;

signals:
	void applyPressed();
};

#endif // LISTFILTEREDITOR_H
