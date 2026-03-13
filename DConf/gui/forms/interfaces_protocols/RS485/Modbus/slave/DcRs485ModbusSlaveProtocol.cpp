#include "DcRs485ModbusSlaveProtocol.h"

namespace {
	const uint8_t TransparentBit = 14;
}

DcRs485ModbusSlaveProtocol::DcRs485ModbusSlaveProtocol(DcController *device, int portIdx) :
	DcRs485ModbusProtocol(device, portIdx)
{
	
}

void DcRs485ModbusSlaveProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485ModbusProtocol::fillReport(report, main);

    auto param = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
	main->addRow(DcReportTable::ParamRecord(param, Rs485::Text::TransparentMode, true, TransparentBit));
}

//=============================================================================
DcRs485ModbusSlaveProtocolWidget::DcRs485ModbusSlaveProtocolWidget(DcRs485ModbusSlaveProtocol *protocol, QWidget *parent) :
	DcRs485ModbusProtocolWidget(protocol, parent)
{
	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addCheckEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Rs485::Text::TransparentMode, TransparentBit);

	mg.addVerticalStretch();
}
