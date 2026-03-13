#pragma once

#include <qdialog.h>

#include <dpc/gui/widgets/ComboBox.h>
#include <dpc/gui/widgets/SpinBox.h>

#include <file_managers/DcAnalogsConversionFileManager.h>

class CustomConversionDialog : public QDialog
{
	Q_OBJECT

public:
    explicit CustomConversionDialog(DcController *device, uint16_t conversionIdx, QWidget *parent = 0);

	DcController* device() const { return m_device; }
	uint16_t idx() const { return m_idx; }

private slots:
	void onOkButton();

private:
	void load();
	bool save();

	QByteArray dataFromFile() const;

private:
	DcController* m_device;
	uint16_t m_idx;

	Dpc::Gui::ComboBox* m_conversionTypeComboBox;
	Dpc::Gui::DoubleSpinBox* m_inputLowSpinBox;
	Dpc::Gui::DoubleSpinBox* m_inputHighSpinBox;
	QWidget* m_currentSettingsWidget;

	DcAnalogsConversionFileManager m_fm;
};

