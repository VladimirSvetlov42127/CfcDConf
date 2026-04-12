#include "douts_form.h"

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/dout_model.h"

using namespace Dpc::Gui;

DoutsForm::DoutsForm(DcController *controller)
    : DcForm(controller,  "Поканальные настройки дискретных выходов", false)
{
    auto douts = controller->signalManager().getSignals<DoutSignal>();
    auto model = new DoutModel(douts, this);

    auto tableView = new Dpc::Gui::TableView(model, this);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);
    tableView->horizontalHeader()->setHighlightSections(false);

    QList<int> visibleColumns;
    visibleColumns << DoutModel::Column::Number;
    visibleColumns << DoutModel::Column::Name;
    visibleColumns << DoutModel::Column::Impulse;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DoutModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DoutModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(DoutModel::Column::Impulse, 300);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget());
    layout->addWidget(tableView);
}

bool DoutsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DOUT_SAMPLTIM}
	};

	return hasAny(controller, params);
}

void DoutsForm::fillReport(DcIConfigReport * report)
{
//	report->insertSection();

//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	DcReportTable table(report->device(), headers, { 5 });
//	for (auto &&it : getItems(report->device())) {
//		QStringList values;
//		for (size_t col = 0; col < it.data.size(); col++) {
//			values << it.data[col].toString();
//		}

//		table.addRow(values);
//	}

//	report->insertTable(table);
}
