#include "DcTimeSyncForm.h"

#include <qtabwidget.h>

#include <gui/editors/EditorsManager.h>

namespace {


	const uint16_t INDEX_TIMEOUT = 7;	

	const ListEditorContainer g_PolarityList = { "Фронт", "Спад" };

	const ListEditorContainer g_DelayMethodList = {
		{ "E2E", 1 }, 
		{ "P2P", 2 } 
	};

	const ListEditorContainer g_StepModeList = {
		{ "One-Step", 1 }, 
		{ "Two-Step", 0 } 
	};

	const ListEditorContainer g_TimeOffsetStamp = { "UTC", "Локальное" };

    const ListEditorContainer g_TimeSyncProtocolList = {
        {"Не поддерживается",			DcController::Protocol_NoUse},
        {"IEEE1588v2 UDP",				DcController::Protocol_IEEE1588v2_UDP},
        {"IEEE1588v2 802.3",			DcController::Protocol_IEEE1588v2_802_3},
        {"SNTPv4",						DcController::Protocol_SNTPv4},
        {"1PPS",						DcController::Protocol_1PPS},
        {"SYBUS/RTU",					DcController::Protocol_SYBUS_RTU},
        {"МЭК 60870-5-101/104 via GSM",	DcController::Protocol_IEC101_104vGSM},
        {"МЭК 60870-5-104",				DcController::Protocol_IEC104},
        {"SYBUS/TCP",					DcController::Protocol_SYBUS_TCP},
        {"MODBUS/RTU",					DcController::Protocol_MODBUS_RTU},
        {"MODBUS/TCP",					DcController::Protocol_MODBUS_TCP},
        {"SNTP/GPRS",					DcController::Protocol_SNTP_GPRS},
        {"МЭК 60870-5-103",				DcController::Protocol_IEC103},
        {"GSM сеть",					DcController::Protocol_GSM}
    };

	ListEditorContainer supportedProtocols(DcController* contr)
	{
		auto protocolList = g_TimeSyncProtocolList;
        if (auto param = contr->paramsRegistry().parameter(SP_TIMESYNC_LIST); param) {
            for (int i = 0; i < param->elementsCount(); i++) {
                if (!contr->paramsRegistry().element(SP_TIMESYNC_LIST, i)->value().toUInt())
					protocolList.exclude({ i });
			}
		}

		return protocolList;
	}

	ListEditorContainer g_timeOffsetList() {
		ListEditorContainer res;
		for (int i = -12; i <= 14; i++)
			res.append({ i > 0 ? QString("+%1").arg(i) : QString::number(i), i});

		return res;
	}
}

namespace Text {
	const QString Source = "Источник установки времени";
	const QString EventTimeStamp = "Выдача метки времени событий";

	const QString Timeout = "Таймаут данных от сервера";
	const QString TimeoutDescription = "0 - таймаут не отслеживается, 1..19 - 2^n секунд, 20..255 - n секунд";
	const QString VLAN_TAG7 = "Номер VLAN (равен VLAN TCP/IP)";
	const QString VLAN_PRIO7 = "Приоритет VLAN(равен VLAN TCP/IP)";
	const QString TIMESYNCROPARS0 = "Метод вычисления задержки";
	const QString TIMESYNCROPARS2 = "Период сообщений ANNOUNCE сервера (2^n сек)";
	const QString TIMESYNCROPARS3 = "Интервал SYNC пакетов (2^n сек)";
	const QString TIMESYNCROPARS4 = "Задержка первого запроса DELAYREQ (2^n сек)";
	const QString TIMESYNCROPARS6 = "Время ожидания ANNOUNCE сервера (2^n сек)";
	const QString TIMESYNCROPARS7 = "Номер домена";
	const QString TIMESYNCROPARS11 = "Период PDELAYREQ сообщений (2^n сек)";
	const QString TIMESYNCROPARS13 = "Локальное смещение времени в часах";

