#include "counter_inputs_form.h"

#include "dpc/gui/widgets/TableView.h"

using namespace Dpc::Gui;

DcCounterForm::DcCounterForm(DcController* controller) :
    DcForm(controller,  "Счетчики", false)
{
    auto tableView = new TableView(new CounterModel(controller, this), this);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget());
    layout->addWidget(tableView);
}

bool DcCounterForm::isAvailableFor(DcController* controller)
{
    return true;
}

void DcCounterForm::fillReport(DcIConfigReport* report)
{

}
