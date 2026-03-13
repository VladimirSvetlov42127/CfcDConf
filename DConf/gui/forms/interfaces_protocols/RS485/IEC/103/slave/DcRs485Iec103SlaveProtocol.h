#pragma once

#include <gui/forms/interfaces_protocols/RS485/IEC/103/DcRs485Iec103Protocol.h>

class DcRs485Iec103SlaveProtocol : public DcRs485Iec103Protocol
{
public:
	DcRs485Iec103SlaveProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485Iec103SlaveProtocolWidget : public DcRs485Iec103ProtocolWidget
{
	Q_OBJECT
public:
	DcRs485Iec103SlaveProtocolWidget(DcRs485Iec103SlaveProtocol *protocol, QWidget *parent = nullptr);
};