	namespace UDP {
		const QString MAC_ADDR3 = "MAC адрес для UDP пакетов";
		const QString IP_PIMARY = "IP адрес IEEE1588 pimary: 224.0.1.129";
		const QString IP_PDELAY = "IP адрес IEEE1588 pdelay: 224.0.0.107";
		const QString Port1 = "Порт 319";
		const QString Port2 = "Порт 320";
		const QString TIMESYNCROPARS10 = "Выдача IGMP сообщений";
	}

	namespace _802 {
		const QString MAC_ADDR1 = "MAC адрес для 802.3 пакетов";
		const QString MAC_ADDR2 = "MAC адрес для 802.3 пакетов peer delay";
		const QString VLAN_TAG5 = "Номер VLAN";
		const QString VLAN_PRIO5 = "Приоритет VLAN";
		const QString TIMESYNCROPARS1 = "Режим синхронизации";
	}

	namespace SNTP {
		const QString IP4_PEER_ADDR0 = "IP адрес сервера SNTP";
		const QString IP4_PEER_ADDR1 = "IP адрес альтернативного сервера SNTP";
		const QString IPPORT0 = "Порт сервера SNTP";
		const QString IPPORT1 = "Порт клиента SNTP";
		const QString TIMESYNCROPARS0 = "Задержка перед первым запросом к серверу(2^n сек)";
		const QString TIMESYNCROPARS1 = "Таймаут приема ответа сервера (2^n сек)";
		const QString TIMESYNCROPARS2 = "Период последующих запросов к серверу(2^n сек)";
		const QString TIMESYNCROPARS3 = "Период повтора запросов к серверу(2^n сек)";
		const QString TIMESYNCROPARS4 = "Степень замедления повторов запросов к серверу(2^n сек)";
		const QString TIMESYNCROPARS5 = "Максимальное замедление повторов запросов к серверу(2^n сек)";
		const QString TIMESYNCROPARS6 = "Локальное смещение времени в часах";
	}

	namespace _1PPS {
		const QString EDGE = "Полярность входного синхроимпульса";
		const QString DELAY = "Предустановленная задержка входного синхроимпульса (мкс)";
		const QString PROTOCOL3 = "Протокол установки даты и времени при синхронизации 1PPS";
	}
}

DcTimeSyncForm::DcTimeSyncForm(DcController *controller) :
	DcForm(controller,  "Настройки синхронизации времени", false),
	m_hasGlobalOffsetParam(false)
{	
	QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

	QTabWidget *tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createTimeSourceTab(), "Синхронизация времени");

	return;
}

bool DcTimeSyncForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_PROTOCOL}
	};

	return hasAny(controller, params);
}

