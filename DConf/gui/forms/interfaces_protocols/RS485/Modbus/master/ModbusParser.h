#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

class ModbusParser
{
public:
	ModbusParser();
	~ModbusParser();

	spModbusFunc_t createFunctionFromBufAndCommand(const QByteArray & functionBuffer, spCommandDescr_t command);
	spModbusFunc_t createFunctionFromBuf(const QByteArray & functionBuffer);
};
