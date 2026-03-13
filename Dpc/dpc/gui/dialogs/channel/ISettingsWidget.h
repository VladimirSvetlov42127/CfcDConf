#pragma once

#include <qwidget.h>

#include <dpc/sybus/channel/Common.h>

namespace Dpc::Gui
{
	class ISettingsWidget : public  QWidget
    {
        Q_OBJECT
	public:
		ISettingsWidget(QWidget *parent = nullptr) : QWidget(parent) {}

		virtual Sybus::SettingsType settings() const = 0;
		virtual QString error() const = 0;
	};
} // namespace
