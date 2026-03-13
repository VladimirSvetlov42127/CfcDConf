#pragma once

#include <gui/forms/DcFormFactory.h>

class DcDiscreteOutputChannelForm : public DcForm
{
	Q_OBJECT

public:
	DcDiscreteOutputChannelForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

};

REGISTER_FORM(DcDiscreteOutputChannelForm, DcMenu::discret_output_channels);
