#include "ModbusWidgetCommandDins.h"

ModbusWidgetCommandDins::ModbusWidgetCommandDins(spCommandDescr_t command, bool isModifyModbus, QWidget *parent)
	: ModbusWidgetCommonCommand(command, isModifyModbus, parent)
{
	if (isModifyModbus == false) {	// Если это не модифицированный модбас, показывать все поля не нужно

		setDataBytesVisible(m_isModifyModbus);
		setDataOrderVisible(m_isModifyModbus);
		setUpdAlgVisible(m_isModifyModbus);
		setExpectedBcntVisible(m_isModifyModbus);
		setAnalizedRspBcntVisible(m_isModifyModbus);
		setExpectedFirstRspBcntVisible(m_isModifyModbus);

		ui.formLayoutForCommand->addRow(new QLabel("Настраивается автоматически в зависимости от функции Модбаса"));
	}			
	
}

ModbusWidgetCommandDins::~ModbusWidgetCommandDins()
{
}
