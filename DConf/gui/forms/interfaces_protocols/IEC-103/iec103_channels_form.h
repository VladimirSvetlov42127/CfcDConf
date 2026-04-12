#pragma once

#include <gui/forms/DcFormFactory.h>

class Iec103ChannelsForm : public DcForm
{
	Q_OBJECT

public:
    Iec103ChannelsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

};

REGISTER_FORM(Iec103ChannelsForm, DcMenu::iec103_channels);
