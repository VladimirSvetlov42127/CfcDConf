#pragma once

#include <gui/forms/DcFormFactory.h>

#include <data_model/dc_signal.h>

class DcSpodesChannelsForm : public DcForm
{
	Q_OBJECT
public:
	DcSpodesChannelsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
	QWidget* createChannelsTable(DefSignalType signalType);
};

REGISTER_FORM(DcSpodesChannelsForm, DcMenu::spodes_channels);
