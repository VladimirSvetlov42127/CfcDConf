#pragma once

#include <gui/forms/functions/analogs/conversion/ICustomConversionSettingsWidget.h>

#include <dpc/gui/widgets/TableView.h>

class LinearPiecewiseSettingsWidget : public ICustomConversionSettingsWidget
{
	Q_OBJECT

public:
    explicit LinearPiecewiseSettingsWidget(const QByteArray &data, QWidget *parent = 0);

	virtual uint16_t itemsCount() const override;
	virtual QByteArray settingsData() const override;

private slots:
	void onAddButton();
	void onRemoveButton();
	void onFromFileButton();

private:
	Dpc::Gui::TableView* m_view;
};

