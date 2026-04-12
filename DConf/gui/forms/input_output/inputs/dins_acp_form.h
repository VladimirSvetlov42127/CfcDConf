#pragma once

#include <gui/forms/DcFormFactory.h>

class DinsAcpForm : public DcForm
{
	Q_OBJECT
public:
    DinsAcpForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DinsAcpForm, DcMenu::discret_input_adc);
