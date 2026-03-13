#include "ModbusParser.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusFunctionsHeaders.h>

ModbusParser::ModbusParser()
{
}

ModbusParser::~ModbusParser()
{
}


spModbusFunc_t ModbusParser::createFunctionFromBufAndCommand(const QByteArray & functionBuffer, spCommandDescr_t command)
{
	if (functionBuffer.isEmpty())
		return nullptr;
	if (!command)
		return nullptr;

	uint8_t functionCode = functionBuffer[0];	// Первым байтом считаем, что всегда будет код функции!
	spModbusFunc_t function(nullptr);

	
	if (command->isSupportedFunctionCode(functionCode)) {
		switch (functionCode)
		{
		case ModbusFunctions_ns::ReadDiscreteInputs: {
			function = spModbusFunc_t(new ModbusFunction02_ReadDiscreteInputs);
			break;
		}
		case ModbusFunctions_ns::ReadInputRegisters: {
			function = spModbusFunc_t(new ModbusFunction04_ReadInputRegisters);
			break;
		}
		}
	}
	else {	// Если код не поддерживается в текущей команде, значит это пользовательский тип
		ModbusFunctions_ns::modbusFunctionType_t type = static_cast<ModbusFunctions_ns::modbusFunctionType_t>(functionCode);
		function = spModbusFunc_t(new ModbusFunctionCustom(type));
	}

	if (!function)
		return nullptr;

	if (!function->setDataByte(command->getDataBytes()))	// Нужно для правильного парсинга функции. 
		return nullptr;

	if (!function->parseFromBuffer(functionBuffer))
		return nullptr;

	return function;
}


spModbusFunc_t ModbusParser::createFunctionFromBuf(const QByteArray & functionBuffer)
{
	if (functionBuffer.isEmpty())
		return nullptr;

	uint8_t functionCode = functionBuffer[0];	// Первым байтом считаем, что всегда будет код функции!
	spModbusFunc_t function(nullptr);

	switch (functionCode)
	{
	case ModbusFunctions_ns::ReadDiscreteInputs: {
		function = spModbusFunc_t(new ModbusFunction02_ReadDiscreteInputs);
		break;
	}
	case ModbusFunctions_ns::ReadInputRegisters: {
		function = spModbusFunc_t(new ModbusFunction04_ReadInputRegisters);
		break;
	}
	default: {	// Если код не поддерживается в текущей команде, значит это пользовательский тип
		ModbusFunctions_ns::modbusFunctionType_t type = static_cast<ModbusFunctions_ns::modbusFunctionType_t>(functionCode);
		function = spModbusFunc_t(new ModbusFunctionCustom(type));

	}
	}

	if (!function)
		return nullptr;

	if (!function->parseFromBuffer(functionBuffer))
		return nullptr;

	return function;
}