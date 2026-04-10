#include "dins_physical_form.h"

#include <QTableWidget>
#include <QLabel>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

#include "gui/models/din_model.h"
#include "service_manager/signals/din_physical_signal.h"

#include "gui/editors/EditorsManager.h"

namespace {
    const QString widget_style = "QGroupBox {border: 2px solid darkgrey; border-radius: 4px; background-color: %1;}";
    const ListEditorContainer amperage_list = { "DC", "AC" };
}

DinsPhysicalForm::DinsPhysicalForm(DcController* controller)
    : DcForm(controller,  "Настройки физических дискретных входов", false)
{
    QVBoxLayout* layout = new QVBoxLayout(centralWidget());
    if (!controller->boards().size()) {
        layout->addWidget(makeTableView(controller));
        return;
    }

    QTableWidget* table_widget = new QTableWidget(this);
    table_widget->setColumnCount(1);
    table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table_widget->horizontalHeader()->hide();
    table_widget->verticalHeader()->hide();
    layout->addWidget(table_widget);

    size_t boardWidgetCount = 0;
    for (auto &board: controller->boards()) {
        if (!board->DinsCount())
            continue;

        //	Формирование виджета
        QGroupBox* boardWidget = new QGroupBox;
        QString color = palette().color(QPalette::Window).name();
        boardWidget->setStyleSheet(widget_style.arg(color));

        QVBoxLayout* main_layout = new QVBoxLayout(boardWidget);

        //	Заголовок формы
        auto boardTitle = board->type();
        if (board->ToBoard())
            boardTitle.append(QString(" (%1)").arg(board->slot()));
        QLabel* title_label = new QLabel(boardTitle);
        QFont font = title_label->font();
        font.setBold(true);
        title_label->setFont(font);
        main_layout->addWidget(title_label, 0, Qt::AlignCenter);

        auto param = board->paramsRegistry().element(SP_DIN_ALG, 0);
        if (param) {
            QGridLayout* grid_layout = new QGridLayout;
            EditorsManager* editors = new EditorsManager(controller, grid_layout, this);
            editors->addListEditor(param, "Оперативный ток", amperage_list);
            editors->addStretch();
            main_layout->addLayout(grid_layout);
        }

        auto tableView = makeTableView(controller, board.get());
        tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        main_layout->addWidget(tableView);

        table_widget->insertRow(boardWidgetCount);
        table_widget->setCellWidget(boardWidgetCount, 0, boardWidget);
        ++boardWidgetCount;
    }
}

bool DinsPhysicalForm::isAvailableFor(DcController* controller)
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
		{SP_DIN_DPSREPRESENT} };

	return hasAny(controller, params);
}

void DinsPhysicalForm::fillReport(DcIConfigReport* report)
{
}

Dpc::Gui::TableView *DinsPhysicalForm::makeTableView(DcController *controller, DcBoard *board)
{
    std::vector<DinSignal*> boardPhysicalDins;
    for(auto din: controller->signalManager().getSignals<DinPhysicalSignal>())
        if (din->board() == board)
            boardPhysicalDins.emplace_back(din);

    auto  model = new DinModel(boardPhysicalDins, this);
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
    visibleColumns << DinModel::Column::DrebezgRatio;
    visibleColumns << DinModel::Column::DrebezgTimer;
    visibleColumns << DinModel::Column::DrebezgNoise;
    visibleColumns << DinModel::Column::Inversion;
    visibleColumns << DinModel::Column::DP;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Name, 200);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Oscill, 150);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Journal, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Archive, 70);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Drebezg, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DrebezgRatio, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DrebezgTimer, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DrebezgNoise, 160);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::Inversion, 80);
    tableView->horizontalHeader()->resizeSection(DinModel::Column::DP, 50);            

    tableView->setItemDelegateForColumn(DinModel::Column::Drebezg, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::DrebezgRatio, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::DrebezgTimer, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(DinModel::Column::DrebezgNoise, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));

    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

    return tableView;
}
