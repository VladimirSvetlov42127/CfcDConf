#pragma once

#include <gui/forms/interfaces_protocols/RS485/DcRs485BaseProtocol.h>

class DcRs485SpodesProtocol : public DcRs485BaseProtocol
{
public:
	DcRs485SpodesProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485SpodesProtocolWidget : public DcRs485BaseProtocolWidget
{
	Q_OBJECT
public:
	DcRs485SpodesProtocolWidget(DcRs485SpodesProtocol *protocol, QWidget *parent = nullptr);
};