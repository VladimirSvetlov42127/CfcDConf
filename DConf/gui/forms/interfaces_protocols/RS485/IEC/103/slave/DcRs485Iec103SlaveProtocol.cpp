#include "DcRs485Iec103SlaveProtocol.h"

namespace {
	const uint8_t TransparentBit = 14;
}

DcRs485Iec103SlaveProtocol::DcRs485Iec103SlaveProtocol(DcController *device, int portIdx) :
	DcRs485Iec103Protocol(device, portIdx)
{	
}

void DcRs485Iec103SlaveProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485Iec103Protocol::fillReport(report, main);

    auto param = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
	main->addRow(DcReportTable::ParamRecord(param, Rs485::Text::TransparentMode, true, TransparentBit));
}

//=============================================================================
DcRs485Iec103SlaveProtocolWidget::DcRs485Iec103SlaveProtocolWidget(DcRs485Iec103SlaveProtocol *protocol, QWidget *parent) :
	DcRs485Iec103ProtocolWidget(protocol, parent)
{
	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addCheckEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Rs485::Text::TransparentMode, TransparentBit);

	mg.addVerticalStretch();
}
