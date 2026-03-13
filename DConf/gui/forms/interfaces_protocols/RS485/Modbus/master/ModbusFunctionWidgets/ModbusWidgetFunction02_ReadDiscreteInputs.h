#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctionWidgets/IModbusWidgetFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction02_ReadDiscreteInputs.h>

class ModbusWidgetFunction02_ReadDiscreteInputs : public IModbusWidgetFunction
{
	Q_OBJECT

public:
	ModbusWidgetFunction02_ReadDiscreteInputs(ModbusFunction02_ReadDiscreteInputs* function, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetFunction02_ReadDiscreteInputs();
	
private:
	ModbusFunction02_ReadDiscreteInputs* m_function;
};
