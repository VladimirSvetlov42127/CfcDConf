#include "VComSettingsWidget.h"

#include <qlabel.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qboxlayout.h>
#include <qserialportinfo.h>
#include <qdebug.h>

namespace {
	const int gControlHeight = 24;
}

namespace Dpc::Gui
{
	using namespace Dpc::Sybus;
	
	VComSettingsWidget::VComSettingsWidget(const SettingsType& settings, QWidget* parent) :
		ISettingsWidget(parent),
		m_settings(settings),
		m_port(-1)
	{
		auto comPortBox = new QComboBox(this);
		comPortBox->setMinimumHeight(gControlHeight);
		comPortBox->setMinimumWidth(100);
		comPortBox->setMaximumHeight(gControlHeight);

		auto refreshCOMList = [comPortBox]() {
			comPortBox->clear();
			foreach(const QSerialPortInfo & info, QSerialPortInfo::availablePorts()) {
				auto text = info.portName();
				auto port = text.mid(3).toUInt();
				comPortBox->addItem(text, port);
			}
			if (comPortBox->count() > 0)
				comPortBox->setCurrentIndex(comPortBox->count() - 1);
		};

		refreshCOMList();
		QString currentPortName = settings.value(ComPortNameSetting).toString();
        //int currentPort = settings.value(ComPortSetting).toInt();
		comPortBox->setCurrentIndex(-1);
		for (size_t i = 0; i < comPortBox->count(); i++)
			if (comPortBox->itemText(i) == currentPortName) {
				comPortBox->setCurrentIndex(i);
				m_port = comPortBox->itemData(i).toUInt();
				m_port_name = currentPortName;
			}
		if (m_port_name.isEmpty()){
            int currentPort = settings.value(ComPortSetting).toInt();

			for (size_t i = 0; i < comPortBox->count(); i++)
				if (comPortBox->itemData(i).toUInt() == currentPort) {
					comPortBox->setCurrentIndex(i);
					m_port = currentPort;
					m_port_name = comPortBox->itemText(i);
				}
		}
		connect(comPortBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index) {
			if (index < 0)
			{
				m_port =  -1;
				m_port_name = "";
			}
			else
			{
				m_port =  comPortBox->itemData(index).toUInt();
				m_port_name =  comPortBox->itemText(index);
			}
			});

		QPushButton* refresh = new QPushButton("Обновить");
		refresh->setMinimumSize(90, gControlHeight);
		QObject::connect(refresh, &QPushButton::clicked, this, refreshCOMList);

		QGridLayout* layout = new QGridLayout(this);
		int row = 1;
		layout->addWidget(new QLabel("Порт"), row, 0);
		layout->addWidget(comPortBox, row, 1);
		layout->addWidget(refresh, row, 2);
		//row++;

		layout->setRowStretch(0, 1);
		layout->setRowStretch(layout->rowCount(), 1);
		layout->setColumnStretch(layout->columnCount(), 1);
	}

	SettingsType VComSettingsWidget::settings() const
	{
		auto res = m_settings;
		res[TypeSetting] = VCOM;
		res[ComPortSetting] = m_port;
		res[ComPortNameSetting] = m_port_name;
		return res;
	}

	QString VComSettingsWidget::error() const
	{
		if (m_port_name.isEmpty())
			return "Не выбран COM port";

		if (m_port < 0)
			return "Не выбран COM port (В старом режиме)";

		return QString();
	}
} // namespace
