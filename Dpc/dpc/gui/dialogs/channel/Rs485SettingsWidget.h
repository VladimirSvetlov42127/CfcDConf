#pragma once

#include <dpc/gui/dialogs/channel/ISettingsWidget.h>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QComboBox;
QT_END_NAMESPACE

namespace Dpc::Gui
{
	class Rs485SettingsWidget : public ISettingsWidget
	{
	public:
		Rs485SettingsWidget(const Sybus::SettingsType &settings, QWidget *parent = nullptr);

		virtual Sybus::SettingsType settings() const override;
		virtual QString error() const override;

	private slots:
		void onBaseLevelComboBoxChanged();

	private:
		Sybus::SettingsType m_settings;

		QSpinBox *m_rs485PortSpin;
		QSpinBox *m_rs485DeviceSpin;

		QComboBox* m_baseLevelComboBox;
		QWidget* m_baseLevelWidget;
	};
} // namespace
