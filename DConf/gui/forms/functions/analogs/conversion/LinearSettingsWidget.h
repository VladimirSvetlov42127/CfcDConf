#pragma once

#include <gui/forms/functions/analogs/conversion/ICustomConversionSettingsWidget.h>

#include <dpc/gui/widgets/SpinBox.h>

class LinearSettingsWidget : public ICustomConversionSettingsWidget
{
	Q_OBJECT

public:
    explicit LinearSettingsWidget(const QByteArray &data, QWidget *parent = 0);

	virtual uint16_t itemsCount() const override;
	virtual QByteArray settingsData() const override;

private:
	Dpc::Gui::DoubleSpinBox* m_k0SpinBox;
	Dpc::Gui::DoubleSpinBox* m_k1SpinBox;
};

