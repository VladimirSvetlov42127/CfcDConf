#include "spodes_channels_form.h"

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/din_model.h"
#include "gui/models/ain_model.h"
#include "gui/models/cin_model.h"

using namespace Dpc::Gui;

namespace {

TableView *makeTableView(DcController* controller, QAbstractTableModel* model, QWidget* parent)
{
    auto tableView = new TableView(model, parent);
    model->setParent(parent);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);
    tableView->horizontalHeader()->setHighlightSections(false);

    QObject::connect(tableView, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    QObject::connect(tableView, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

    return tableView;
}

} // namespace

namespace Text {
    const QMap< Signal::Type, QString> Titles = {
        {Signal::Type::Din, "Дискреты"},
        {Signal::Type::Ain, "Аналоги"},
        {Signal::Type::Cin, "Счётчики"}
	};
}

SpodesChannelsForm::SpodesChannelsForm(DcController *controller)
    : DcForm(controller,  "Поканальные настройки СПОДЭС", false)
    , m_tabWidget{new QTabWidget(this)}

{
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
    mainLayout->addWidget(m_tabWidget);

    auto dins = controller->signalManager().getSignals<DinSignal>();
    auto tableView = makeTableView(controller, new DinModel(dins), this);
    QList<int> visibleColumns;
    visibleColumns << DinModel::Column::Number;
    visibleColumns << DinModel::Column::Name;
    visibleColumns << DinModel::Column::SpodesSelect;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::SpodesSelect, 80);
    appendTableWidget(tableView, Text::Titles.value(Signal::Type::Din));

    auto ains = controller->signalManager().getSignals<AinSignal>();
    tableView = makeTableView(controller, new AinModel(ains), this);
    visibleColumns.clear();
    visibleColumns << AinModel::Column::Number;
    visibleColumns << AinModel::Column::Name;
    visibleColumns << AinModel::Column::SpodesSelect;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::SpodesSelect, 80);
    appendTableWidget(tableView, Text::Titles.value(Signal::Type::Ain));

    auto cins = controller->signalManager().getSignals<CinSignal>();
    tableView = makeTableView(controller, new CinModel(cins), this);
    visibleColumns.clear();
    visibleColumns << CinModel::Column::Number;
    visibleColumns << CinModel::Column::Name;
    visibleColumns << CinModel::Column::SpodesSelect;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(CinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(CinModel::Column::Name, 250);
    tableView->horizontalHeader()->resizeSection(CinModel::Column::SpodesSelect, 80);
    appendTableWidget(tableView, Text::Titles.value(Signal::Type::Cin));
}

bool SpodesChannelsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_SPODES_MASK}
	};

	return hasAny(controller, params);
}

void SpodesChannelsForm::fillReport(DcIConfigReport * report)
{
//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	for (auto it = Text::Titles.begin(); it != Text::Titles.end(); it++) {
//		report->insertSection(it.value());

//		DcReportTable table(report->device(), headers, { 5 });
//		for (auto &&it : getItems(report->device(), it.key())) {
//			QStringList values;
//			for (size_t col = 0; col < it.data.size(); col++) {
//				if (col == Columns::PollingColumn)
//					values << DcReportTable::checkedSign(it.data[col].toBool());
//				else
//					values << it.data[col].toString();
//			}

//			table.addRow(values);
//		}

//		report->insertTable(table);
    //	}
}

void SpodesChannelsForm::appendTableWidget(QWidget *tableWidget, const QString &title)
{
    auto decorateWidget = new QWidget;
    QVBoxLayout *decorateLayout = new QVBoxLayout(decorateWidget);
    decorateLayout->addWidget(tableWidget);
    m_tabWidget->addTab(decorateWidget, title);
}
