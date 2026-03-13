#include "ModbusWidgetFunction04_ReadInputRegisters.h"

ModbusWidgetFunction04_ReadInputRegisters::ModbusWidgetFunction04_ReadInputRegisters(ModbusFunction04_ReadInputRegisters* function, QWidget *parent)
	: IModbusWidgetFunction(parent),
	m_function(function)
{
	ui.lEdFunctionCode->setText(QString::number(m_function->getFunctionType()));
	ui.lEdStartAddress->setText(QString::number(m_function->getStartAddress()));
	ui.lEdCount->setText(QString::number(m_function->getModbusRegisterCount()));

	// Переименование начального адреса в регистр
	ui.labStartAddress->setText("Начальный регистр, 2 байта");
	ui.labCount->setText("Количество регистров, 2 байта");
}

ModbusWidgetFunction04_ReadInputRegisters::~ModbusWidgetFunction04_ReadInputRegisters()
{
}

