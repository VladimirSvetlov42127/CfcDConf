#pragma once

#include <dpc/gui/dialogs/channel/ISettingsWidget.h>

namespace Dpc::Gui
{
	class VComSettingsWidget : public ISettingsWidget
	{
	public:
		VComSettingsWidget(const Sybus::SettingsType &settings, QWidget *parent = nullptr);

		virtual Sybus::SettingsType settings() const override;
		virtual QString error() const override;

	private:
		Sybus::SettingsType m_settings;
		int m_port;
		QString m_port_name;
	};
} // namespace