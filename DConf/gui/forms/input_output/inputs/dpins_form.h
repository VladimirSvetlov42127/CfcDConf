#pragma once

#include <gui/forms/DcFormFactory.h>

class DPinSignalsForm : public DcForm
{
	Q_OBJECT

public:
    DPinSignalsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DPinSignalsForm, DcMenu::discret_2pos_channels);
