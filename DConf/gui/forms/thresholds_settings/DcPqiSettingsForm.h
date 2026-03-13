#pragma once

#include <gui/forms/DcFormFactory.h>

class DcPqiSettingsForm : public DcForm
{
	Q_OBJECT
public:
	DcPqiSettingsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
};

REGISTER_FORM(DcPqiSettingsForm, DcMenu::pqi_settings);
