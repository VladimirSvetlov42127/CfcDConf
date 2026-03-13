#pragma once

#include <gui/forms/DcFormFactory.h>

class QLabel;

class DcDiscreteOutputBlockControl : public DcForm
{
	Q_OBJECT

public:
	DcDiscreteOutputBlockControl(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

};

REGISTER_FORM(DcDiscreteOutputBlockControl, DcMenu::block_control);
