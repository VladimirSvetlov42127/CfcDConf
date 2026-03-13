#pragma once

#include <gui/forms/interfaces_protocols/RS485/Spodes/DcRs485SpodesProtocol.h>

class DcRs485SpodesSlaveProtocol : public DcRs485SpodesProtocol
{
public:
	DcRs485SpodesSlaveProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485SpodesSlaveProtocolWidget : public DcRs485SpodesProtocolWidget
{
	Q_OBJECT

public:
	DcRs485SpodesSlaveProtocolWidget(DcRs485SpodesSlaveProtocol *protocol, QWidget *parent = nullptr);
};
