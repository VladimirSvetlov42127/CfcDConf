#include "DcRs485Iec101Protocol.h"

namespace Text {
}

DcRs485Iec101Protocol::DcRs485Iec101Protocol(DcController *device, int portIdx) :
	DcRs485IecProtocol(device, portIdx)
{
}

void DcRs485Iec101Protocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485IecProtocol::fillReport(report, main);
}

//=============================================================================
DcRs485Iec101ProtocolWidget::DcRs485Iec101ProtocolWidget(DcRs485Iec101Protocol *protocol, QWidget *parent) :
	DcRs485IecProtocolWidget(protocol, parent)
{
}
