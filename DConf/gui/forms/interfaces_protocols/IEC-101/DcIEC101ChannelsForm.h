#pragma once

#include <gui/forms/DcFormFactory.h>
#include <data_model/dc_signal.h>

class DcIEC101ChannelsForm : public DcForm
{
	Q_OBJECT

public:
	DcIEC101ChannelsForm(DcController *controller);
	~DcIEC101ChannelsForm();

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

	QWidget* createChannelsTable(DefSignalType signalType);
};

REGISTER_FORM(DcIEC101ChannelsForm, DcMenu::iec101_104_channels);
