#pragma once

#include <gui/forms/DcFormFactory.h>

class DcPqiEventSettingsForm : public DcForm
{
	Q_OBJECT
public:
	DcPqiEventSettingsForm(DcController* controller);
	~DcPqiEventSettingsForm();

    static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
};

REGISTER_FORM(DcPqiEventSettingsForm, DcMenu::pqi_event_settings);
