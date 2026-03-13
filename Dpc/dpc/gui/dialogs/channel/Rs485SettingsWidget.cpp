#include "Rs485SettingsWidget.h"

#include <qlabel.h>
#include <qspinbox.h>
#include <qboxlayout.h>
#include <qcombobox.h>

#include <dpc/gui/dialogs/channel/TcpSettingsWidget.h>
#include <dpc/gui/dialogs/channel/VComSettingsWidget.h>

namespace Dpc::Gui
{
	using namespace Dpc::Sybus;

	Rs485SettingsWidget::Rs485SettingsWidget(const SettingsType& settings, QWidget* parent) :
		ISettingsWidget(parent),		
		m_settings(settings),
		m_rs485PortSpin(new QSpinBox(this)),
		m_rs485DeviceSpin(new QSpinBox(this)),
		m_baseLevelComboBox(new QComboBox),
		m_baseLevelWidget(new QWidget)
	{
		m_rs485PortSpin->setRange(0, USHRT_MAX);
		m_rs485PortSpin->setValue(settings.value(Rs485PortSetting).toUInt());
		m_rs485PortSpin->setToolTip("Номер RS485 порта через которое будет устанавливаться соединение");
		m_rs485DeviceSpin->setRange(0, USHRT_MAX);
		m_rs485DeviceSpin->setValue(settings.value(Rs485DeviceSetting).toUInt());
		m_rs485DeviceSpin->setToolTip("Адрес устройства на выбраном порту");

		m_baseLevelComboBox->addItem("TCP/IP", RS485vTCP);
		m_baseLevelComboBox->addItem("VCOM", RS485vVCOM);
		m_baseLevelComboBox->setCurrentIndex(-1);	
		connect(m_baseLevelComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Rs485SettingsWidget::onBaseLevelComboBoxChanged);

		m_baseLevelWidget->setLayout(new QHBoxLayout);
		QGridLayout* layout = new QGridLayout(this);
		int row = 1;
		layout->addWidget(new QLabel("Порт"), row, 0);
		layout->addWidget(m_rs485PortSpin, row, 1);
		row++;

		layout->addWidget(new QLabel("Устройство"), row, 0);
		layout->addWidget(m_rs485DeviceSpin, row, 1);
		row++;

		layout->addWidget(new QLabel("Через"), row, 0);
		layout->addWidget(m_baseLevelComboBox, row, 1);
		row++;
		
		layout->addWidget(m_baseLevelWidget, row, 0, 1, 2);
		//row++;

		layout->setRowStretch(0, 1);
		layout->setRowStretch(layout->rowCount(), 1);
		//layout->setColumnStretch(layout->columnCount(), 1);
		
		auto baseType = settings.value(TypeSetting);
		for(size_t i = 0; i < m_baseLevelComboBox->count(); i++)
			if (baseType == m_baseLevelComboBox->itemData(i).toUInt()) {
				m_baseLevelComboBox->setCurrentIndex(i);
				break;
			}

		if (m_baseLevelComboBox->currentIndex() < 0)
			m_baseLevelComboBox->setCurrentIndex(0);
	}

	SettingsType Rs485SettingsWidget::settings() const
	{
		SettingsType res;

		if (auto w = m_baseLevelWidget->findChild<ISettingsWidget*>(); w) {
			res.insert(w->settings());
			res[TypeSetting] = static_cast<ChannelType>(m_baseLevelComboBox->currentData().toUInt());
		}
		else {

		}
		
		res[Rs485PortSetting] = m_rs485PortSpin->value();
		res[Rs485DeviceSetting] = m_rs485DeviceSpin->value();
		return res;
	}

	QString Rs485SettingsWidget::error() const
	{

		QString error;
		if (auto w = m_baseLevelWidget->findChild<ISettingsWidget*>(); w)
			error = w->error();

		return error;
	}

	void Rs485SettingsWidget::onBaseLevelComboBoxChanged()
	{
		for (auto child : m_baseLevelWidget->findChildren<ISettingsWidget*>(QString(), Qt::FindDirectChildrenOnly))
			delete child;

		QWidget* newWidget = nullptr;
		switch (m_baseLevelComboBox->currentData().toUInt())
		{
		case RS485vTCP: newWidget = new TcpSettingsWidget(m_settings); break;
		case RS485vVCOM: newWidget = new VComSettingsWidget(m_settings); break;
		default: break;
		}

		if (!newWidget)
			return;

		m_baseLevelWidget->layout()->addWidget(newWidget);
	}
} // namespace