#include "iec101_channels_form.h"

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/din_model.h"
#include "gui/models/ain_model.h"
#include "gui/models/cin_model.h"

using namespace Dpc::Gui;

namespace {

const QMap<Signal::Type, QString> Tables = {
    {Signal::Type::Din, "Дискреты"},
    {Signal::Type::Ain, "Аналоги"},
    {Signal::Type::Cin, "Счётчики"}
};

TableView *makeTableView(DcController* controller, QAbstractTableModel* model, const QList<int>& columns)
{
    auto tableView = new TableView(model);
    model->setParent(tableView);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->setOnlyVisibleColumns(columns);
    tableView->horizontalHeader()->resizeSection(columns.value(0), 25);
    tableView->horizontalHeader()->resizeSection(columns.value(1), 250);
    tableView->horizontalHeader()->resizeSection(columns.value(2), 80);
    tableView->horizontalHeader()->resizeSection(columns.value(3), 90);
    tableView->horizontalHeader()->resizeSection(columns.value(4), 90);
    tableView->horizontalHeader()->resizeSection(columns.value(5), 160);

    QObject::connect(tableView, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    QObject::connect(tableView, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

    return tableView;
}

} // namespace

Iec101ChannelsForm::Iec101ChannelsForm(DcController *controller)
    : DcForm(controller,  "Поканальные настройки МЭК 60870-5-101/104", false)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
    QTabWidget *tabWidget = new QTabWidget;
    mainLayout->addWidget(tabWidget);

    for(auto tableIt = Tables.begin(); tableIt != Tables.end(); ++tableIt)
        tabWidget->addTab(createTableView(tableIt.key()), tableIt.value());
}

bool Iec101ChannelsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_SELECT_MASK},
		{SP_GROUP1LIST},
		{SP_GROUP2LIST},
		{SP_BACKGROUNDLIST}
	};

	return hasAny(controller, params);
}

void Iec101ChannelsForm::fillReport(DcIConfigReport * report)
{
//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	for (auto it = Text::Titles.begin(); it != Text::Titles.end(); it++) {
//		report->insertSection(it.value());

//		DcReportTable table(report->device(), headers, { 5, 20 });
//		for (auto &&it : getItems(report->device(), it.key())) {
//			QStringList values;
//			for (size_t col = 0; col < it.data.size(); col++) {
//				if (col == Columns::PollingColumn || col == Columns::Group1Column || col == Columns::Group2Column || col == Columns::BackgroundColumn)
//					values << DcReportTable::checkedSign(it.data[col].toBool());
//				else
//					values << it.data[col].toString();
//			}

//			table.addRow(values);
//		}

//		report->insertTable(table);
    //	}
}

QWidget *Iec101ChannelsForm::createTableView(Signal::Type type)
{
    TableView* tableView = nullptr;
    if (Signal::Type::Din == type) {
        QList<int> visibleColumns;
        visibleColumns << DinModel::Column::Number;
        visibleColumns << DinModel::Column::Name;
        visibleColumns << DinModel::Column::IEC101Select;
        visibleColumns << DinModel::Column::IEC101Group1;
        visibleColumns << DinModel::Column::IEC101Group2;
        visibleColumns << DinModel::Column::IEC101Background;
        auto dins = controller()->signalManager().getSignals<DinSignal>();
        tableView = makeTableView(controller(), new DinModel(dins), visibleColumns);
    }
    else if (Signal::Type::Ain == type) {
        QList<int> visibleColumns;
        visibleColumns << AinModel::Column::Number;
        visibleColumns << AinModel::Column::Name;
        visibleColumns << AinModel::Column::IEC101Select;
        visibleColumns << AinModel::Column::IEC101Group1;
        visibleColumns << AinModel::Column::IEC101Group2;
        visibleColumns << AinModel::Column::IEC101Background;
        auto ains = controller()->signalManager().getSignals<AinSignal>();
        tableView = makeTableView(controller(), new AinModel(ains), visibleColumns);

    }
    else if (Signal::Type::Cin == type) {
        QList<int> visibleColumns;
        visibleColumns.clear();
        visibleColumns << CinModel::Column::Number;
        visibleColumns << CinModel::Column::Name;
        visibleColumns << CinModel::Column::IEC101Select;
        visibleColumns << CinModel::Column::IEC101Group1;
        visibleColumns << CinModel::Column::IEC101Group2;
        visibleColumns << CinModel::Column::IEC101Background;
        auto cins = controller()->signalManager().getSignals<CinSignal>();
        tableView = makeTableView(controller(), new CinModel(cins), visibleColumns);
    }

    auto decorateWidget = new QWidget;
    QVBoxLayout *decorateLayout = new QVBoxLayout(decorateWidget);
    decorateLayout->addWidget(tableView);
    return decorateWidget;
}