void DcTimeSyncForm::fillReport(DcIConfigReport * report)
{
	auto device = report->device();

	report->insertSection();
    auto param = device->paramsRegistry().element(SP_PROTOCOL, 0);

	DcReportTable table(device);
	bool hasGlobalOffset = table.addRow(DcReportTable::ParamRecord(SP_TIMEZONE_HOUR, 0, g_timeOffsetList().toHash(), Text::SNTP::TIMESYNCROPARS6));
	table.addRow(DcReportTable::ParamRecord(SP_CHANGETIME, 0, g_TimeOffsetStamp.toHash(), Text::EventTimeStamp));
	table.addRow(DcReportTable::ParamRecord(param, supportedProtocols(device).toHash(), Text::Source));

	auto protocol = param->value().toUInt();
    if (DcController::Protocol_NoUse == protocol) {
		report->insertTable(table);
		return;
	}

	int idxBase = 0;
	auto ppsProtocolList = supportedProtocols(device);
    ppsProtocolList.exclude({ DcController::Protocol_IEEE1588v2_UDP, DcController::Protocol_IEEE1588v2_802_3, DcController::Protocol_1PPS });
	switch (protocol)
	{
    case DcController::Protocol_IEEE1588v2_UDP:
		table.addRow(DcReportTable::ParamRecord(SP_MAC_ADDR, 3, Text::UDP::MAC_ADDR3));
		table.addRow({ Text::UDP::IP_PIMARY, Text::UDP::Port1 });
		table.addRow({ Text::UDP::IP_PDELAY, Text::UDP::Port2 });
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_TAG, 7, Text::VLAN_TAG7));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_PRIO, 7, Text::VLAN_PRIO7));
		idxBase = 0 * PROFILE_SIZE;
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 0, g_DelayMethodList.toHash(), Text::TIMESYNCROPARS0));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 2, Text::TIMESYNCROPARS2));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 3, Text::TIMESYNCROPARS3));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 4, Text::TIMESYNCROPARS4));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 6, Text::TIMESYNCROPARS6));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 7, Text::TIMESYNCROPARS7));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 10, Text::UDP::TIMESYNCROPARS10, true));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 11, Text::TIMESYNCROPARS11));
		if (!hasGlobalOffset)
			table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 13, Text::TIMESYNCROPARS13));
		break;
    case DcController::Protocol_IEEE1588v2_802_3:
		table.addRow(DcReportTable::ParamRecord(SP_MAC_ADDR, 1, Text::_802::MAC_ADDR1));
		table.addRow(DcReportTable::ParamRecord(SP_MAC_ADDR, 2, Text::_802::MAC_ADDR2));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_TAG, 5, Text::_802::VLAN_TAG5));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_PRIO, 5, Text::_802::VLAN_PRIO5));		
		idxBase = 0 * PROFILE_SIZE;
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 0, g_DelayMethodList.toHash(), Text::TIMESYNCROPARS0));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 1, g_StepModeList.toHash(), Text::_802::TIMESYNCROPARS1));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 2, Text::TIMESYNCROPARS2));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 3, Text::TIMESYNCROPARS3));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 4, Text::TIMESYNCROPARS4));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 6, Text::TIMESYNCROPARS6));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 7, Text::TIMESYNCROPARS7));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 11, Text::TIMESYNCROPARS11));
		if (!hasGlobalOffset)
			table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 13, Text::TIMESYNCROPARS13));
		break;
    case DcController::Protocol_SNTPv4:
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, 0, Text::SNTP::IP4_PEER_ADDR0));
		table.addRow(DcReportTable::ParamRecord(SP_IP4_PEER_ADDR, 1, Text::SNTP::IP4_PEER_ADDR1));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 0, Text::SNTP::IPPORT0));
		table.addRow(DcReportTable::ParamRecord(SP_IPPORT, 1, Text::SNTP::IPPORT1));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_TAG, 7, Text::VLAN_TAG7));
		table.addRow(DcReportTable::ParamRecord(SP_VLAN_PRIO, 7, Text::VLAN_PRIO7));
		idxBase = 1 * PROFILE_SIZE;
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 0, Text::SNTP::TIMESYNCROPARS0));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 1, Text::SNTP::TIMESYNCROPARS1));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 2, Text::SNTP::TIMESYNCROPARS2));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 3, Text::SNTP::TIMESYNCROPARS3));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 4, Text::SNTP::TIMESYNCROPARS4));
		table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 5, Text::SNTP::TIMESYNCROPARS5));
		if (!hasGlobalOffset)
			table.addRow(DcReportTable::ParamRecord(SP_TIMESYNCROPARS, idxBase + 6, Text::SNTP::TIMESYNCROPARS6));
		break;
    case DcController::Protocol_1PPS:
		if (!table.addRow(DcReportTable::ParamRecord(SP_GBS_ENTITY_STATUS, 0, g_PolarityList.toHash(), Text::_1PPS::EDGE)))
			table.addRow(DcReportTable::ParamRecord(SP_1PPS_EDGE, 0, g_PolarityList.toHash(), Text::_1PPS::EDGE));
		if (!table.addRow(DcReportTable::ParamRecord(SP_SM_TIMEDIFSUMM, 0, Text::_1PPS::DELAY)))
			table.addRow(DcReportTable::ParamRecord(SP_1PPS_DELAY, 0, Text::_1PPS::DELAY));		
		table.addRow(DcReportTable::ParamRecord(SP_PROTOCOL, 3, ppsProtocolList.toHash(), Text::_1PPS::PROTOCOL3));
		break;
	default:
		break;
	}

	table.addRow(DcReportTable::ParamRecord(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout));
	report->insertTable(table);
}

