#pragma once

#include <gui/forms/DcFormFactory.h>

class DcAuvUrovForm : public DcForm
{
	Q_OBJECT
public:
	DcAuvUrovForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:

};

REGISTER_FORM(DcAuvUrovForm, DcMenu::auv_urov);