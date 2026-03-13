#pragma once

#include <gui/forms/DcFormFactory.h>

class DcSettingsForm : public DcForm
{

public:
	DcSettingsForm(DcController *controller);
	
	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
private:	
	QWidget *createCommonTab();
	QWidget *createFactorsTab();
	QWidget *createConnectionTab();
	QWidget *createDisplayTab();
	QWidget *createKeyboardTab();
	QWidget *createObjectTab();
	QWidget* createSetpointsGroupsTab();
};

REGISTER_FORM(DcSettingsForm, DcMenu::general_settings);