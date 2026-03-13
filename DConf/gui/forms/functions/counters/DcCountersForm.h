#pragma once

#include <gui/forms/DcFormFactory.h>

class DcCountersForm : public DcForm
{

public:
	DcCountersForm(DcController *controller);
	
	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
private:	
	QWidget *createDiscretsChangesTab();
};

REGISTER_FORM(DcCountersForm, DcMenu::counters_functions);