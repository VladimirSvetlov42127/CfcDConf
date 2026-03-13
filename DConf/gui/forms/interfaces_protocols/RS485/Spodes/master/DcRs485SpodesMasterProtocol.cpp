#include "DcRs485SpodesMasterProtocol.h"

#include <qpushbutton.h>

#include <gui/forms/interfaces_protocols/RS485/Spodes/master/SpodesSlaveSettingsDialog.h>

namespace {
}

namespace Text {
	const QString TcpPort = "Порт TCP прозрачного канала";
	const QString Disabled = "Отключен";
}

DcRs485SpodesMasterProtocol::DcRs485SpodesMasterProtocol(DcController *device, int portIdx) :
	DcRs485SpodesProtocol(device, portIdx)
{
	
}

void DcRs485SpodesMasterProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485SpodesProtocol::fillReport(report, main);

	auto addressStationRowIndex = 4;
	QStringList& addressStationRow = (*main)[addressStationRowIndex];
	addressStationRow.first() = Text::TcpPort;
	auto portValue = addressStationRow.last().toUInt();
	if (!portValue)
		addressStationRow.last() = Text::Disabled;

	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount, Rs485::Text::SlavesCount));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval));
}

//=============================================================================
DcRs485SpodesMasterProtocolWidget::DcRs485SpodesMasterProtocolWidget(DcRs485SpodesMasterProtocol *protocol, QWidget *parent) :
	DcRs485SpodesProtocolWidget(protocol, parent)
{
	addressLabel()->setText(Text::TcpPort);
	addressEditor()->setSpecialValueText(Text::Disabled);

    auto slaveCountEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount));
	slaveCountEditor->setRange(0, device()->rs485SlavesMax() / device()->rs485PortsCount());

	auto slaveSettingsButton = new QPushButton("Настройка ведомых");
	slaveSettingsButton->setEnabled(slaveCountEditor->param()->value().toUInt());
	connect(slaveCountEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {slaveSettingsButton->setEnabled(value.toUInt()); });
	connect(slaveSettingsButton, &QPushButton::clicked, this, [=] {
		SpodesSlaveSettingsDialog dlg(portIdx(), device(), this);
		dlg.exec();
	});

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(slaveCountEditor, new QLabel(Rs485::Text::SlavesCount), slaveSettingsButton);
	mg.addLineEditor(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries, 0, 5);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse, 10, 2000);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval, 100);

	mg.addVerticalStretch();
}
