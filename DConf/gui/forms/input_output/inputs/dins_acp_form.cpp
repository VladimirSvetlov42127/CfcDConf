 #include "dins_acp_form.h"

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

#include "gui/models/din_model.h"

DinsAcpForm::DinsAcpForm(DcController *controller) :
	DcForm(controller,  "Входы АЦП", false)
{
    auto dins = controller->signalManager().getSignals<DinSignal>(Signal::Subtype::Acp);
    auto model = new DinModel(dins, this);
    auto tableView = new Dpc::Gui::TableView(model, this);

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
    visibleColumns << DinModel::Column::Drebezg;
    visibleColumns << DinModel::Column::DrebezgTimer;
    visibleColumns << DinModel::Column::DrebezgNoise;
    visibleColumns << DinModel::Column::Inversion;
    visibleColumns << DinModel::Column::ThresholdChain;
    visibleColumns << DinModel::Column::Threshold0;
    visibleColumns << DinModel::Column::Threshold1;
    visibleColumns << DinModel::Column::ThresholdKZ;
    visibleColumns << DinModel::Column::DP;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Name, 200);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Oscill, 150);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Journal, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Archive, 70);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Drebezg, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DrebezgTimer, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DrebezgNoise, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Inversion, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::ThresholdChain, 120);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Threshold0, 100);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Threshold1, 100);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::ThresholdKZ, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DP, 50);

    tableView->verticalHeader()->hide();
    tableView->setItemDelegateForColumn(DinModel::Column::Drebezg, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::DrebezgTimer, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::DrebezgNoise, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::ThresholdChain, new Dpc::Gui::DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(DinModel::Column::Threshold0, new Dpc::Gui::DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(DinModel::Column::Threshold1, new Dpc::Gui::DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(DinModel::Column::ThresholdKZ, new Dpc::Gui::DoubleSpinBoxDelegate(0, 10000, this));

    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DinsAcpForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DIN_TABLEPARS},
		{SP_DIN_BREAK}
	};

	return hasAny(controller, params);
}

void DinsAcpForm::fillReport(DcIConfigReport * report)
{
//	report->insertSection();

//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	DcReportTable table(report->device(), headers, { 5, 25 });
//	for (auto &&it : getItems(report->device())) {
//		QStringList values;
//		for (size_t col = 0; col < it.data.size(); col++) {
//			if (col == DinModel::Column::OscillColumn || col == DinModel::Column::JournalColumn || col == DinModel::Column::ArhciveColumn ||
//				col == DinModel::Column::InversionColumn)
//				values << DcReportTable::checkedSign(it.data[col].toBool());
//			else
//				values << it.data[col].toString();
//		}

//		table.addRow(values);
//	}

//	report->insertTable(table);
}
