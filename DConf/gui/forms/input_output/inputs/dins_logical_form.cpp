#include <gui/forms/input_output/inputs/dins_logical_form.h>

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/din_model.h"

DinsLogicalForm::DinsLogicalForm(DcController* controller) : DcForm(controller,  "Настройки логических дискретных входов", false)
{
    auto dins = controller->signalManager().getSignals<DinSignal>(Signal::Subtype::Logical);
    auto model = new DinModel(dins, this);
	Dpc::Gui::TableView* tableView = new Dpc::Gui::TableView(model, this);

	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);

	tableView->horizontalHeader()->setStretchLastSection(false);
	tableView->horizontalHeader()->setHighlightSections(false);
	tableView->horizontalHeader()->setFixedHeight(45);
	tableView->horizontalHeader()->setHighlightSections(false);

    QList<int> visibleColumns;
    visibleColumns << DinModel::Column::Number;
    visibleColumns << DinModel::Column::Name;
    visibleColumns << DinModel::Column::Oscill;
    visibleColumns << DinModel::Column::Journal;
    visibleColumns << DinModel::Column::Archive;
    visibleColumns << DinModel::Column::DP;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Oscill, 150);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Journal, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Archive, 70);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DP, 50);

    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DinsLogicalForm::isAvailableFor(DcController* controller)
{
	static QList<Param> params = {
		{SP_DIN_DINOSCMASK},
		{SP_TREND_INITIALISE},
		{SP_DIN_CASH_REQMASK},
		{SP_DIN_INT_CNT},
		{SP_DIN_DEBPARS_DOUBLE},
		{SP_DIN_INT_WDT},
		{SP_DIN_NOISE_WDT},
		{SP_DIN_INVERS},
		{SP_DIN_VDINSAVED},
		{SP_DIN_VDINFIXED},
		{SP_DIN_DPSREPRESENT}
	};

	return hasAny(controller, params);
}

void DinsLogicalForm::fillReport(DcIConfigReport* report)
{
}
