#pragma once

#include <gui/forms/DcFormFactory.h>

class DcSpodesSettingsForm : public DcForm
{

public:
	DcSpodesSettingsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DcSpodesSettingsForm, DcMenu::spodes_settings);
