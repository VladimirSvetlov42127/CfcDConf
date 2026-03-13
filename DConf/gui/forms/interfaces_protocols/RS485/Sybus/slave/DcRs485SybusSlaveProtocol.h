#pragma once

#include <gui/forms/interfaces_protocols/RS485/Sybus/DcRs485SybusProtocol.h>

class DcRs485SybusSlaveProtocol : public DcRs485SybusProtocol
{
public:
	DcRs485SybusSlaveProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485SybusSlaveProtocolWidget : public DcRs485SybusProtocolWidget
{
	Q_OBJECT

public:
	DcRs485SybusSlaveProtocolWidget(DcRs485SybusSlaveProtocol *protocol, QWidget *parent = nullptr);
};


