#pragma once

#include <QWidget>
#include "ui_IModbusWidgetFunction.h"
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandDefines.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

class IModbusWidgetFunction : public QWidget
{
	Q_OBJECT

public:
	IModbusWidgetFunction(QWidget *parent = Q_NULLPTR);
	~IModbusWidgetFunction();

	void updateFromFunction(spModbusFunc_t function);

protected:
	Ui::IModbusWidgetFunction ui;
};
