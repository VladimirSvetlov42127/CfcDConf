#include "DcRs485SpodesProtocol.h"

#include <qpushbutton.h>

#include <dpc/gui/widgets/SpinBox.h>

using namespace Dpc::Gui;

namespace {
	const QList<int> g_Speed = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };
	const uint MinBuffer = 128;
	const uint MaxBuffer = 512;
}

namespace Text {
	const QString TimeoutConnectionActivity = "Таймаут активности соединения (сек)";

	const QString TimeoutDisabled = "Отключен";
	const QString IntervalDefault = "По умолчанию";
}

DcRs485SpodesProtocol::DcRs485SpodesProtocol(DcController *device, int portIdx) :
	DcRs485BaseProtocol(device, portIdx, g_Speed)
{	
}

void DcRs485SpodesProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485BaseProtocol::fillReport(report, main);

    auto options = device()->paramsRegistry().element(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Options);
	auto timeoutValue = options->value().toUInt() & 0xFFFF;
	auto intervalValue = options->value().toUInt() >> 16;

	main->addRow({ Text::TimeoutConnectionActivity , timeoutValue ? QString::number(timeoutValue) : Text::TimeoutDisabled });
	main->addRow({ Rs485::Text::FrameInterval , intervalValue ? QString::number(intervalValue) : Text::IntervalDefault });
}

//=============================================================================
DcRs485SpodesProtocolWidget::DcRs485SpodesProtocolWidget(DcRs485SpodesProtocol *protocol, QWidget *parent) :
	DcRs485BaseProtocolWidget(protocol, parent)
{
	inputBufferEditor()->setRange(MinBuffer, MaxBuffer);
	outputBufferEditor()->setRange(MinBuffer, MaxBuffer);

    auto options = device()->paramsRegistry().element(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Options);
	auto timeoutConnectionSpinBox = new IntSpinBox;
	timeoutConnectionSpinBox->setRange(0, USHRT_MAX);
	timeoutConnectionSpinBox->setSpecialValueText(Text::TimeoutDisabled);
	timeoutConnectionSpinBox->setValue(options->value().toUInt() & 0xFFFF);

	auto frameIntervalSpinBox = new IntSpinBox;
	frameIntervalSpinBox->setRange(0, USHRT_MAX);
	frameIntervalSpinBox->setSpecialValueText(Text::IntervalDefault);
	frameIntervalSpinBox->setValue(options->value().toUInt() >> 16);

	auto onChanged = [=]() {
		uint32_t val = (frameIntervalSpinBox->value() << 16) | timeoutConnectionSpinBox->value();
		options->updateValue(QString::number(val));
	};
	connect(timeoutConnectionSpinBox, qOverload<int>(&IntSpinBox::valueChanged), this, onChanged);
	connect(frameIntervalSpinBox, qOverload<int>(&IntSpinBox::valueChanged), this, onChanged);

	auto timeoutDisableButton = new QPushButton("Не отрабатывать");
	connect(timeoutDisableButton, &QPushButton::clicked, this, [=] { timeoutConnectionSpinBox->setValue(0); });

	auto intervalDefaultButton = new QPushButton(Text::IntervalDefault);
	connect(intervalDefaultButton, &QPushButton::clicked, this, [=] { frameIntervalSpinBox->setValue(0); });

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(timeoutConnectionSpinBox, new QLabel(Text::TimeoutConnectionActivity), timeoutDisableButton);
	mg.addWidget(frameIntervalSpinBox, new QLabel(Rs485::Text::FrameInterval), intervalDefaultButton);
}
