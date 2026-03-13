#pragma once

#include <gui/forms/DcFormFactory.h>

class DcThresholdsForm : public DcForm
{
	Q_OBJECT
public:
	DcThresholdsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
};

REGISTER_FORM(DcThresholdsForm, DcMenu::thresholds);
