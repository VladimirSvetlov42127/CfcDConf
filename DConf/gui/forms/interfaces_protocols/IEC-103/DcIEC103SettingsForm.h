#pragma once

#include <gui/forms/DcFormFactory.h>

class DcIEC103SettingsForm : public DcForm
{

public:
	DcIEC103SettingsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
};

REGISTER_FORM(DcIEC103SettingsForm, DcMenu::iec103_settings);
