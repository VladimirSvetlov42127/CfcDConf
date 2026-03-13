#pragma once

#include <gui/forms/interfaces_protocols/RS485/DcRs485BaseProtocol.h>

class DcRs485ModbusProtocol : public DcRs485BaseProtocol
{
public:
	DcRs485ModbusProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
	int currentFormat() const;
};


class DcRs485ModbusProtocolWidget : public DcRs485BaseProtocolWidget
{
	Q_OBJECT
public:
	DcRs485ModbusProtocolWidget(DcRs485ModbusProtocol *protocol, QWidget *parent = nullptr);
};