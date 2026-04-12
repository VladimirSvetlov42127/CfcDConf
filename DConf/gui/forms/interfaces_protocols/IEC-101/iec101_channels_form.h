#pragma once

#include "gui/forms/DcFormFactory.h"

class QTabWidget;

class Iec101ChannelsForm : public DcForm
{
	Q_OBJECT

public:
    Iec101ChannelsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

    QWidget* createTableView(Signal::Type type);

private:
    QTabWidget* m_tabWidget;
};

REGISTER_FORM(Iec101ChannelsForm, DcMenu::iec101_104_channels);
