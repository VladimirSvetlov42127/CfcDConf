#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctionWidgets/IModbusWidgetFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction04_ReadInputRegisters.h>

class ModbusWidgetFunction04_ReadInputRegisters : public IModbusWidgetFunction
{
	Q_OBJECT

public:
	ModbusWidgetFunction04_ReadInputRegisters(ModbusFunction04_ReadInputRegisters* function, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetFunction04_ReadInputRegisters();


private:
	ModbusFunction04_ReadInputRegisters* m_function;
};
