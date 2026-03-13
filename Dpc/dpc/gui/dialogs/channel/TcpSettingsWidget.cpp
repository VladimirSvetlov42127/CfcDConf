#include "TcpSettingsWidget.h"

#include <qlabel.h>
#include <qspinbox.h>
#include <qboxlayout.h>

#include <dpc/gui/widgets/IpAddressWidget.h>

namespace {
	const QString  DEFAULT_IP_ADDR = "192.168.0.12";
	const uint16_t DEFAULT_IP_PORT = 3000;
}

namespace Dpc::Gui
{
	using namespace Dpc::Sybus;

	TcpSettingsWidget::TcpSettingsWidget(const SettingsType& settings, QWidget* parent) :
		ISettingsWidget(parent),
		m_settings(settings),
		m_ipWidget(new IpAddressWidget(this)),
		m_portSpin(new QSpinBox(this))
	{
		QString ipAddr = settings.value(IpAddrSetting).toString();
		if (ipAddr.isEmpty())
			ipAddr = DEFAULT_IP_ADDR;
		m_ipWidget->setIpAddress(ipAddr);

		uint16_t port = settings.value(IpPortSetting).toUInt();
		if (!port)
			port = DEFAULT_IP_PORT;
		m_portSpin->setRange(1, USHRT_MAX);
		m_portSpin->setValue(port);

		QGridLayout* layout = new QGridLayout(this);
		int row = 1;
		layout->addWidget(new QLabel("IP-Адрес"), row, 0);
		layout->addWidget(m_ipWidget, row, 1, 1, 2);
		row++;

		layout->addWidget(new QLabel("Порт"), row, 0);
		layout->addWidget(m_portSpin, row, 1);
		//row++;


		layout->setRowStretch(0, 1);
		layout->setRowStretch(layout->rowCount(), 1);
		layout->setColumnStretch(layout->columnCount(), 1);
	}

	SettingsType TcpSettingsWidget::settings() const
	{
		auto res = m_settings;
		res[TypeSetting] = TCP;
		res[IpAddrSetting] = m_ipWidget->ipAddreess();
		res[IpPortSetting] = m_portSpin->value();
		return res;
	}

	QString TcpSettingsWidget::error() const
	{
		QStringList parts = m_ipWidget->ipAddreess().split('.');
		for (auto& p : parts) {
			bool isEmpty = p.isEmpty();
			bool isNum;
			auto v = p.toUInt(&isNum);
			if (isEmpty || !isNum || v > UCHAR_MAX)
				return "IP-Адрес задан некорректно";
		}

		return QString();
	}
} // namespace