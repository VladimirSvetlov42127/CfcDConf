#pragma once

#include <gui/forms/DcFormFactory.h>

class DcDiscreteInputACDSignalForm : public DcForm
{
	Q_OBJECT
public:
	DcDiscreteInputACDSignalForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DcDiscreteInputACDSignalForm, DcMenu::discret_input_adc);
