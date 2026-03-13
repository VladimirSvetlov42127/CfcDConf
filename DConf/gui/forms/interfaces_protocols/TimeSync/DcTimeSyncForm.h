#pragma once

#include <gui/forms/DcFormFactory.h>

class DcTimeSyncForm : public DcForm
{
	Q_OBJECT
public:
	DcTimeSyncForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);

private:
	QWidget *createTimeSourceTab();

	QWidget *createIEEE1588v2_UDP();
	QWidget *createIEEE1588v2_802();
	QWidget *createSNTPv4();
	QWidget *create1PPS();
	QWidget *createDefault();

	bool m_hasGlobalOffsetParam;
};

REGISTER_FORM(DcTimeSyncForm, DcMenu::time_sync);