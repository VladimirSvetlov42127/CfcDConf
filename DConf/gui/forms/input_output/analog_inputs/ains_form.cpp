#include "ains_form.h"

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include "gui/models/ain_model.h"

using namespace Dpc::Gui;

namespace {
	ComboBoxDelegate g_ApertureTypeDelegate = { "Абсолютный", "Относительный" };
}

AinsForm::AinsForm(DcController *controller) :
	DcForm(controller,  "Апертуры и пороги аналоговых входов", false)
{
    auto ains = controller->signalManager().getSignals<AinSignal>();
    auto model = new AinModel(ains, this);

    auto tableView = new Dpc::Gui::TableView(model, this);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);
    tableView->horizontalHeader()->setHighlightSections(false);

    QList<int> visibleColumns;
    visibleColumns << AinModel::Column::Number;
    visibleColumns << AinModel::Column::Name;
    visibleColumns << AinModel::Column::ApertureType;
    visibleColumns << AinModel::Column::ApertureVal;
    visibleColumns << AinModel::Column::ThresholdSensibility;
    visibleColumns << AinModel::Column::Threshold0;
    visibleColumns << AinModel::Column::Threshold1;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::ApertureType, 120);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::ApertureVal, 120);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::ThresholdSensibility, 140);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::Threshold0, 140);
    tableView->horizontalHeader()->resizeSection(AinModel::Column::Threshold1, 140);

    tableView->setItemDelegateForColumn(AinModel::Column::ApertureType, &g_ApertureTypeDelegate);
    tableView->setItemDelegateForColumn(AinModel::Column::ApertureVal, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(AinModel::Column::ThresholdSensibility, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(AinModel::Column::Threshold0, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(AinModel::Column::Threshold1, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool AinsForm::isAvailableFor(DcController * controller)
{
    return controller->signalManager().ainSize();
}

void AinsForm::fillReport(DcIConfigReport * report)
{
//    report->insertSection();

//    QStringList headers;
//    for (size_t i = 0; i < ColumnsCount; i++)
//        headers << columnName(Columns(i));

//    DcReportTable table(report->device(), headers, { 5, 30 });
//    for (auto &&it : getItems(report->device())) {
//        QStringList values;
//        for (size_t col = 0; col < it.data.size(); col++) {
//            if (AinModel::Column::ApertureType == col)
//                values << g_ApertureTypeDelegate.textFor(it.data[col].toUInt()).toString();
//            else
//                values << it.data[col].toString();
//        }

//        table.addRow(values);
//    }

//    report->insertTable(table);
}
