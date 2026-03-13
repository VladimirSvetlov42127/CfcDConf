#include "ModbusCommandAins.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction04_ReadInputRegisters.h>	// Как функция по умолчанию
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunctionCustom.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusParser.h>

ModbusCommandAins::ModbusCommandAins(bool isModifyModbus)
	: ModbusCommandDescription(ModbusMaster_ns::AinsInd, isModifyModbus),
	m_k0(0), m_k1(0)
{
	setDefaulValues();
}

ModbusCommandAins::ModbusCommandAins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent)
	: ModbusCommandDescription(ModbusMaster_ns::AinsInd, startChannel, count, isModifyModbus, parent),
	m_k0(0), m_k1(0)
{
	setDefaulValues();
}

ModbusCommandAins::ModbusCommandAins(const ModbusCommandAins & rhs)
{
	setCommandStruct(rhs.getCommandStruct());
	setCommandName(getCommandName());
}

ModbusCommandAins::~ModbusCommandAins()
{
}

ModbusCommandAins & ModbusCommandAins::operator=(const ModbusCommandAins & rhs)
{
	setCommandName(rhs.getCommandName());
	setCommandStruct(rhs.getCommandStruct());
	return *this;
}

void ModbusCommandAins::setDefaulValues()
{
	setOPR(ModbusCommand_ns::oprRead);
	setDataBytes(4);
		
	if (m_isModifyModbus) {	// Если модбас модифицированный, добавляем возможность создания функции через буфер обычный
		mv_modbusFunctions.append(ModbusFunctions_ns::Custom);
	}
	else {
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadDiscreteInputs);	// команда поддерживает этот тип функции
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadInputRegisters);	// команда поддерживает этот тип функции
	}
	

}

bool ModbusCommandAins::setValues(startAddress_t startAddr, count_t count, ModbusCommand_ns::opr_t opr, uint8_t dataBytes, uint8_t dataOrder, 
	uint8_t updAlg, uint8_t expectedByteCount, const QByteArray & expectedFirstByte, const QByteArray & modbusFunctionBuf, float k0, float k1)
{
	if(!setStartAddress(startAddr))
		return false;

	if (!setCount(count))
		return false;

	if (!setOPR(opr))
		return false;

	if (!setDataBytes(dataBytes))
		return false;
	
	if (!setDataOrder(dataOrder))
		return false;

	if (!setUpdAlg(updAlg))
		return false;

	if (!setExpectedBcnt(expectedByteCount))
		return false;

	if (!setExpectedFirstRspBcnt(expectedFirstByte))
		return false;

	if (!setExpectedFirstRspBcnt(expectedFirstByte))
		return false;

	if (!setK0(k0))
		return false;

	if (!setK1(k1))
		return false;

	ModbusParser parser;
	spModbusFunc_t modbusFunction = parser.createFunctionFromBuf(modbusFunctionBuf);
	if (!modbusFunction)
		return false;

	if (!setModbusFunction(modbusFunction))
		return false;

	return true;
}

const QByteArray & ModbusCommandAins::getCommandDescription()
{
	m_commandByteArray.clear();

	if (!calculateCommandLen())	// Если это пользовательская функция, то пересчитываем
		return m_commandByteArray;

	QByteArray tempHeadline(getCommandHeadlineByteArray());	// Вызываем из предка. Получаем общий буфер
	if (tempHeadline.isEmpty())
		return m_commandByteArray;	// Возвращаем пустой

	QByteArray tempModbusFunction(getModbusFunction()->getModbusRequestByteArray());
	if (tempModbusFunction.isEmpty())
		return m_commandByteArray;	// Возвращаем пустой

	QByteArray floatKoeff;
	char * p = (char *)&m_k0;
	floatKoeff.append(p, sizeof(float));

	p = (char *)&m_k1;
	floatKoeff.append(p, sizeof(float));

	m_commandByteArray.append(tempHeadline);
	m_commandByteArray.append(floatKoeff);
	m_commandByteArray.append(tempModbusFunction);

	return m_commandByteArray;
}

bool ModbusCommandAins::calculateCommandLen()
{
	spModbusFunc_t function(getModbusFunction());
	if (!function)
		return false;

	uint16_t len = getHeadlineByteLen() + sizeof(m_k0) + sizeof(m_k1) + function->getReqLen();
	if (!setLen(len))
		return false;

	return true;
}

bool ModbusCommandAins::parseExtraInfoFromBuf(const QByteArray & buf)
{
	if (buf.size() != 2 * sizeof(float))	// Буфер не должен быть пуст для этой функции!
		return false;
	
	float *p = (float*)buf.data();
	float k0 = *p;
	p++;
	float k1 = *p;

	if (!setK0(k0) || !setK1(k1)) {
		return false;
	}
	return true;
}

void ModbusCommandAins::createDefaultModbusFunction()
{
	spModbusFunc_t function;
	if (!m_isModifyModbus) {
		function.reset(new ModbusFunction04_ReadInputRegisters(getStartAddress(), getCount()));
	}
	else {
		function.reset(new ModbusFunctionCustom(getStartAddress(), getCount()));
	}

	setModbusFunction(function);
}

bool ModbusCommandAins::setK0(float k)
{
	m_k0 = k;
	return true;
}

bool ModbusCommandAins::setK1(float k)
{
	m_k1 = k;
	return true;
}
