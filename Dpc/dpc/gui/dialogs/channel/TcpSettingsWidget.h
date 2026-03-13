#pragma once

#include <dpc/gui/dialogs/channel/ISettingsWidget.h>

QT_BEGIN_NAMESPACE
class QSpinBox;
QT_END_NAMESPACE

namespace Dpc::Gui
{
	class IpAddressWidget;

	class TcpSettingsWidget : public ISettingsWidget
	{
	public:
		TcpSettingsWidget(const Sybus::SettingsType &settings, QWidget *parent = nullptr);

		virtual Sybus::SettingsType settings() const override;
		virtual QString error() const override;

	private:
		Sybus::SettingsType m_settings;
		IpAddressWidget *m_ipWidget;
		QSpinBox *m_portSpin;
	};
} // namespace
