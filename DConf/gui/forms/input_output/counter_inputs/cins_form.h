#ifndef CINS_FORM_H
#define CINS_FORM_H

#include "gui/forms/DcFormFactory.h"

class CinsForm : public DcForm
{
    Q_OBJECT

public:
    CinsForm(DcController* controller);

    static bool isAvailableFor(DcController* controller);
    static void fillReport(DcIConfigReport* report);
};

REGISTER_FORM(CinsForm, DcMenu::counter_inputs);

#endif // CINS_FORM_H
