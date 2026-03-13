#include "ModbusFunction02_ReadDiscreteInputs.h"

ModbusFunction02_ReadDiscreteInputs::ModbusFunction02_ReadDiscreteInputs()
	: ModbusFunction(ModbusFunctions_ns::ReadDiscreteInputs)
{
}

ModbusFunction02_ReadDiscreteInputs::ModbusFunction02_ReadDiscreteInputs(ModbusMaster_ns::startAddress_t start, ModbusMaster_ns::count_t count, QObject *parent)
	: ModbusFunction(ModbusFunctions_ns::ReadDiscreteInputs, start, count, parent)
{	
}

ModbusFunction02_ReadDiscreteInputs::~ModbusFunction02_ReadDiscreteInputs()
{
}


const QByteArray & ModbusFunction02_ReadDiscreteInputs::getModbusRequestByteArray() 
{
	m_requestByteArray.clear();

	m_requestByteArray.append(getFunctionType());

	uint16_t start(getStartAddress());
	uint8_t highByte = start >> 8;
	uint8_t lowByte = start & 0xFF;
	m_requestByteArray.append(highByte);
	m_requestByteArray.append(lowByte);

	uint16_t count(getCount());
	highByte = count >> 8;
	lowByte = count & 0xFF;
	m_requestByteArray.append(highByte);
	m_requestByteArray.append(lowByte);

	return m_requestByteArray;
}


bool ModbusFunction02_ReadDiscreteInputs::parseFromBuffer(const QByteArray &buffer) {

	if (buffer.size() < function02_byteSize)
		return false;

	uint16_t pos = 0;	
	uint8_t code = buffer[pos++];
	if (code != ModbusFunctions_ns::ReadDiscreteInputs)
		return false;

	uint8_t highByte = buffer[pos++];
	uint8_t lowByte = buffer[pos++];
	uint16_t start = (highByte << 8) + lowByte;
	setStartAddress(start);

	highByte = buffer[pos++];
	lowByte = buffer[pos++];
	uint16_t count = (highByte << 8) + lowByte;
	setCount(count);

	return true;
}

uint8_t ModbusFunction02_ReadDiscreteInputs::getExpectedBcnt() const
{
	uint8_t channelCount = getCount();	// Получаем кол-во запрашиваемых каналов
	uint8_t functionCodeByte = 1;
	uint8_t channelCountByte = 1;

	// Если запрашиваемое кол-во каналов кратно 8 (помещаеются в целое число байт), то пишем это число, если не делится, прибавляем еще один байт
	uint8_t byteForChannels = (channelCount % 8 == 0) ? (channelCount / 8) : (channelCount / 8 + 1);
	uint8_t countAll = functionCodeByte + channelCountByte + byteForChannels;
	return countAll;
}

QByteArray ModbusFunction02_ReadDiscreteInputs::getExpectedFirstRspBcnt() const
{	
	// Проверяем два байта: код функции и кол-во требуемых в ответе байт
	QByteArray arr;
	arr.append(ModbusFunctions_ns::ReadDiscreteInputs);

	uint8_t channelCount = getCount();	// Получаем кол-во запрашиваемых каналов
	uint8_t byteForChannels = (channelCount % 8 == 0) ? (channelCount / 8) : (channelCount / 8 + 1);
	arr.append(byteForChannels);
	return arr;
}
