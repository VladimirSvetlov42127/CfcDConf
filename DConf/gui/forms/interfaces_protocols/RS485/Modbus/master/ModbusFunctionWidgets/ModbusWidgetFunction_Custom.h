#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctionWidgets/IModbusWidgetFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunctionCustom.h>

class ModbusWidgetFunction_Custom : public IModbusWidgetFunction
{
	Q_OBJECT

public:
	ModbusWidgetFunction_Custom(ModbusFunctionCustom *function, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetFunction_Custom();

	bool build();

private:
	QLineEdit *m_lEdBuffer;
	ModbusFunctionCustom *m_function;

public slots:
	void slot_buffer_editFinished();
};
