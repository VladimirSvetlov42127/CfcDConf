#include "DcRs485SybusSlaveProtocol.h"

namespace {
	const uint8_t TransparentBit = 14;
}

DcRs485SybusSlaveProtocol::DcRs485SybusSlaveProtocol(DcController *device, int portIdx) :
	DcRs485SybusProtocol(device, portIdx)
{
}

void DcRs485SybusSlaveProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485SybusProtocol::fillReport(report, main);

    auto param = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
	main->addRow(DcReportTable::ParamRecord(param, Rs485::Text::TransparentMode, true, TransparentBit));
}

//=============================================================================
DcRs485SybusSlaveProtocolWidget::DcRs485SybusSlaveProtocolWidget(DcRs485SybusSlaveProtocol *protocol, QWidget *parent) :
	DcRs485SybusProtocolWidget(protocol, parent)
{
	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addCheckEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Rs485::Text::TransparentMode, TransparentBit);

	mg.addVerticalStretch();
}
