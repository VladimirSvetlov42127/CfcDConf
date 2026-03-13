#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

ModbusFunction::ModbusFunction()
	: QObject(nullptr), m_functionCode(ModbusFunctions_ns::Unknown), m_startAddress(0), m_count(0)
	, m_dataByteLen(0)
{
}

ModbusFunction::ModbusFunction(ModbusFunctions_ns::modbusFunctionType_t fuctionCode)
	: QObject(nullptr), m_functionCode(fuctionCode), m_startAddress(0), m_count(0)
	, m_dataByteLen(0)
{

}

ModbusFunction::ModbusFunction(ModbusFunctions_ns::modbusFunctionType_t fuctionCode, ModbusMaster_ns::startAddress_t start, ModbusMaster_ns::count_t count, QObject *parent)
	: QObject(parent), m_functionCode(fuctionCode), m_startAddress(start), m_count(count)
	, m_dataByteLen(0)
{
}

ModbusFunction::~ModbusFunction()
{
}

bool ModbusFunction::setFunctionCode(ModbusFunctions_ns::modbusFunctionType_t code)
{
	m_functionCode = code;
	return true;
}

bool ModbusFunction::setStartAddress(ModbusMaster_ns::startAddress_t start)
{
	m_startAddress = start;
	return true;
}

bool ModbusFunction::setCount(ModbusMaster_ns::count_t count)
{
	m_count = count;
	return true;
}

bool ModbusFunction::setDataByte(uint16_t bytes)
{
	m_dataByteLen = bytes;
	return true;
}
