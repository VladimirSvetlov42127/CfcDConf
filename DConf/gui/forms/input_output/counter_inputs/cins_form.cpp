#include "cins_form.h"

#include <dpc/gui/widgets/TableView.h>

#include "gui/models/cin_model.h"

using namespace Dpc::Gui;

CinsForm::CinsForm(DcController* controller) :
    DcForm(controller,  "Счетчики", false)
{
    auto cins = controller->signalManager().getSignals<CinSignal>();
    auto model = new CinModel(cins, this);

    auto tableView = new Dpc::Gui::TableView(model, this);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);
    tableView->horizontalHeader()->setHighlightSections(false);

    QList<int> visibleColumns;
    visibleColumns << CinModel::Column::Number;
    visibleColumns << CinModel::Column::Name;
    tableView->setOnlyVisibleColumns(visibleColumns);
    tableView->horizontalHeader()->resizeSection(CinModel::Column::Number, 25);
    tableView->horizontalHeader()->resizeSection(CinModel::Column::Name, 300);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget());
    layout->addWidget(tableView);
}

bool CinsForm::isAvailableFor(DcController* controller)
{
    return controller->signalManager().cinSize();
}

void CinsForm::fillReport(DcIConfigReport* report)
{
}