QWidget * DcTimeSyncForm::createTimeSourceTab()
{
    auto timeSyncParam = controller()->paramsRegistry().element(SP_PROTOCOL, 0);
	if (!timeSyncParam)
		return nullptr;

	auto wCreator = [=](const QVariant &data) {
		switch (data.toUInt())
		{
        case DcController::Protocol_IEEE1588v2_UDP: return createIEEE1588v2_UDP();
        case DcController::Protocol_IEEE1588v2_802_3: return  createIEEE1588v2_802();
        case DcController::Protocol_SNTPv4: return  createSNTPv4();
        case DcController::Protocol_1PPS: return  create1PPS();
        case DcController::Protocol_NoUse: return new QWidget;
		default: return  createDefault();
		}
	};

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	if (manager.addListEditor(SP_TIMEZONE_HOUR, 0, Text::SNTP::TIMESYNCROPARS6, g_timeOffsetList()))
		m_hasGlobalOffsetParam = true;

	manager.addListEditor(SP_CHANGETIME, 0, Text::EventTimeStamp, g_TimeOffsetStamp);

	auto editor = manager.addListEditor(timeSyncParam, Text::Source, supportedProtocols(controller()));
    editor->setItemData(QString::number(DcController::Protocol_IEC101_104vGSM), "МЭК 60870-5-101 или МЭК 104 через GSM", Qt::ToolTipRole);
	manager.addWidgetCreatorLayout(editor, wCreator);

	tabLayout->setColumnStretch(2, 1);
	manager.addStretch();
	return tab;
}

QWidget * DcTimeSyncForm::createIEEE1588v2_UDP()
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager mg(controller(), layout);

	mg.addLineEditor(SP_MAC_ADDR, 3, Text::UDP::MAC_ADDR3);
	mg.addLineConst(Text::UDP::IP_PIMARY, Text::UDP::Port1);
	mg.addLineConst(Text::UDP::IP_PDELAY, Text::UDP::Port2);

	auto editor = mg.addLineEditor(SP_VLAN_TAG, 7, Text::VLAN_TAG7);
	if (editor)
		editor->setReadOnly(true);
	editor = mg.addLineEditor(SP_VLAN_PRIO, 7, Text::VLAN_PRIO7);
	if (editor)
		editor->setReadOnly(true);

	int profile = 0;
	int idxBase = profile * PROFILE_SIZE;
	mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 0, Text::TIMESYNCROPARS0, g_DelayMethodList);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 2, Text::TIMESYNCROPARS2);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 3, Text::TIMESYNCROPARS3);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 4, Text::TIMESYNCROPARS4);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 6, Text::TIMESYNCROPARS6);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 7, Text::TIMESYNCROPARS7);
	mg.addCheckEditor(SP_TIMESYNCROPARS, idxBase + 10, Text::UDP::TIMESYNCROPARS10);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 11, Text::TIMESYNCROPARS11);
	if (!m_hasGlobalOffsetParam)
		mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 13, Text::TIMESYNCROPARS13, g_timeOffsetList());
	mg.addLineEditor(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout);

	mg.addStretch();
	return widget;
}

