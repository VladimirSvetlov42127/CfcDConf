#pragma once

#include <gui/forms/DcFormFactory.h>

class DcTcpForm : public DcForm
{

public:
	DcTcpForm(DcController *controller);

	static bool isAvailableFor(DcController *controller);
	static void fillReport(DcIConfigReport *report);
	
private:	
	QWidget *createIpTab() const;
	QWidget *createReservationTab() const;
	QWidget *createProtectionTab() const;
	QWidget *createFirewallTab() const;
	QWidget *createSybuslTab() const;
	QWidget *createSntpTab() const;
	QWidget *createIec1041Tab() const;
	QWidget *createIec1042Tab() const;
	QWidget *createModbusTcpTab() const;
};

REGISTER_FORM(DcTcpForm, DcMenu::tcp);
