#pragma once

#include <gui/forms/DcFormFactory.h>

class AinsForm : public DcForm
{
	Q_OBJECT
public:
    AinsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(AinsForm, DcMenu::analogs_aperture);

