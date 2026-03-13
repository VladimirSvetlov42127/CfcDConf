#include "ModbusWidgetFunction02_ReadDiscreteInputs.h"

ModbusWidgetFunction02_ReadDiscreteInputs::ModbusWidgetFunction02_ReadDiscreteInputs(ModbusFunction02_ReadDiscreteInputs* function, QWidget *parent)
	: IModbusWidgetFunction(parent),
	m_function(function)
{
	if (m_function) {
		ui.lEdFunctionCode->setText(QString::number(m_function->getFunctionType()));
		ui.lEdStartAddress->setText(QString::number(m_function->getStartAddress()));
		ui.lEdCount->setText(QString::number(m_function->getCount()));
	}

}

ModbusWidgetFunction02_ReadDiscreteInputs::~ModbusWidgetFunction02_ReadDiscreteInputs()
{
}


