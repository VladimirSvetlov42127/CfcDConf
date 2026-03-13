#pragma once

#include <gui/forms/interfaces_protocols/RS485/Sybus/DcRs485SybusProtocol.h>

class DcRs485SybusMasterProtocol : public DcRs485SybusProtocol
{
public:
	DcRs485SybusMasterProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485SybusMasterProtocolWidget : public DcRs485SybusProtocolWidget
{
	Q_OBJECT

public:
	DcRs485SybusMasterProtocolWidget(DcRs485SybusMasterProtocol *protocol, QWidget *parent = nullptr);
};