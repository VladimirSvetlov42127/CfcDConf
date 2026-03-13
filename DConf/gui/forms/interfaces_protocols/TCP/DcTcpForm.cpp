#include "DcTcpForm.h"

#include <qtabwidget.h>

#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_ReservationList = { "Нет(Режим коммутатора)", "IEC 62439-3(PRP)", "IEC 62439-3(HSR)" };
	const ListEditorContainer g_ConnectionModeList = { "Пассивный", "Активный", "Отключён" };

	const uint32_t PARAM_CRITICAL_TO_NO_CONNECTION = 0x6230;
}

namespace Text {
	namespace Ip {
		const QString Title = "IP";

		const QString IP4_ADDR0 = "IP-адрес";
		const QString IP4_NETMASK0 = "Маска";
		const QString IP4_GW_ADDR0 = "Шлюз";
		const QString VLAN_TAG7 = "Номер VLAN";
		const QString VLAN_PRIO7 = "Приоритет VLAN";
	}

	namespace Reservation {
		const QString Title = "Резервирование";

		const QString PROTOCOL1 = "Резервирование";
		const QString MAC_ADDR9 = "MAC адрес PRP/HSR SUP пакетов";
		const QString VLAN_TAG6 = "Номер VLAN для PRP/HSR";
		const QString VLAN_PRIO6 = "Приоритет VLAN PRP/HSR";
		const QString RESERVE_PARAMS0 = "Интервал выдачи PRP/HSR_Supervision пакетов (мс)";
		const QString RESERVE_PARAMS1 = "Время существования записи узла в PRP/HSR таблице(мс)";
		const QString RESERVE_PARAMS2 = "Время существования записи в PRP/HSR таблице дублирования(мс)";
		const QString RESERVE_PARAMS3 = "Время молчания после рестарта(мс)";
	}

	namespace Protection {
		const QString Title = "Защита сети";

		const QString IFACE_OPTIONS0 = "Уровень защиты от широковещательного шторма (%)";
		const QString IFACE_OPTIONS1 = "Включение Multicast пакетов в Broadcast защиту";
		const QString IFACE_OPTIONS2 = "Защита от образования петель в режиме коммутатора";
	}

	namespace Firewall {
		const QString Title = "Брендмауэр входящих соединений";

		const QString PROTOCOL5 = "Включён";
		const QString FIREWALL_IP0 = "IP-адрес %1";
		const QString FIREWALL_IP1 = "Маска %1";
	}

	namespace Sybus {
		const QString Title = "Sybus";
	}

	namespace Sntp {
		const QString Title = "SNTP";
	}

	namespace Iec1041 {
		const QString Title = "Клиент МЭК-104 1";
	}

	namespace Iec1042 {
		const QString Title = "Клиент МЭК-104 2";
	}

	namespace ModbusTcp {
		const QString Title = "MODBUS/TCP";
	}

	const QString CRITICAL_TO_NO_CONNECTION = "Критичен к отсутствию соединения";
	const QString ONLINE_SWITCHERS = "Режим соединения";
	const QString TIMER_VALUES = "Таймаут активности соединения (сек)";

	const QString IP4_PEER_ADDR = "IP-адрес (активный режим)";
	const QString IP4_PEER_ADDR_SERVER1 = "IP-адрес сервера 1";
	const QString IP4_PEER_ADDR_SERVER2 = "IP-адрес сервера 2";

	const QString IPPORT = "IP порт";
	const QString IPPORT_ACTIVE = "IP порт (активный режим)";
	const QString IPPORT_PASSIVE = "IP порт (пассивный режим)";
	const QString IPPORT_SERVER1 = "IP порт сервера 1";
	const QString IPPORT_SERVER2 = "IP порт сервера 2";
	const QString IPPORT_SNTP = "IP порт SNTP клиента";
}

DcTcpForm::DcTcpForm(DcController *controller) :
	DcForm(controller,  "Настройки TCP", false)
{
	QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

	QTabWidget *tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createIpTab(), Text::Ip::Title);
	tabWidget->addTab(createReservationTab(), Text::Reservation::Title);
	tabWidget->addTab(createProtectionTab(), Text::Protection::Title);
	tabWidget->addTab(createFirewallTab(), Text::Firewall::Title);
	tabWidget->addTab(createSybuslTab(), Text::Sybus::Title);
	tabWidget->addTab(createSntpTab(), Text::Sntp::Title);
	tabWidget->addTab(createIec1041Tab(), Text::Iec1041::Title);
	tabWidget->addTab(createIec1042Tab(), Text::Iec1042::Title);
	tabWidget->addTab(createModbusTcpTab(), Text::ModbusTcp::Title);
}

