#pragma once

#include <gui/forms/DcFormFactory.h>

class QTabWidget;

class SpodesChannelsForm : public DcForm
{
	Q_OBJECT
public:
    SpodesChannelsForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
    void appendTableWidget(QWidget* tableWidget, const QString& title);

private:
    QTabWidget* m_tabWidget;
};

REGISTER_FORM(SpodesChannelsForm, DcMenu::spodes_channels);
