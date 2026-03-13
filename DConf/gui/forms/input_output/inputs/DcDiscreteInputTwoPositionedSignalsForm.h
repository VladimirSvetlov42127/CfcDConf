#pragma once

#include <gui/forms/DcFormFactory.h>

class DcDiscreteInputTwoPositionedSignalsForm : public DcForm
{
	Q_OBJECT

public:
	DcDiscreteInputTwoPositionedSignalsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DcDiscreteInputTwoPositionedSignalsForm, DcMenu::discret_2pos_channels);
