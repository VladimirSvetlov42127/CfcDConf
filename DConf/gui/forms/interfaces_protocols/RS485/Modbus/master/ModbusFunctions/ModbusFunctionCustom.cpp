#include "ModbusFunctionCustom.h"

ModbusFunctionCustom::ModbusFunctionCustom()
	: ModbusFunction()
{
}

ModbusFunctionCustom::ModbusFunctionCustom(modbusFunctionType_t customType)
	: ModbusFunction(customType)
{
}


ModbusFunctionCustom::ModbusFunctionCustom(startAddress_t start, count_t count, QObject * parent)
	: ModbusFunction(ModbusFunctions_ns::Custom, start, count, parent)
{
}

ModbusFunctionCustom::~ModbusFunctionCustom()
{
}

bool ModbusFunctionCustom::parseFromBuffer(const QByteArray & buffer) {
	return setRequestArray(buffer);
}

bool ModbusFunctionCustom::setRequestArray(const QByteArray & array) {
	if (array.isEmpty())
		return false;

	m_requestByteArray = array;	// Сохраняем массив байт
	
	// Преобразуем тип функции, считаем, что первый байтов будет код функции!
	ModbusFunctions_ns::modbusFunctionType_t type = static_cast<ModbusFunctions_ns::modbusFunctionType_t>(array[0]);
	setFunctionCode(type);	 

	return true; 
}