bool DcTcpForm::isAvailableFor(DcController * controller)
{
    return controller->hasEthernet();
}

void DcTcpForm::fillReport(DcIConfigReport * report)
{
	auto device = report->device();
    auto param = device->paramsRegistry().element(SP_IP4_ADDR, 0);
	if (param) {
		report->insertSection(Text::Ip::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_IP4_ADDR, 0, Text::Ip::IP4_ADDR0));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_NETMASK, 0, Text::Ip::IP4_NETMASK0));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_GW_ADDR, 0, Text::Ip::IP4_GW_ADDR0));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_TAG, 7, Text::Ip::VLAN_TAG7));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_PRIO, 7, Text::Ip::VLAN_PRIO7));
		report->insertTable(table);
	}

    param = device->paramsRegistry().element(SP_RESERVE_PARAMS, 0);
	if (param) {
		report->insertSection(Text::Reservation::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_PROTOCOL, 1, g_ReservationList.toHash(), Text::Reservation::PROTOCOL1));
		table.addRow(DcReportTable::ParamRecord(SP_MAC_ADDR, 9, Text::Reservation::MAC_ADDR9));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_TAG, 6, Text::Reservation::VLAN_TAG6));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_PRIO, 6, Text::Reservation::VLAN_PRIO6));
		table.addRow(DcReportTable::ParamRecord(SP_RESERVE_PARAMS, 0, Text::Reservation::RESERVE_PARAMS0));
		table.addRow(DcReportTable::ParamRecord(SP_RESERVE_PARAMS, 1, Text::Reservation::RESERVE_PARAMS1));
		table.addRow(DcReportTable::ParamRecord(SP_RESERVE_PARAMS, 2, Text::Reservation::RESERVE_PARAMS2));
		table.addRow(DcReportTable::ParamRecord(SP_RESERVE_PARAMS, 3, Text::Reservation::RESERVE_PARAMS3));
		report->insertTable(table);
	}

    param = device->paramsRegistry().element(SP_IFACE_OPTIONS, 0);
	if (param) {
		report->insertSection(Text::Protection::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_IFACE_OPTIONS, 0, Text::Protection::IFACE_OPTIONS0));
		table.addRow(DcReportTable::ParamRecord(SP_IFACE_OPTIONS, 1, Text::Protection::IFACE_OPTIONS1, true));
		table.addRow(DcReportTable::ParamRecord(SP_IFACE_OPTIONS, 2, Text::Protection::IFACE_OPTIONS2, true));
		report->insertTable(table);
	}

    auto paramV2 = device->paramsRegistry().parameter(SP_FIREWALL_IP);
	if (paramV2) {
		report->insertSection(Text::Firewall::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(SP_PROTOCOL, 5, Text::Firewall::PROTOCOL5, true));
        for (size_t i = 0; i < paramV2->elementsCount() / 2; i++) {
			int paramIdxBase = i * 2;
			int num = i + 1;

			table.addRow(DcReportTable::ParamRecord(SP_FIREWALL_IP, paramIdxBase, Text::Firewall::FIREWALL_IP0.arg(num)));
			table.addRow(DcReportTable::ParamRecord(SP_FIREWALL_IP, paramIdxBase + 1, Text::Firewall::FIREWALL_IP1.arg(num)));
		}
		report->insertTable(table);
	}
	
	int idx = 0;
    param = device->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (param) {
		report->insertSection(Text::Sybus::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION, true));
		table.addRow(DcReportTable::ParamRecord(SP_ONLINE_SWITCHERS, idx, g_ConnectionModeList.toHash(), Text::ONLINE_SWITCHERS));
		table.addRow(DcReportTable::ParamRecord(SP_TIMER_VALUES, idx, Text::TIMER_VALUES));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, 0, Text::IP4_PEER_ADDR));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 0, Text::IPPORT_ACTIVE));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 6, Text::IPPORT_PASSIVE));
		report->insertTable(table);
	}

	idx++;
    param = device->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (param) {
		report->insertSection(Text::Sntp::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION, true));
		table.addRow(DcReportTable::ParamRecord(SP_ONLINE_SWITCHERS, idx, g_ConnectionModeList.toHash(), Text::ONLINE_SWITCHERS));
		table.addRow(DcReportTable::ParamRecord(SP_TIMER_VALUES, idx, Text::TIMER_VALUES));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, 3, Text::IP4_PEER_ADDR_SERVER1));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 4, Text::IPPORT_SERVER1));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, 4, Text::IP4_PEER_ADDR_SERVER2));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 5, Text::IPPORT_SERVER2));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 3, Text::IPPORT_SNTP));
		report->insertTable(table);
	}

	idx++;
    param = device->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (param) {
		report->insertSection(Text::Iec1041::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION, true));
		table.addRow(DcReportTable::ParamRecord(SP_ONLINE_SWITCHERS, idx, g_ConnectionModeList.toHash(), Text::ONLINE_SWITCHERS));
		table.addRow(DcReportTable::ParamRecord(SP_TIMER_VALUES, idx, Text::TIMER_VALUES));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, idx + 3, Text::IP4_PEER_ADDR));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, idx + 5, Text::IPPORT));
		report->insertTable(table);
	}

	idx++;
    param = device->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (param) {
		report->insertSection(Text::Iec1042::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION, true));
		table.addRow(DcReportTable::ParamRecord(SP_ONLINE_SWITCHERS, idx, g_ConnectionModeList.toHash(), Text::ONLINE_SWITCHERS));
		table.addRow(DcReportTable::ParamRecord(SP_TIMER_VALUES, idx, Text::TIMER_VALUES));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, idx + 3, Text::IP4_PEER_ADDR));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, idx + 5, Text::IPPORT));
		report->insertTable(table);
	}

	idx++;
    param = device->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (param) {
		report->insertSection(Text::ModbusTcp::Title);
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION, true));
		table.addRow(DcReportTable::ParamRecord(SP_ONLINE_SWITCHERS, idx, g_ConnectionModeList.toHash(), Text::ONLINE_SWITCHERS));
		table.addRow(DcReportTable::ParamRecord(SP_TIMER_VALUES, idx, Text::TIMER_VALUES));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, idx + 3, Text::IP4_PEER_ADDR));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, idx + 5, Text::IPPORT));
		report->insertTable(table);
	}
}

