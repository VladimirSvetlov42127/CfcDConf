#include "iec103_channels_form.h"

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/din_model.h"

using namespace Dpc::Gui;

Iec103ChannelsForm::Iec103ChannelsForm(DcController *controller) :
	DcForm(controller,  "Поканальные настройки МЭК 60870-5-103", false)
{
    auto dins = controller->signalManager().getSignals<DinSignal>();
    auto model = new DinModel(dins, this);
    auto tableView = new TableView(model, this);

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
    visibleColumns << DinModel::Column::IEC103Select;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::IEC103Select, 80);

    connect(tableView, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
	QTabWidget *tabWidget = new QTabWidget;
	mainLayout->addWidget(tabWidget);

	QWidget *discretTab = new QWidget;
	QVBoxLayout *discretTabLayout = new QVBoxLayout(discretTab);
	discretTabLayout->addWidget(tableView);

	tabWidget->addTab(discretTab, "Дискретные сигналы");
}

bool Iec103ChannelsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_IEC_103_MASK_DINS}
	};

	return hasAny(controller, params);
}

void Iec103ChannelsForm::fillReport(DcIConfigReport * report)
{
//	report->insertSection();

//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	DcReportTable table(report->device(), headers, { 5 });
//	for (auto &&it : getItems(report->device())) {
//		QStringList values;
//		for (size_t col = 0; col < it.data.size(); col++) {
//			if (col == Columns::PollingColumn)
//				values << DcReportTable::checkedSign(it.data[col].toBool());
//			else
//				values << it.data[col].toString();
//		}

//		table.addRow(values);
//	}

//	report->insertTable(table);
}
