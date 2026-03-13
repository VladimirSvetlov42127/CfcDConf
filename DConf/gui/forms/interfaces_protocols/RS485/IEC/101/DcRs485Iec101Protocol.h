#pragma once

#include <gui/forms/interfaces_protocols/RS485/IEC/DcRs485IecProtocol.h>

class DcRs485Iec101Protocol : public DcRs485IecProtocol
{
public:
	DcRs485Iec101Protocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485Iec101ProtocolWidget : public DcRs485IecProtocolWidget
{
	Q_OBJECT
public:
	DcRs485Iec101ProtocolWidget(DcRs485Iec101Protocol *protocol, QWidget *parent = nullptr);
};