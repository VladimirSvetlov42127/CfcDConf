#pragma once

#include <gui/forms/DcFormFactory.h>

class DcOscillographyForm : public DcForm
{
	Q_OBJECT

public:
	DcOscillographyForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
private:
    QWidget *createCommonTab();
    QWidget *createAnalogTab();
};

REGISTER_FORM(DcOscillographyForm, DcMenu::oscillography);
