#pragma once

#include <gui/forms/DcFormFactory.h>

class DcIEC103ChannelsForm : public DcForm
{
	Q_OBJECT

public:
	DcIEC103ChannelsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

};

REGISTER_FORM(DcIEC103ChannelsForm, DcMenu::iec103_channels);