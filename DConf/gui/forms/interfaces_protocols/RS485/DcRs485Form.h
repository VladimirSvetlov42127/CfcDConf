#pragma once

#include <gui/forms/DcFormFactory.h>

class DcRs485Form : public DcForm
{
	Q_OBJECT

public:
	DcRs485Form(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DcRs485Form, DcMenu::rs485);

