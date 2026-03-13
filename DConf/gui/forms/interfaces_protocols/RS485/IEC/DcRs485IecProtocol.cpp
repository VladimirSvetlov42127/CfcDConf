#include "DcRs485IecProtocol.h"

namespace {
	const QList<int> g_Speed = { 2400, 4800, 9600, 19200, 38400, 56000, 64000, 115200 };
}

namespace Text {
	const QString Format = "8 бит + чет";
}

DcRs485IecProtocol::DcRs485IecProtocol(DcController *device, int portIdx) :
	DcRs485BaseProtocol(device, portIdx, g_Speed)
{	
}

void DcRs485IecProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485BaseProtocol::fillReport(report, main);

	main->addRow({ Rs485::Text::CharacterFormat, Text::Format });
}

//=============================================================================
DcRs485IecProtocolWidget::DcRs485IecProtocolWidget(DcRs485IecProtocol *protocol, QWidget *parent) :
	DcRs485BaseProtocolWidget(protocol, parent)
{
	addressEditor()->setRange(1, 254);
	inputBufferEditor()->setRange(32, 300);
	outputBufferEditor()->setRange(64, 1450);

	auto formatEdit = new QLineEdit(Text::Format);
	formatEdit->setEnabled(false);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(formatEdit, Rs485::Text::CharacterFormat);
}
