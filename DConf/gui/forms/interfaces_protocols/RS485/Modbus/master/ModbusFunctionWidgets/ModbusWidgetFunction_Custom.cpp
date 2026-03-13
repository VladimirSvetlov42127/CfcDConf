#include "ModbusWidgetFunction_Custom.h"
#include <qformlayout.h>
#include <qlineedit.h>
#include <qlabel.h>

ModbusWidgetFunction_Custom::ModbusWidgetFunction_Custom(ModbusFunctionCustom *function, QWidget *parent)
	: IModbusWidgetFunction(parent), 
	m_function(function)
{
	build();
}

ModbusWidgetFunction_Custom::~ModbusWidgetFunction_Custom()
{
}

bool ModbusWidgetFunction_Custom::build()
{

	// Создаем новый виджет
	QFormLayout *formLayout = ui.formLayout_2;
	int count = formLayout->rowCount();
	for (int i = count - 1; i >= 0; i--) {
		formLayout->removeRow(i);
	}
	   
	m_lEdBuffer = new QLineEdit;
	m_lEdBuffer->setPlaceholderText("0A 1B 2C...");
	m_lEdBuffer->setMaximumWidth(350);

	if (!m_function)
		return false;

	QByteArray functionArray(m_function->getModbusRequestByteArray());

	if (!functionArray.isEmpty()) {
		QString functionArrayStr;
		QString templateFunc = "%1 ";

		for (int i = 0; i < functionArray.size(); i++) {
			uint8_t byte = functionArray.at(i);
			functionArrayStr += templateFunc.arg(byte, 2, 16, QChar('0'));
		}
		functionArrayStr = functionArrayStr.toUpper();
		m_lEdBuffer->setText(functionArrayStr);
	}


	formLayout->addRow(new QLabel("Введите команду: "), m_lEdBuffer);
	//this->setLayout(formLayout);	// Устанавливаем новый слой
	//old = formLayout;

	bool ok = connect(m_lEdBuffer, SIGNAL(editingFinished()), SLOT(slot_buffer_editFinished()));
	Q_ASSERT(ok);
	if (!ok) 
		return false;

	return true;
}


void ModbusWidgetFunction_Custom::slot_buffer_editFinished() {

	QByteArray tempFunctionArray;
	QString funcString(m_lEdBuffer->text());

	while (funcString.isEmpty() != true) {
		QString left(funcString.left(2));	// Берем первые 2 символа
		bool ok;
		uint8_t byte = left.toInt(&ok, 16);
		if (!ok) {
			m_lEdBuffer->setText("Ошибка ввода");
			return;
		}
		tempFunctionArray.append(byte);

		if (funcString.size() == 2) {	// Проверяем, после двух символов должнен идти пробел, если это не последняя пара
			funcString.remove(0, 2);	// Удаляем два символа
		}
		else {
			if(funcString.at(2) == ' ')
				funcString.remove(0, 3);	// Удаляем три символа
			else {
				m_lEdBuffer->setText("Ошибка ввода");
				return;
			}
				
		}
			
	}

	// Не проверяем на занятость!
	//if (ModbusFunctions_ns::isStandartFunctionCode(tempFunctionArray[0])) {
	//	m_lEdBuffer->setText("Код функции уже реализован");
	//} else 
		m_function->setRequestArray(tempFunctionArray);


}