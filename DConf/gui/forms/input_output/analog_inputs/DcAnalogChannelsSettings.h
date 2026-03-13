#pragma once

#include <gui/forms/DcFormFactory.h>

class DcAnalogChannelsSettings : public DcForm
{
	Q_OBJECT

public:
	DcAnalogChannelsSettings(DcController *controller);
	~DcAnalogChannelsSettings();

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
};

REGISTER_FORM(DcAnalogChannelsSettings, DcMenu::analogs_aperture);

