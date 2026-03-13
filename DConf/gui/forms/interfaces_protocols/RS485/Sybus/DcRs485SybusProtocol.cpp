#include "DcRs485SybusProtocol.h"

namespace Text {
	const QString PacketMethod = "Метод выделения пакета";

	namespace Description {
		const QString Format9Bit = "Режим Sybus";
		const QString Cobs = "Режим Sybus Cobs";
	}
}

namespace {
	const QList<int> g_Speed = { 9600, 19200, 38400, 153600, 307200 };

	enum {
		Format9Bit,
		FormatCobs
	};

	const uint16_t CobsMask = 0x8000;

	const ListEditorContainer g_Formats = { {"9 бит", Format9Bit}, {"Модифицированный байт стаффинг", FormatCobs} };
}

DcRs485SybusProtocol::DcRs485SybusProtocol(DcController *device, int portIdx) :
	DcRs485BaseProtocol(device, portIdx, g_Speed)
{
}

void DcRs485SybusProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485BaseProtocol::fillReport(report, main);

	main->addRow({ Text::PacketMethod, g_Formats.text(QVariant(currentFormat())) });
}

int DcRs485SybusProtocol::currentFormat() const
{
    auto optionsParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);

	int currentFormatValue = -1;
	if ((optionsParam->value().toUInt() & CobsMask) &&
		(parityParam->value().toUInt() == Rs485::Parity::Odd) &&
		(lengthParam->value().toUInt() == Rs485::Format::F8Bit))
		currentFormatValue = FormatCobs;
	else if (~(optionsParam->value().toUInt() & CobsMask) &&
		(parityParam->value().toUInt() == Rs485::Parity::No) &&
		(lengthParam->value().toUInt() == Rs485::Format::F9Bit))
		currentFormatValue = Format9Bit;

	return currentFormatValue;
}

//=============================================================================
DcRs485SybusProtocolWidget::DcRs485SybusProtocolWidget(DcRs485SybusProtocol *protocol, QWidget *parent) :
	DcRs485BaseProtocolWidget(protocol, parent)
{
	addressEditor()->setRange(1, 253);
	inputBufferEditor()->setRange(100, 300);
	outputBufferEditor()->setRange(100, 1450);

    auto optionsParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);

	int currentFormatValue = protocol->currentFormat();
	auto descLabel = new QLabel;
	auto setDescription = [=](int val) {
		switch (val)
		{
		case Format9Bit: descLabel->setText(Text::Description::Format9Bit); break;
		case FormatCobs: descLabel->setText(Text::Description::Cobs); break;
		default: descLabel->setText(QString());	break;
		}
	};
	setDescription(currentFormatValue);

	auto editor = new ListParamEditor(g_Formats);
	editor->setValue(QString::number(currentFormatValue));

	auto onChanged = [=](const QString &value) {
		auto val = value.toUInt();
		setDescription(val);
		auto optionsValue = val == Format9Bit ? optionsParam->value().toUInt() & (~CobsMask) : optionsParam->value().toUInt() | CobsMask;
		int parityValue = val == Format9Bit ? Rs485::Parity::No : Rs485::Parity::Odd;
		int lengthValue = val == Format9Bit ? Rs485::Format::F9Bit : Rs485::Format::F8Bit;
		optionsParam->updateValue(QString::number(optionsValue));
		parityParam->updateValue(QString::number(parityValue));
		lengthParam->updateValue(QString::number(lengthValue));
	};
	connect(editor, &ParamEditor::valueChanged, this, onChanged);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(editor, new QLabel(Text::PacketMethod), descLabel);
}
