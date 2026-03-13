#pragma once

#include <qwidget.h>

class DcController;
class DcIConfigReport;

class DcRs485PortWidget : public QWidget
{
	Q_OBJECT

public:
	DcRs485PortWidget(DcController *controller, int portIdx, QWidget *parent = nullptr);

	static void fillReport(DcIConfigReport *report, int portIdx);

private:
};