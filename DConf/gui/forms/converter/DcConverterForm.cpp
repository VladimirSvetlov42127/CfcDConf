#include "DcConverterForm.h"

#include <qtabwidget.h>
#include <qscrollarea.h>



#include <gui/editors/EditorsManager.h>

namespace {
	const quint16 PARAM_USART_SPEED = 0x0D30;
	const quint16 PARAM_USART_DATA = 0x0E30;
	const quint16 PARAM_USART_PARITY = 0x0F30;
	const quint16 PARAM_USART_STOPBITS = 0x1030;

	const quint16 PARAM_USART_FORMAT = 0x1130;
	const quint16 PARAM_USART_DELAY = 0x1230;
	const quint16 PARAM_USART_TIMEOUT_END = 0x1330;
	const quint16 PARAM_USART_TIMEOUT = 0x1430;

	const quint16 PARAM_ETHERNET_TIMEOUT = 0x1930;
	const quint16 PARAM_ETHERNET_TRANSPORT = 0x1730;

	const quint16 PARAM_IPADDRESS = SP_IP4_ADDR;

	const quint16 PARAM_UDP_LOCAL_PORT = 0x1A30;
	const quint16 PARAM_UDP_REMOTE_IPADDRESS = 0x1B30;
	const quint16 PARAM_UDP_REMOTE_PORT = 0x1C30;
	const quint16 PARAM_UDP_SAME_INPUT_OUTPUT_ADDRESS = 0x2130;

	const quint16 PARAM_TCP_CLIENT_SERVER = 0x1830;
	const quint16 PARAM_TCP_LOCAL_PORT = 0x1D30;
	const quint16 PARAM_TCP_REMOTE_IPADDRESS = 0x1E30;
	const quint16 PARAM_TCP_REMOTE_PORT = 0x1F30;

	const quint16 PARAM_TIMESYNC_RS485 = 0x2030;

	const ListEditorContainer g_UsartSpeedList = { { "1200", "2400", "4800", "9600", "19200", "38400", "115200", "307200" }, true };
	const ListEditorContainer g_UsartParityList = { "без паритета", "чет", "нечет" };
	const ListEditorContainer g_UsartFormatList = { { "SYBUS", "MODBUS RTU", "MODBUS TCP в MODBUS RTU", "IEC-101" }, true };
	const ListEditorContainer g_EthernetTransportList = { "UDP", "TCP" };
	const ListEditorContainer g_TcpClientServerList = { "Сервер", "Клиент" };

	const ListEditorContainer g_UsartDataList = { { "7 бит", 7 }, { "8 бит", 8 }, { "9 бит", 9 } };
	const ListEditorContainer g_UsartStopsList = { { "1 бит", 1 }, { "2 бита", 2 } };
}

