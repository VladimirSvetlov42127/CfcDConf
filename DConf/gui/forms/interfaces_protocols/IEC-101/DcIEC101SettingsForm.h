#pragma once

#include <gui/forms/DcFormFactory.h>

class DcIEC101SettingsForm : public DcForm
{

public:
	DcIEC101SettingsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
	QWidget *createProtocolSettingsTab() const;
	QWidget *createProtocolAssingmentsTab() const;

private:	
};

REGISTER_FORM(DcIEC101SettingsForm, DcMenu::iec101_104_settings);
