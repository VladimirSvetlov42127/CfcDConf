#pragma once

#include <gui/forms/DcFormFactory.h>

class DcSwitchesForm : public DcForm
{
	Q_OBJECT

public:
	DcSwitchesForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
private:
	QWidget *createTab(int idx) const;
	QWidget* createTUBlockTab() const;

};

REGISTER_FORM(DcSwitchesForm, DcMenu::switches);