DcConverterForm::DcConverterForm(DcController *controller) :
	DcForm(controller,  "Конвертер Ethernet-RS485", false)
{
	QVBoxLayout *vbox = new QVBoxLayout(centralWidget());

	QTabWidget *mainTabWidget = new QTabWidget;
	vbox->addWidget(mainTabWidget);

    auto usartParamSpeed = controller->paramsRegistry().parameter(PARAM_USART_SPEED);
	if (!usartParamSpeed)
		return;

    size_t channelsCount = usartParamSpeed->elementsCount();
	for (size_t i = 0; i < channelsCount; i++) {
		QWidget *tab = new QWidget;
		QGridLayout *layout = new QGridLayout(tab);

		QScrollArea *tabArea = new QScrollArea;
		tabArea->setWidgetResizable(true);
		tabArea->setFrameShape(QFrame::NoFrame);
		tabArea->setWidget(tab);

		mainTabWidget->addTab(tabArea, QString("Канал %1").arg(i + 1));		

		QGroupBox *usartBox = new QGroupBox("USART");
		QVBoxLayout *usartBoxLayout = new QVBoxLayout(usartBox);
		layout->addWidget(usartBox, 0, 0, 1, 2);		

		// настройка фрейма
		QGroupBox *usartFrameBox = new QGroupBox("Настройка фрейма");
		usartBoxLayout->addWidget(usartFrameBox);
		QGridLayout *usartFrameLayout = new QGridLayout(usartFrameBox);
		
		EditorsManager mg(controller, usartFrameLayout);
		mg.addListEditor(PARAM_USART_SPEED, i, "Скорость", g_UsartSpeedList);
		mg.addListEditor(PARAM_USART_DATA, i, "Данные", g_UsartDataList);
		mg.addListEditor(PARAM_USART_PARITY, i, "Паритет", g_UsartParityList);
		mg.addListEditor(PARAM_USART_STOPBITS, i, "Стоп", g_UsartStopsList);
		mg.addHorizontalStretch();

		// настройка пакета
		QGroupBox *usartPacketBox = new QGroupBox("Настройка пакета");
		usartBoxLayout->addWidget(usartPacketBox);
		QGridLayout *usartPacketLayout = new QGridLayout(usartPacketBox);

		mg.setLayout(usartPacketLayout);
		mg.addListEditor(PARAM_USART_FORMAT, i, "Формат пакета", g_UsartFormatList);
		mg.addLineEditor(PARAM_USART_DELAY, i, "Задержка перед передачей пакета (мс)");
		mg.addLineEditor(PARAM_USART_TIMEOUT_END, i, "Таймаут конца пакета (мс)");
		mg.addLineEditor(PARAM_USART_TIMEOUT, i, "Таймаут (мс)");
		mg.addHorizontalStretch();

		// настройка Ethernet
		QGroupBox *ethernetBox = new QGroupBox("ETHERNET");
		layout->addWidget(ethernetBox, 1, 0, 1, 2);
		QGridLayout *ethernetBoxLayout = new QGridLayout(ethernetBox);
		ethernetBoxLayout->setColumnStretch(2, 1);

		mg.setLayout(ethernetBoxLayout);
		mg.addLineEditor(PARAM_ETHERNET_TIMEOUT, i, "Таймаут (мс)");
		auto transportEditor = mg.addListEditor(PARAM_ETHERNET_TRANSPORT, i, "Транспорт", g_EthernetTransportList);

		QGroupBox *udpSettings = new QGroupBox("Настройка UDP");
		QGroupBox *tcpSettings = new QGroupBox("Настройка TCP");
		auto onTransportChanged = [udpSettings, tcpSettings](const QString &value) {
			bool isTcp = value.toInt();
			udpSettings->setVisible(!isTcp);
			tcpSettings->setVisible(isTcp);
		};
		connect(transportEditor, &ParamEditor::valueChanged, this, onTransportChanged);

		ethernetBoxLayout->addWidget(udpSettings, ethernetBoxLayout->rowCount(), 0, 1, 3);
		ethernetBoxLayout->addWidget(tcpSettings, ethernetBoxLayout->rowCount(), 0, 1, 3);
		onTransportChanged(transportEditor->param()->value());

		QGridLayout *udpLayout = new QGridLayout(udpSettings);
		udpLayout->setColumnStretch(3, 1);

		QGroupBox *udpInputDataBox = new QGroupBox("Входящие данные");
		QGroupBox *udpOutputDataBox = new QGroupBox("Исходящие данные");		
		udpLayout->addWidget(udpInputDataBox, 0, 0);
		udpLayout->addWidget(udpOutputDataBox, 0, 1);

        auto udpSameAddressEditor = new CheckParamEditor(controller->paramsRegistry().element(PARAM_UDP_SAME_INPUT_OUTPUT_ADDRESS, i));
		udpLayout->addWidget(new QLabel("Для исходящих данных использовать IP-адрес: порт источника UDP-запроса"), udpLayout->rowCount(), 0, 1, 2);
		udpLayout->addWidget(udpSameAddressEditor, udpLayout->rowCount() - 1, 2);

		auto onUdpSameAddressChanged = [udpOutputDataBox, udpLayout](const QString &val) {
			udpOutputDataBox->setEnabled(!val.toInt());
		};
		connect(udpSameAddressEditor, &ParamEditor::valueChanged, this, onUdpSameAddressChanged);
		onUdpSameAddressChanged(udpSameAddressEditor->param()->value());

		QGridLayout *udpInputDataBoxLayout = new QGridLayout(udpInputDataBox);	
		mg.setLayout(udpInputDataBoxLayout);
		auto udpIpAddressEditor = mg.addLineEditor(PARAM_IPADDRESS, 0, "IP-адрес");
		udpIpAddressEditor->setEnabled(false);
		mg.addLineEditor(PARAM_UDP_LOCAL_PORT, i, "IP-порт");

		QGridLayout* udpOutputDataBoxLayout = new QGridLayout(udpOutputDataBox);
		mg.setLayout(udpOutputDataBoxLayout);
		mg.addLineEditor(PARAM_UDP_REMOTE_IPADDRESS, i, "IP-адрес");
		mg.addLineEditor(PARAM_UDP_REMOTE_PORT, i, "IP-порт");

		QGridLayout *tcpLayout = new QGridLayout(tcpSettings);
		
		mg.setLayout(tcpLayout);
		auto tcpClientServer = mg.addListEditor(PARAM_TCP_CLIENT_SERVER, i, "Клиент/Сервер", g_TcpClientServerList);
		QGroupBox *inputConnection = new QGroupBox("Входящие соединения");
		QGroupBox *outputConnection = new QGroupBox("Исходящие соединения");
		tcpLayout->addWidget(inputConnection, tcpLayout->rowCount(), 0, 1, 3);
		tcpLayout->addWidget(outputConnection, tcpLayout->rowCount(), 0, 1, 3);
		tcpLayout->setColumnStretch(tcpLayout->columnCount(), 1);

		auto onTcpClientServerChanged = [inputConnection, outputConnection](const QString &value) {
			bool isClient = value.toInt();
			inputConnection->setVisible(!isClient);
			outputConnection->setVisible(isClient);
		};
		connect(tcpClientServer, &ParamEditor::valueChanged, this, onTcpClientServerChanged);
		onTcpClientServerChanged(tcpClientServer->param()->value());

		QGridLayout *inputConnectionLayout = new QGridLayout(inputConnection);
		inputConnectionLayout->setColumnStretch(2, 1);

		mg.setLayout(inputConnectionLayout);
		auto editor = mg.addLineEditor(PARAM_IPADDRESS, 0, "IP-адрес");
		editor->setEnabled(false);
		mg.addLineEditor(PARAM_TCP_LOCAL_PORT, i, "IP-порт");

		QGridLayout *outputConnectionLayout = new QGridLayout(outputConnection);
		outputConnectionLayout->setColumnStretch(2, 1);

		mg.setLayout(outputConnectionLayout);
		mg.addLineEditor(PARAM_TCP_REMOTE_IPADDRESS, 0, "IP-адрес");
		mg.addLineEditor(PARAM_TCP_REMOTE_PORT, i, "IP-порт");

		mg.setLayout(layout);
		mg.addCheckEditor(PARAM_TIMESYNC_RS485, i, "Подержка синхронизации времени по RS485");
		mg.addStretch();
	}
}

bool DcConverterForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{PARAM_USART_DATA}
	};

	return hasAny(controller, params);
}

void DcConverterForm::fillReport(DcIConfigReport * report)
{
}
