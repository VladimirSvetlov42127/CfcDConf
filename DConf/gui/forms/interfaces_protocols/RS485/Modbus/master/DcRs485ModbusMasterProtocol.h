#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/DcRs485ModbusProtocol.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusWidgetSlaveSettings.h>

class DcRs485ModbusMasterProtocol : public DcRs485ModbusProtocol
{
public:
	DcRs485ModbusMasterProtocol(DcController *device, int portIdx);

	virtual void fillReport(DcIConfigReport *report, DcReportTable *main) override;
};


class DcRs485ModbusMasterProtocolWidget : public DcRs485ModbusProtocolWidget
{
	Q_OBJECT

public:
	DcRs485ModbusMasterProtocolWidget(DcRs485ModbusMasterProtocol *protocol, QWidget *parent = nullptr);
	~DcRs485ModbusMasterProtocolWidget();

private slots:
	void onSlaveSettings();

private:
	ModbusWidgetSlaveSettings *m_slaveWidget;
	ModbusSlaveFileSaver m_SlaveLoader;
};