#pragma once


#include "ModbusWidgetCommonCommand.h"


class ModbusWidgetCommandDins : public ModbusWidgetCommonCommand
{
	Q_OBJECT

public:
	ModbusWidgetCommandDins(spCommandDescr_t command, bool showAllParams = false, QWidget *parent = nullptr);
	~ModbusWidgetCommandDins();

};
