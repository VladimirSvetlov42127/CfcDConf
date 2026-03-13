#ifndef COUNTER_INPUTS_FORM_H
#define COUNTER_INPUTS_FORM_H

#include "counter_model.h"
#include "gui/forms/DcFormFactory.h"

class DcCounterForm : public DcForm
{
    Q_OBJECT

public:
    DcCounterForm(DcController* controller);

    static bool isAvailableFor(DcController* controller);
    static void fillReport(DcIConfigReport* report);
};

REGISTER_FORM(DcCounterForm, DcMenu::counter_inputs);

#endif // COUNTER_INPUTS_FORM_H