QWidget * DcTcpForm::createIpTab() const
{
    auto param = controller()->paramsRegistry().element(SP_IP4_ADDR, 0);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addLineEditor(SP_IP4_ADDR, 0, Text::Ip::IP4_ADDR0);
	manager.addLineEditor(SP_IP4_NETMASK, 0, Text::Ip::IP4_NETMASK0);
	manager.addLineEditor(SP_IP4_GW_ADDR, 0, Text::Ip::IP4_GW_ADDR0);
	manager.addLineEditor(SP_VLAN_TAG, 7, Text::Ip::VLAN_TAG7);
	manager.addLineEditor(SP_VLAN_PRIO, 7, Text::Ip::VLAN_PRIO7);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createReservationTab() const
{
    auto param = controller()->paramsRegistry().element(SP_RESERVE_PARAMS, 0);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addListEditor(SP_PROTOCOL, 1, Text::Reservation::PROTOCOL1, g_ReservationList);
	manager.addLineEditor(SP_MAC_ADDR, 9, Text::Reservation::MAC_ADDR9);
	manager.addLineEditor(SP_VLAN_TAG, 6, Text::Reservation::VLAN_TAG6);
	manager.addLineEditor(SP_VLAN_PRIO, 6, Text::Reservation::VLAN_PRIO6);
	manager.addLineEditor(SP_RESERVE_PARAMS, 0, Text::Reservation::RESERVE_PARAMS0);
	manager.addLineEditor(SP_RESERVE_PARAMS, 1, Text::Reservation::RESERVE_PARAMS1);
	manager.addLineEditor(SP_RESERVE_PARAMS, 2, Text::Reservation::RESERVE_PARAMS2);
	manager.addLineEditor(SP_RESERVE_PARAMS, 3, Text::Reservation::RESERVE_PARAMS3);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createProtectionTab() const
{
    auto param = controller()->paramsRegistry().element(SP_IFACE_OPTIONS, 0);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addLineEditor(SP_IFACE_OPTIONS, 0, Text::Protection::IFACE_OPTIONS0);
	manager.addCheckEditor(SP_IFACE_OPTIONS, 1, Text::Protection::IFACE_OPTIONS1);
	manager.addCheckEditor(SP_IFACE_OPTIONS, 2, Text::Protection::IFACE_OPTIONS2);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createFirewallTab() const
{
    auto firewallParam = controller()->paramsRegistry().parameter(SP_FIREWALL_IP);
	if (!firewallParam)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(SP_PROTOCOL, 5, Text::Firewall::PROTOCOL5);
    for (size_t i = 0; i < firewallParam->elementsCount() / 2; i++) {
		int paramIdxBase = i * 2;
		int num = i + 1;

		manager.addLineEditor(SP_FIREWALL_IP, paramIdxBase, Text::Firewall::FIREWALL_IP0.arg(num));
		manager.addLineEditor(SP_FIREWALL_IP, paramIdxBase + 1, Text::Firewall::FIREWALL_IP1.arg(num));
	}

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createSybuslTab() const
{
	uint16_t idx = 0;
    auto param = controller()->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION);
	manager.addListEditor(SP_ONLINE_SWITCHERS, idx, Text::ONLINE_SWITCHERS, g_ConnectionModeList);
	manager.addLineEditor(SP_TIMER_VALUES, idx, Text::TIMER_VALUES);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 0, Text::IP4_PEER_ADDR);
	manager.addLineEditor(SP_IPPORT, 0, Text::IPPORT_ACTIVE);
	manager.addLineEditor(SP_IPPORT, 6, Text::IPPORT_PASSIVE);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createSntpTab() const
{
	uint16_t idx = 1;
    auto param = controller()->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION);
	manager.addListEditor(SP_ONLINE_SWITCHERS, idx, Text::ONLINE_SWITCHERS, g_ConnectionModeList);
	manager.addLineEditor(SP_TIMER_VALUES, idx, Text::TIMER_VALUES);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 3, Text::IP4_PEER_ADDR_SERVER1);
	manager.addLineEditor(SP_IPPORT, 4, Text::IPPORT_SERVER1);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 4, Text::IP4_PEER_ADDR_SERVER2);
	manager.addLineEditor(SP_IPPORT, 5, Text::IPPORT_SERVER2);
	manager.addLineEditor(SP_IPPORT, 3, Text::IPPORT_SNTP);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createIec1041Tab() const
{
	uint16_t idx = 2;
    auto param = controller()->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION);
	manager.addListEditor(SP_ONLINE_SWITCHERS, idx, Text::ONLINE_SWITCHERS, g_ConnectionModeList);
	manager.addLineEditor(SP_TIMER_VALUES, idx, Text::TIMER_VALUES);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 5, Text::IP4_PEER_ADDR);
	manager.addLineEditor(SP_IPPORT, 7, Text::IPPORT);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createIec1042Tab() const
{
	uint16_t idx = 3;
    auto param = controller()->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION);
	manager.addListEditor(SP_ONLINE_SWITCHERS, idx, Text::ONLINE_SWITCHERS, g_ConnectionModeList);
	manager.addLineEditor(SP_TIMER_VALUES, idx, Text::TIMER_VALUES);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 6, Text::IP4_PEER_ADDR);
	manager.addLineEditor(SP_IPPORT, 8, Text::IPPORT);

	manager.addStretch();
	return tab;
}

QWidget * DcTcpForm::createModbusTcpTab() const
{
	uint16_t idx = 4;
    auto param = controller()->paramsRegistry().element(PARAM_CRITICAL_TO_NO_CONNECTION, idx);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addCheckEditor(PARAM_CRITICAL_TO_NO_CONNECTION, idx, Text::CRITICAL_TO_NO_CONNECTION);
	manager.addListEditor(SP_ONLINE_SWITCHERS, idx, Text::ONLINE_SWITCHERS, g_ConnectionModeList);
	manager.addLineEditor(SP_TIMER_VALUES, idx, Text::TIMER_VALUES);
	manager.addLineEditor(SP_IP4_PEER_ADDR, 7, Text::IP4_PEER_ADDR);
	manager.addLineEditor(SP_IPPORT, 9, Text::IPPORT);

	manager.addStretch();
	return tab;
}