QWidget * DcTimeSyncForm::createIEEE1588v2_802()
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager mg(controller(), layout);

	mg.addLineEditor(SP_MAC_ADDR, 1, Text::_802::MAC_ADDR1);
	mg.addLineEditor(SP_MAC_ADDR, 2, Text::_802::MAC_ADDR2);

	auto editor = mg.addLineEditor(SP_VLAN_TAG, 5, Text::_802::VLAN_TAG5);
	if (editor)
		editor->setReadOnly(true);
	editor = mg.addLineEditor(SP_VLAN_PRIO, 5, Text::_802::VLAN_PRIO5);
	if (editor)
		editor->setReadOnly(true);

	int profile = 0;
	int idxBase = profile * PROFILE_SIZE;
	mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 0, Text::TIMESYNCROPARS0, g_DelayMethodList);
	mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 1, Text::_802::TIMESYNCROPARS1, g_StepModeList);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 2, Text::TIMESYNCROPARS2);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 3, Text::TIMESYNCROPARS3);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 4, Text::TIMESYNCROPARS4);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 6, Text::TIMESYNCROPARS6);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 7, Text::TIMESYNCROPARS7);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 11, Text::TIMESYNCROPARS11);
	if (!m_hasGlobalOffsetParam)
		mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 13, Text::TIMESYNCROPARS13, g_timeOffsetList());
	mg.addLineEditor(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout);

	mg.addStretch();
	return widget;
}

QWidget * DcTimeSyncForm::createSNTPv4()
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager mg(controller(), layout);	
	
	mg.addLineEditor(SP_IP4_PEER_ADDR, 0, Text::SNTP::IP4_PEER_ADDR0);
	mg.addLineEditor(SP_IP4_PEER_ADDR, 1, Text::SNTP::IP4_PEER_ADDR1);
	mg.addLineEditor(SP_IPPORT, 0, Text::SNTP::IPPORT0);
	mg.addLineEditor(SP_IPPORT, 1, Text::SNTP::IPPORT1);

	auto editor = mg.addLineEditor(SP_VLAN_TAG, 7, Text::VLAN_TAG7);
	if (editor)
		editor->setReadOnly(true);
	editor = mg.addLineEditor(SP_VLAN_PRIO, 7, Text::VLAN_PRIO7);
	if (editor)
		editor->setReadOnly(true);

	int profile = 1;
	int idxBase = profile * PROFILE_SIZE;
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 0, Text::SNTP::TIMESYNCROPARS0);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 1, Text::SNTP::TIMESYNCROPARS1);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 2, Text::SNTP::TIMESYNCROPARS2);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 3, Text::SNTP::TIMESYNCROPARS3);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 4, Text::SNTP::TIMESYNCROPARS4);
	mg.addLineEditor(SP_TIMESYNCROPARS, idxBase + 5, Text::SNTP::TIMESYNCROPARS5);
	if (!m_hasGlobalOffsetParam)
		mg.addListEditor(SP_TIMESYNCROPARS, idxBase + 6, Text::SNTP::TIMESYNCROPARS6, g_timeOffsetList());
	mg.addLineEditor(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout);

	mg.addStretch();
	return widget;
}

QWidget * DcTimeSyncForm::create1PPS()
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager mg(controller(), layout);

	int idx = 0;	
	QString label = Text::_1PPS::EDGE;	
	if (!mg.addListEditor(SP_GBS_ENTITY_STATUS, idx, label, g_PolarityList))
		mg.addListEditor(SP_1PPS_EDGE, idx, label, g_PolarityList);

	idx = 0;
	label = Text::_1PPS::DELAY;
	if (!mg.addLineEditor(SP_SM_TIMEDIFSUMM, idx, label))
		mg.addLineEditor(SP_1PPS_DELAY, idx, label);	


	auto ppsProtocolList = supportedProtocols(controller());
    ppsProtocolList.exclude({ DcController::Protocol_IEEE1588v2_UDP, DcController::Protocol_IEEE1588v2_802_3, DcController::Protocol_1PPS });
	mg.addListEditor(SP_PROTOCOL, 3, Text::_1PPS::PROTOCOL3, ppsProtocolList);
	mg.addLineEditor(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout, Text::TimeoutDescription);

	mg.addStretch();
	return widget;
}

QWidget * DcTimeSyncForm::createDefault()
{
	QWidget *widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	EditorsManager mg(controller(), layout);
	mg.addLineEditor(SP_PROTOCOL, INDEX_TIMEOUT, Text::Timeout, Text::TimeoutDescription);

	mg.addStretch();
	return widget;
}
