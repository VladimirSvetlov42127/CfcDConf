#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/DcRs485ModbusProtocol.h>

class DcRs485ModbusSlaveProtocol : public DcRs485ModbusProtocol
{
public:
	DcRs485ModbusSlaveProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};

class DcRs485ModbusSlaveProtocolWidget : public DcRs485ModbusProtocolWidget
{
	Q_OBJECT

public:
	DcRs485ModbusSlaveProtocolWidget(DcRs485ModbusSlaveProtocol *protocol, QWidget *parent = nullptr);
};