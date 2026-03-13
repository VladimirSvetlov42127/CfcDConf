#include "DcRs485ModbusProtocol.h"

namespace {
	const QList<int> g_Speed = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 };

	enum {
		Format8Bit_No,
		Format8Bit_Odd,
		Format8Bit_Even
	};

	enum {
		Stop1Bit = 1
	};

	const ListEditorContainer g_Formats = { 
		{"8 бит без паритета", Format8Bit_No},
		{"8 бит + нечет", Format8Bit_Odd},
		{"8 бит + чет", Format8Bit_Even}
	};
}

namespace Text {
}

DcRs485ModbusProtocol::DcRs485ModbusProtocol(DcController *device, int portIdx) :
	DcRs485BaseProtocol(device, portIdx, g_Speed)
{	
}

void DcRs485ModbusProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485BaseProtocol::fillReport(report, main);

	main->addRow({ Rs485::Text::CharacterFormat, g_Formats.text(QVariant(currentFormat())) });
}

int DcRs485ModbusProtocol::currentFormat() const
{
    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);
    auto stopBitsParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatStopBits);

	int currentFormatValue = -1;
	if (stopBitsParam->value().toUInt() == Stop1Bit) {
		if ((parityParam->value().toUInt() == Rs485::Parity::No) && (lengthParam->value().toUInt() == Rs485::Format::F8Bit))
			currentFormatValue = Format8Bit_No;
		else if ((parityParam->value().toUInt() == Rs485::Parity::Odd) && (lengthParam->value().toUInt() == Rs485::Format::F9Bit))
			currentFormatValue = Format8Bit_Odd;
		else if ((parityParam->value().toUInt() == Rs485::Parity::Even) && (lengthParam->value().toUInt() == Rs485::Format::F9Bit))
			currentFormatValue = Format8Bit_Even;
	}

	return currentFormatValue;
}

//=============================================================================
DcRs485ModbusProtocolWidget::DcRs485ModbusProtocolWidget(DcRs485ModbusProtocol *protocol, QWidget *parent) :
	DcRs485BaseProtocolWidget(protocol, parent)
{
	addressEditor()->setRange(1, 247);
	inputBufferEditor()->setRange(32, 300);
	outputBufferEditor()->setRange(32, 1450);

    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);
    auto stopBitsParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatStopBits);

	auto editor = new ListParamEditor(g_Formats);
	editor->setValue(QString::number(protocol->currentFormat()));

	auto onChanged = [=](const QString &value) {
		int parityValue = 0;
		int lengthValue = 0;
		switch (value.toUInt())
		{
		case Format8Bit_No:
			parityValue = Rs485::Parity::No;
			lengthValue = Rs485::Format::F8Bit;
			break;
		case Format8Bit_Odd:
			parityValue = Rs485::Parity::Odd;
			lengthValue = Rs485::Format::F9Bit;
			break;
		case Format8Bit_Even:
			parityValue = Rs485::Parity::Even;
			lengthValue = Rs485::Format::F9Bit;
			break;
		default: break;
		}

		parityParam->updateValue(QString::number(parityValue));
		lengthParam->updateValue(QString::number(lengthValue));
		stopBitsParam->updateValue(QString::number(Stop1Bit));
	};
	connect(editor, &ParamEditor::valueChanged, this, onChanged);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(editor, Rs485::Text::CharacterFormat);
}
