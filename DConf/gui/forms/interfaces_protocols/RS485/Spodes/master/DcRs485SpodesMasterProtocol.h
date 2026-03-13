#pragma once

#include <gui/forms/interfaces_protocols/RS485/Spodes/DcRs485SpodesProtocol.h>

class DcRs485SpodesMasterProtocol : public DcRs485SpodesProtocol
{
public:
	DcRs485SpodesMasterProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485SpodesMasterProtocolWidget : public DcRs485SpodesProtocolWidget
{
	Q_OBJECT

public:
	DcRs485SpodesMasterProtocolWidget(DcRs485SpodesMasterProtocol *protocol, QWidget *parent = nullptr);
};
