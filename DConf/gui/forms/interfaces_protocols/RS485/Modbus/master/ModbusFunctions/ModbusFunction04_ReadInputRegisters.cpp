#include "ModbusFunction04_ReadInputRegisters.h"

ModbusFunction04_ReadInputRegisters::ModbusFunction04_ReadInputRegisters() 
	: ModbusFunction(ModbusFunctions_ns::ReadInputRegisters)
{
}

ModbusFunction04_ReadInputRegisters::ModbusFunction04_ReadInputRegisters(ModbusMaster_ns::startAddress_t start, ModbusMaster_ns::count_t count, QObject *parent)
	: ModbusFunction(ModbusFunctions_ns::ReadInputRegisters, start, count, parent)
{
}

ModbusFunction04_ReadInputRegisters::~ModbusFunction04_ReadInputRegisters()
{
}

const QByteArray & ModbusFunction04_ReadInputRegisters::getModbusRequestByteArray()
{
	m_requestByteArray.clear();

	m_requestByteArray.append(getFunctionType());

	uint16_t start(getStartAddress());
	uint8_t highByte = start >> 8;
	uint8_t lowByte = start & 0xFF;
	m_requestByteArray.append(highByte);
	m_requestByteArray.append(lowByte);

	uint16_t registerCount(getModbusRegisterCount());	// Высчитываем кол-во регистров
	highByte = registerCount >> 8;
	lowByte = registerCount & 0xFF;		
	m_requestByteArray.append(highByte);
	m_requestByteArray.append(lowByte);

	return m_requestByteArray;
}

bool ModbusFunction04_ReadInputRegisters::parseFromBuffer(const QByteArray & buffer)
{
	if (buffer.size() < function04_byteSize)
		return false;

	uint16_t pos = 0;
	uint8_t code = buffer[pos++];
	if (code != ModbusFunctions_ns::ReadInputRegisters)
		return false;

	uint8_t highByte = buffer[pos++];
	uint8_t lowByte = buffer[pos++];
	uint16_t start = (highByte << 8) + lowByte;
	setStartAddress(start);

	highByte = buffer[pos++];
	lowByte = buffer[pos++];
	uint16_t RegisterCount = (highByte << 8) + lowByte;	// Считывается кол-во регистров!
	   
	return true;
}

uint8_t ModbusFunction04_ReadInputRegisters::getExpectedBcnt() const
{
	uint8_t functionCodeByte = 1;
	uint8_t byteCountOfValues = 1;

	uint8_t dataBytes = (getDataBytes() == 0) ? 8 : getDataBytes();
	uint8_t bytesForValues = dataBytes * getCount(); // Общее кол-во байт для передачи всех параметров указанного размера
	uint8_t countAll = functionCodeByte + byteCountOfValues + bytesForValues;
	return countAll;
}

QByteArray ModbusFunction04_ReadInputRegisters::getExpectedFirstRspBcnt() const
{
	QByteArray arr;
	arr.append(ModbusFunctions_ns::ReadInputRegisters);
	uint8_t dataBytes = (getDataBytes() == 0) ? 8 : getDataBytes();
	uint8_t bytesForValues = dataBytes * getCount();	// Общее кол-во байт для передачи всех параметров указанного размера
	arr.append(bytesForValues);
	return arr;
}

uint16_t ModbusFunction04_ReadInputRegisters::getModbusRegisterCount() const
{
	uint16_t count(getCount());
	uint16_t dataBytes(getDataBytes());
	if (dataBytes == 0)	// Нулем обозначают 8 байт
		dataBytes = 8;

	uint16_t registerCount = 0;
	if (MODBUS_REGISTER_BYTE_SIZE != dataBytes) {	// Если длина элемента в байтах не равна размеру регистра (2 байта)
		registerCount = (dataBytes % MODBUS_REGISTER_BYTE_SIZE == 0)
			? (count * dataBytes / MODBUS_REGISTER_BYTE_SIZE)
			: (count * dataBytes / MODBUS_REGISTER_BYTE_SIZE + 1); // Вычисляем кол-во регистров 
	}
	else {
		registerCount = count;
	}
	return registerCount;
}

//bool ModbusFunction04_ReadInputRegisters::setModbusRegisterCount(uint16_t registerCount)
//{
//	uint16_t dataBytes(getDataBytes());
//	if (dataBytes == 0)
//		dataBytes = 8;
//
//	if (registerCount == 0) {
//		setCount(0);
//		return true;
//	}
//
//	uint16_t count;
//	if (dataBytes % MODBUS_REGISTER_BYTE_SIZE == 0)
//		count = registerCount * MODBUS_REGISTER_BYTE_SIZE / dataBytes;
//	else
//		count = (registerCount - 1) * MODBUS_REGISTER_BYTE_SIZE / dataBytes;
//		
//	return setCount(count);
//}