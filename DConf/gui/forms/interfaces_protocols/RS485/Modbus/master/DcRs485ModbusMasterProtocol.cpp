#include "DcRs485ModbusMasterProtocol.h"

#include <qpushbutton.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusDialogSlaveInit_readOrCreate.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusDialogSlaveInit_standartOrNot.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

namespace {
}

namespace Text {
	const QString TransparentChannelPort = "Порт TCP для прозрачного канала";
	const QString TcpAccess = "Доступ через Modbus/TCP";
	const QString ConnectionTimeout = "Таймаут соединения (сек)";
	const QString FrameIntervalStandart = "Стандарт Modbus";
	const QString NoPort = "Нет";
}

DcRs485ModbusMasterProtocol::DcRs485ModbusMasterProtocol(DcController *device, int portIdx) :
	DcRs485ModbusProtocol(device, portIdx)
{	
}


void DcRs485ModbusMasterProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485ModbusProtocol::fillReport(report, main);

    auto tcpParam = device()->paramsRegistry().element(SP_PROTOCOL, 4);
	auto isTcp = tcpParam->value() == QString::number(portIdx() + 1);
	main->addRow({ Text::TcpAccess, DcReportTable::checkedSign(isTcp) });	

	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount, Rs485::Text::SlavesCount));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval));

    auto frameIntervalValue = device()->paramsRegistry().element(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Options)->value();
	main->addRow({ Rs485::Text::FrameInterval, frameIntervalValue.toUInt() ? frameIntervalValue : Text::FrameIntervalStandart });

    auto transparentPortParam = device()->paramsRegistry().element(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Address);
	auto transparentPort = transparentPortParam->value().toUShort();
	main->addRow({ Text::TransparentChannelPort, transparentPort ? QString::number(transparentPort) : Text::NoPort });
	if (transparentPort || isTcp)
		main->addRow(DcReportTable::ParamRecord(SP_PROTOCOL, 6, Text::ConnectionTimeout));
}

//=============================================================================
DcRs485ModbusMasterProtocolWidget::DcRs485ModbusMasterProtocolWidget(DcRs485ModbusMasterProtocol *protocol, QWidget *parent) :
	DcRs485ModbusProtocolWidget(protocol, parent),
    m_slaveWidget(nullptr),
    m_SlaveLoader(device(), portIdx())
{
	addressLabel()->setText(Text::TransparentChannelPort);
	addressEditor()->setRange(0, USHRT_MAX);
	addressEditor()->setSpecialValueText(Text::NoPort);

	const auto tcpSetValue = QString::number(portIdx() + 1);
    auto tcpParam = device()->paramsRegistry().element(SP_PROTOCOL, 4);
	auto tcpCheckBox = new QCheckBox;
	tcpCheckBox->setTristate(false);
	tcpCheckBox->setChecked(tcpParam->value() == tcpSetValue);
	connect(tcpCheckBox, &QCheckBox::toggled, this, [=](bool checked) { tcpParam->updateValue(checked ? tcpSetValue : "0"); });

	auto connectionTimeoutLabel = new QLabel(Text::ConnectionTimeout);
    auto connectionTimeoutEditor = new LineParamEditor(device()->paramsRegistry().element(SP_PROTOCOL, 6));
	auto showTimeoutEditor = [=]() {
		auto visible = addressEditor()->param()->value().toUInt() || tcpCheckBox->isChecked();
		connectionTimeoutLabel->setVisible(visible);
		connectionTimeoutEditor->setVisible(visible);
	};
	connect(addressEditor(), &ParamEditor::valueChanged, this, showTimeoutEditor);
	connect(tcpCheckBox, &QCheckBox::toggled, this, showTimeoutEditor);
	showTimeoutEditor();

    auto frameIntervalEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_DWORD, indexBase() + Rs485::Indexs::DWord::Options));
	frameIntervalEditor->setRange(0, USHRT_MAX);
	frameIntervalEditor->setSpecialValueText(Text::FrameIntervalStandart);

	auto frameIntervalStandartButton = new QPushButton("По стандарту Modbus");
	connect(frameIntervalStandartButton, &QPushButton::clicked, this, [=]() { frameIntervalEditor->setValue(QString::number(0)); });

    auto slaveCountEditor = new LineParamEditor(device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount));
	slaveCountEditor->setRange(0, device()->rs485SlavesMax() / device()->rs485PortsCount());

	auto slaveSettingsButton = new QPushButton("Настройка ведомых");
	slaveSettingsButton->setEnabled(slaveCountEditor->param()->value().toUInt());
	connect(slaveCountEditor, &ParamEditor::valueChanged, this, [=](const QString &value) {slaveSettingsButton->setEnabled(value.toUInt()); });
	connect(slaveSettingsButton, &QPushButton::clicked, this, &DcRs485ModbusMasterProtocolWidget::onSlaveSettings);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	mg.addWidget(tcpCheckBox, Text::TcpAccess);	
	mg.addWidget(slaveCountEditor, new QLabel(Rs485::Text::SlavesCount), slaveSettingsButton);
	mg.addLineEditor(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries, 0, 5);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse, 10, 2000);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval, 100);
	mg.addWidget(frameIntervalEditor, new QLabel(Rs485::Text::FrameInterval), frameIntervalStandartButton);
	mg.addWidget(connectionTimeoutEditor, connectionTimeoutLabel);

	mg.addVerticalStretch();
}

