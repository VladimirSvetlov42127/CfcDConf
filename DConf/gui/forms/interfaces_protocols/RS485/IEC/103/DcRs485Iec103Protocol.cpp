#include "DcRs485Iec103Protocol.h"

namespace {
}

DcRs485Iec103Protocol::DcRs485Iec103Protocol(DcController *device, int portIdx) :
	DcRs485IecProtocol(device, portIdx)
{	
}

void DcRs485Iec103Protocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485IecProtocol::fillReport(report, main);
}

//=============================================================================
DcRs485Iec103ProtocolWidget::DcRs485Iec103ProtocolWidget(DcRs485Iec103Protocol *protocol, QWidget *parent) :
	DcRs485IecProtocolWidget(protocol, parent)
{
}
