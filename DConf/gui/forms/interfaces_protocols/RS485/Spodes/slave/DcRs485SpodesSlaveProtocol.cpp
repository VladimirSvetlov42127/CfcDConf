#include "DcRs485SpodesSlaveProtocol.h"

namespace {
}

namespace Text {
}

DcRs485SpodesSlaveProtocol::DcRs485SpodesSlaveProtocol(DcController *device, int portIdx) :
	DcRs485SpodesProtocol(device, portIdx)
{
	
}

void DcRs485SpodesSlaveProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485SpodesProtocol::fillReport(report, main);
}

//=============================================================================
DcRs485SpodesSlaveProtocolWidget::DcRs485SpodesSlaveProtocolWidget(DcRs485SpodesSlaveProtocol *protocol, QWidget *parent) :
	DcRs485SpodesProtocolWidget(protocol, parent)
{
	addressEditor()->setRange(1, 247);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addVerticalStretch();
}