DcRs485ModbusMasterProtocolWidget::~DcRs485ModbusMasterProtocolWidget()
{
	delete m_slaveWidget;
}

void DcRs485ModbusMasterProtocolWidget::onSlaveSettings()
{
    auto param = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount);
	int slaveCount = param->value().toInt();
	if (slaveCount == 0) {
		MsgBox::warning("Количество слейвов было задано как ноль. Настройка невозможна.");
		return;
	}

	ModbusDialogSlaveInir_readOrCreate dialog1;	// Создаем диалог чтобы узнать загружаем ли или создаем новую
	int readConfig = dialog1.exec();
	if (readConfig == ModbusDialogSlaveInir_readOrCreate::loadConfig) {	// Загружаем из модели или из файла

		QVector<spSlave_t> vectorOfSlaves;	// Будем складывать слейвы сюда
		int result = m_SlaveLoader.readSlaveConfig(slaveCount, vectorOfSlaves);	// Пытаемся получить конфигурацию для слейвов
		if (result != ModbusSlaveFileSaver::ALL_IS_OK) {
			//MsgBox::error("Невозможно загрузить конфигурацию слейвов");
			return;
		}
		if (vectorOfSlaves.isEmpty()) {
			MsgBox::error("Вектор слейвов пуст");
			return;
		}

		if (vectorOfSlaves[0]->isModbusModeModify()) {	//  Проверяем, был ли слейв настроен в режиме модифицированного модбаса
            m_slaveWidget = new ModbusWidgetSlaveSettings(portIdx(), device(), vectorOfSlaves, true);
		}
		else
            m_slaveWidget = new ModbusWidgetSlaveSettings(portIdx(), device(), vectorOfSlaves, false);

		if (m_slaveWidget)
			m_slaveWidget->show();

	}
	else if (readConfig == ModbusDialogSlaveInir_readOrCreate::createConfig) {	// Или создаем 
		ModbusDialogSlaveInit_standartOrNot dial2;
		int result = dial2.exec();	// Стандартный или модифицированный модбас?

		bool isModifyModbus;
		if (result == ModbusDialogSlaveInit_standartOrNot::standartModbus) {
			isModifyModbus = false;
            m_slaveWidget = new ModbusWidgetSlaveSettings(portIdx(), device(), slaveCount, isModifyModbus);
			m_slaveWidget->show();
		}
		else if (result == ModbusDialogSlaveInit_standartOrNot::modifyModbus) {
			isModifyModbus = true;
            m_slaveWidget = new ModbusWidgetSlaveSettings(portIdx(), device(), slaveCount, isModifyModbus);
			m_slaveWidget->show();
		}

	}
	else {
		//MsgBox::question("Не выбран тип настройки");
	}
}
