#include "dc_rs485_transparent_protocol.h"

#include <dpc/gui/widgets/LineEdit.h>

namespace {
    const QList<int> g_Speed = { 1200, 2400, 4800, 9600, 19200, 38400, 56000, 57600, 64000, 115200, 128000, 153600, 230400, 256000, 307200, 460800 };

    enum {
        Format8Bit_No,
        Format8Bit_Odd,
        Format8Bit_Even,
        Format7Bit_Odd,
        Format7Bit_Even
    };

    enum {
        Stop1Bit = 1,
        Stop2Bit = 2
    };

    const ListEditorContainer g_Formats = {
        {"8 бит без паритета", Format8Bit_No},
        {"8 бит + нечет", Format8Bit_Odd},
        {"8 бит + чет", Format8Bit_Even},
        {"7 бит + нечет", Format7Bit_Odd},
        {"7 бит + чет", Format7Bit_Even}
    };

    const ListEditorContainer g_StopBits = {
        {"1", Stop1Bit},
        {"2", Stop2Bit}
    };

    const uint16_t FLAG_MONITOR_MODE = 0x1000;
    const uint16_t FLAG_END_OF_PACKET1 = 0x2000;
    const uint16_t FLAG_END_OF_PACKET2 = 0x4000;
    const uint16_t FLAG_PAUSE_US = 0x6000;

    const ListEditorContainer g_WorkMode = {
        {"Запрос-ответ", 0},
        {"Монитор", FLAG_MONITOR_MODE}
    };

    const ListEditorContainer g_EndOfPacketSign = {
        {"Пауза (мс)", 0},
        {"1 Спец. символ", FLAG_END_OF_PACKET1},
        {"2 Спец. символ", FLAG_END_OF_PACKET2},
        {"Пауза (мкс)", FLAG_PAUSE_US},
    };
}

namespace Text {
    const QString StopBits = "Кол-во стоповых бит";
    const QString TcpIpPort = "TCP/IP порт";
    const QString TcpIpPortTimeout = "Таймаут активности соединения (сек)";
    const QString DeviceTimeout = "Таймаут ответа устройства (мсек)";
    const QString WorkMode = "Режим работы";
    const QString EndOfPacketSign = "Признак конца пакета";
}

DcRs485TransparentProtocol::DcRs485TransparentProtocol(DcController *device, int portIdx)
    : DcRs485BaseProtocol(device, portIdx, g_Speed)
{

}

void DcRs485TransparentProtocol::fillReport(DcIConfigReport *report, DcReportTable *main)
{
    DcRs485BaseProtocol::fillReport(report, main);
    main->addRow({ Rs485::Text::CharacterFormat, g_Formats.text(QVariant(currentFormat())) });
}

int DcRs485TransparentProtocol::currentFormat() const
{
    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);

    int currentFormatValue = -1;
    auto parityValue = parityParam->value().toUInt();
    auto lengthValue = lengthParam->value().toUInt();

    if (Rs485::Format::F8Bit == lengthValue) {
        if (Rs485::Parity::No == parityValue)
            currentFormatValue = Format8Bit_No;
        if (Rs485::Parity::Odd == parityValue)
            currentFormatValue = Format7Bit_Odd;
        if (Rs485::Parity::Even == parityValue)
            currentFormatValue = Format7Bit_Even;
    }

    if (Rs485::Format::F9Bit == lengthValue) {
        if (Rs485::Parity::Odd == parityValue)
            currentFormatValue = Format8Bit_Odd;
        if (Rs485::Parity::Even == parityValue)
            currentFormatValue = Format8Bit_Even;
    }

    return currentFormatValue;
}

DcRs485TransparentProtocolWidget::DcRs485TransparentProtocolWidget(DcRs485TransparentProtocol *protocol, QWidget *parent)
    : DcRs485BaseProtocolWidget(protocol, parent)
{
    addressLabel()->hide();
    addressEditor()->hide();

    inputBufferEditor()->setRange(32, 1450);
    outputBufferEditor()->setRange(32, 1450);

    auto editor = new ListParamEditor(g_Formats);
    editor->setValue(QString::number(protocol->currentFormat()));

    auto parityParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatParity);
    auto lengthParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatLenght);
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
        case Format7Bit_Odd:
            parityValue = Rs485::Parity::Odd;
            lengthValue = Rs485::Format::F8Bit;
            break;
        case Format7Bit_Even:
            parityValue = Rs485::Parity::Even;
            lengthValue = Rs485::Format::F8Bit;
            break;
        default: break;
        }

        parityParam->updateValue(QString::number(parityValue));
        lengthParam->updateValue(QString::number(lengthValue));
    };
    connect(editor, &ParamEditor::valueChanged, this, onChanged);

    auto optionsParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options);
    auto optionsParamValue = optionsParam->value().toUInt();
    auto workModeEditor = new ListParamEditor(g_WorkMode);
    workModeEditor->setValue(optionsParamValue & FLAG_MONITOR_MODE ? "1" : "0");
    connect(workModeEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {
        auto optionsValue = optionsParam->value().toUInt();
        auto newMode = value.toUInt();
        if (!newMode)
            optionsValue &= ~FLAG_MONITOR_MODE;
        else
            optionsValue |= newMode;
        optionsParam->updateValue(QString::number(optionsValue));
    });

    auto endOfPacketEditor = new ListParamEditor(g_EndOfPacketSign);
    if (FLAG_PAUSE_US & optionsParamValue)
        endOfPacketEditor->setValue(QString::number(FLAG_PAUSE_US));
    else if (FLAG_END_OF_PACKET2 & optionsParamValue)
        endOfPacketEditor->setValue(QString::number(FLAG_END_OF_PACKET2));
    else if (FLAG_END_OF_PACKET1 & optionsParamValue)
        endOfPacketEditor->setValue(QString::number(FLAG_END_OF_PACKET1));
    else
        endOfPacketEditor->setValue("0");

    auto onEndOfPacketEditorChanged = [=](const QString &value) {
        auto optionsValue = optionsParam->value().toUInt();
        auto endOfPacketSign = value.toUInt();
        if (!endOfPacketSign)
            optionsValue &= ~FLAG_PAUSE_US;
        else
            optionsValue |= endOfPacketSign;
        optionsParam->updateValue(QString::number(optionsValue));
    };
    connect(endOfPacketEditor, &ParamEditor::valueChanged, this, onEndOfPacketEditorChanged);

    auto endOfPacketArgEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval));

    EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
    mg.addWidget(editor, Rs485::Text::CharacterFormat);
    mg.addListEditor(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::FormatStopBits, Text::StopBits, g_StopBits);
    mg.addWidget(workModeEditor, Text::WorkMode);
    mg.addWidget(endOfPacketEditor, new QLabel(Text::EndOfPacketSign), endOfPacketArgEditor);
    auto tcpIpPortEditor = mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::TcpIpPort, Text::TcpIpPort);
    if (tcpIpPortEditor)
        tcpIpPortEditor->setSpecialValueText("Нет");
    auto tcpIpPortTimeoutEditor = mg.addLineEditor(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Options, Text::TcpIpPortTimeout);
    if (tcpIpPortTimeoutEditor)
        tcpIpPortTimeoutEditor->setSpecialValueText("Нет");
    auto deviceTimeoutEditor = mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Text::DeviceTimeout);
    if (deviceTimeoutEditor)
        deviceTimeoutEditor->setSpecialValueText("Нет");

    mg.addVerticalStretch();
}
