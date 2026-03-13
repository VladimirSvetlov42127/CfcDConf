#include "DcRs485Iec101SlaveProtocol.h"

#include <cmath>
#include <dpc/gui/widgets/SpinBox.h>
#include <gui/forms/interfaces_protocols/IEC-101/DcIEC101ChannelsForm.h>
#include <gui/forms/interfaces_protocols/IEC-101/DcIEC101SettingsForm.h>

using namespace Dpc::Gui;

namespace {
	const uint16_t AddressStationLenMask = 0x0002;

	enum {
		Len1Byte = 1, 
		Len2Byte, 
		Len3Byte
	};

	const ListEditorContainer g_Len2ByteList = { {QString::number(Len1Byte), QString::number(Len2Byte)}, true };
	const ListEditorContainer g_Len3ByteList = { {QString::number(Len1Byte), QString::number(Len2Byte), QString::number(Len3Byte)}, true };
}

namespace Text {	
	const QString DifferentFromAsdu = "Установить отличный от общего адреса ASDU";
	const QString AddressStation = "Адрес станции";
	const QString AddressStationLen = "Длина адреса станции, байт";
	const QString Asdu = "Общий адрес ASDU"; 
	const QString AsduLen = "Длина общего адреса ASDU, байт";
	const QString ReasonLen = "Длина причины передачи, байт";
	const QString ObjectLen = "Длина адреса объекта информации, байт";
	const QString Discrets = "Дискреты";
	const QString Analogs = "Аналоги";
	const QString Counters = "Счетчики";
	const QString ParamAssignments = "Назначение параметров";
}

DcRs485Iec101SlaveProtocol::DcRs485Iec101SlaveProtocol(DcController *device, int portIdx) :
	DcRs485Iec101Protocol(device, portIdx)
{
}

void DcRs485Iec101SlaveProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485Iec101Protocol::fillReport(report, main);

	main->remove(4);

    auto addressStationParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Address);
    auto asduParam = device()->paramsRegistry().element(SP_IEC_104_PARAMS, 6);
    auto options = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
	auto addressStationValue = options->value().toUInt() & 1 ?
		addressStationParam->value() : QString::number(static_cast<uint8_t>(asduParam->value().toUInt()));

	auto addressStationLenValue = options->value().toUInt() & AddressStationLenMask ? QString::number(Len2Byte) : QString::number(Len1Byte);

	main->addRow({ Text::AddressStation, addressStationValue });
	main->addRow({ Text::AddressStationLen, addressStationLenValue });
	main->addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 6, Text::Asdu));
	main->addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 7, g_Len2ByteList.toHash(), Text::AsduLen));
	main->addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 8, g_Len2ByteList.toHash(), Text::ReasonLen));
	main->addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 9, g_Len3ByteList.toHash(), Text::ObjectLen));
}

//=============================================================================
DcRs485Iec101SlaveProtocolWidget::DcRs485Iec101SlaveProtocolWidget(DcRs485Iec101SlaveProtocol *protocol, QWidget *parent) :
	DcRs485Iec101ProtocolWidget(protocol, parent)
{
	addressLabel()->setVisible(false);
	addressEditor()->setVisible(false);

    auto options = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);

	m_differentFromAsduEditor = new CheckParamEditor(options, 0);
	m_differentFromAsduEditor->setText(Text::DifferentFromAsdu);
    auto addressStationParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Address);
	auto addressStationEditor = new IntSpinBox;
	addressStationEditor->setRange(0, UCHAR_MAX);

	auto addressStationLenEditor = new ListParamEditor(g_Len2ByteList);
	addressStationLenEditor->setValue(options->value().toUInt() & AddressStationLenMask ? QString::number(Len2Byte) : QString::number(Len1Byte));
	connect(addressStationLenEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {
		auto optionsVal = options->value().toUInt();
		switch (value.toUInt())
		{
		case Len1Byte: optionsVal &= (~AddressStationLenMask); break;
		case Len2Byte: optionsVal |= AddressStationLenMask; break;
		default: return;
		}
		options->updateValue(QString::number(optionsVal));
	});

    auto asduLenEditor = new ListParamEditor(device()->paramsRegistry().element(SP_IEC_104_PARAMS, 7), g_Len2ByteList);
    auto asduEditor = new LineParamEditor(device()->paramsRegistry().element(SP_IEC_104_PARAMS, 6));
	asduEditor->setRange(0, std::pow(UCHAR_MAX + 1, asduLenEditor->param()->value().toUInt()) - 1);
	connect(asduEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {
		if (!m_differentFromAsduEditor->checked())
			addressStationEditor->setValue(static_cast<uint8_t>(value.toUInt()));
	});
	connect(asduLenEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {
		asduEditor->setRange(0, std::pow(UCHAR_MAX + 1, value.toUInt()) - 1);
	});

	auto onDifferentFromAsduEditorChanged = [=]() {
		if (m_differentFromAsduEditor->checked()) {
			addressStationEditor->setEnabled(true);
			addressStationEditor->setValue(static_cast<uint8_t>(addressStationParam->value().toUInt()));
		}
		else {
			addressStationEditor->setEnabled(false);
			addressStationEditor->setValue(static_cast<uint8_t>(asduEditor->param()->value().toUInt()));
		}
	};
	onDifferentFromAsduEditorChanged();
	connect(m_differentFromAsduEditor, &ParamEditor::valueChanged, this, onDifferentFromAsduEditorChanged);
	connect(addressStationEditor, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) {
		if (m_differentFromAsduEditor->checked())
			addressStationParam->updateValue(QString::number(value));
	});

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(addressStationEditor, new QLabel(Text::AddressStation), m_differentFromAsduEditor);
	mg.addWidget(addressStationLenEditor, Text::AddressStationLen);
	mg.addWidget(asduEditor, Text::Asdu);
	mg.addWidget(asduLenEditor, Text::AsduLen);
	mg.addListEditor(SP_IEC_104_PARAMS, 8, Text::ReasonLen, g_Len2ByteList);
	mg.addListEditor(SP_IEC_104_PARAMS, 9, Text::ObjectLen, g_Len3ByteList);

	mg.addVerticalStretch();


    DcIEC101SettingsForm iec101Form(device());
	addTab(iec101Form.createProtocolAssingmentsTab(), Text::ParamAssignments);

    DcIEC101ChannelsForm channelsForm(device());
	addTab(channelsForm.createChannelsTable(DEF_SIG_TYPE_DISCRETE), Text::Discrets);
	addTab(channelsForm.createChannelsTable(DEF_SIG_TYPE_ANALOG), Text::Analogs);
	addTab(channelsForm.createChannelsTable(DEF_SIG_TYPE_COUNTER), Text::Counters);
}
