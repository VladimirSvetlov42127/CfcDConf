#pragma once

#include <gui/forms/DcFormFactory.h>

class DoutsForm : public DcForm
{
	Q_OBJECT

public:
    DoutsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DoutsForm, DcMenu::discret_output_channels);
