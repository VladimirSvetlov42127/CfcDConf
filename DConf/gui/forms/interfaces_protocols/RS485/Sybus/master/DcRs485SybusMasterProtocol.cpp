#include "DcRs485SybusMasterProtocol.h"

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/TabSybusMasterIdentification.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/TabSybusMasterChannels.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/TabSybusInitialization.h>
#include <gui/forms/interfaces_protocols/RS485/common_slave_functions.h>

using namespace Dpc::Gui;

namespace {
	enum {
		Sync1980 = 1,
		Sync1970
	};
}

namespace Text {
	const QString Sync1980 = "Cинхронизация времени от 1980 года (мс)";
	const QString Sync1970 = "Cинхронизация времени от 1970 года (мкс)";

	const QString TabIdentification = "Идентификация ведомого";
	const QString TabChannels = "Настройка каналов";
	const QString TabInitialization = "Команды инициализации";
}

DcRs485SybusMasterProtocol::DcRs485SybusMasterProtocol(DcController * device, int portIdx) :
	DcRs485SybusProtocol(device, portIdx)
{
}

void DcRs485SybusMasterProtocol::fillReport(DcIConfigReport * report, DcReportTable * main)
{
	DcRs485SybusProtocol::fillReport(report, main);

	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount, Rs485::Text::SlavesCount));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Text::Sync1980, true, Sync1980));
	main->addRow(DcReportTable::ParamRecord(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Text::Sync1970, true, Sync1970));
}

//=============================================================================
DcRs485SybusMasterProtocolWidget::DcRs485SybusMasterProtocolWidget(DcRs485SybusMasterProtocol *protocol, QWidget *parent) :
	DcRs485SybusProtocolWidget(protocol, parent)
{
    auto slavesParam = device()->paramsRegistry().element(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::SlaveCount);
	auto currentSlavesCount = slavesParam->value().toUInt();

    auto slaveIdentificationTab = new TabSybusMasterIdentification(device(), portIdx(), this);
	slaveIdentificationTab->build();

    auto slaveChannelsTab = new TabSybusMasterChannels(device(), portIdx(), this);
	slaveChannelsTab->build();
	connect(this, &QTabWidget::currentChanged, this, [=](int) {slaveChannelsTab->isAllParametersValid(); });

	auto slaveInitializationTab = new TabSybusInitialization(device(), portIdx(), currentSlavesCount, this);

	EditorsManager mg(device(), static_cast<QGridLayout*>(mainWidget()->layout()));
	auto slavesCountEditor = mg.addLineEditor(slavesParam, Rs485::Text::SlavesCount, 0, device()->rs485SlavesMax() / device()->rs485PortsCount());
	mg.addLineEditor(SP_USARTPRTPAR_BYTE, indexBase() + Rs485::Indexs::Byte::Retries, Rs485::Text::SlavesRetries, 0, 5);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Timeout, Rs485::Text::TimeoutResponse, 10, 2000);
	mg.addLineEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Interval, Rs485::Text::PollingInterval, 100);
	mg.addCheckEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Text::Sync1980, Sync1980);
	mg.addCheckEditor(SP_USARTPRTPAR_WORD, indexBase() + Rs485::Indexs::Word::Options, Text::Sync1970, Sync1970);	

	mg.addVerticalStretch();

	auto identificationIndex = addTab(slaveIdentificationTab->formIdentification(), Text::TabIdentification);
	auto channelsIndex = addTab(slaveChannelsTab->formChannels(), Text::TabChannels);
	auto initializationIndex = addTab(slaveInitializationTab, Text::TabInitialization);
	setTabEnabled(identificationIndex, currentSlavesCount);
	setTabEnabled(channelsIndex, currentSlavesCount);
	setTabEnabled(initializationIndex, currentSlavesCount);	

	auto onSlavesCountChanged = [=](const QString &value) {
		auto slavesCount = value.toUInt();
		slaveInitializationTab->setSlaveCount(slavesCount);

		bool enable = slavesCount;
		setTabEnabled(identificationIndex, enable);
		setTabEnabled(channelsIndex, enable);
		setTabEnabled(initializationIndex, enable);

		if (enable) {
			slaveIdentificationTab->updateSlaveList();
			slaveChannelsTab->updateSlaveList();
		}

		if (!resetUnusedSlaves(device(), portIdx(), value.toInt())) {
			MsgBox::warning("Возникли ошибки при попытке выключить неиспользуемых слейвов.");
		}
	};
	connect(slavesCountEditor, &ParamEditor::valueChanged, this, onSlavesCountChanged);
}
