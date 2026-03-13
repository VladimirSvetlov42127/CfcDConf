#include "ModbusCommandCins.h"


#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction04_ReadInputRegisters.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunctionCustom.h>

ModbusCommandCins::ModbusCommandCins(bool isModifyModbus)
	: ModbusCommandDescription(ModbusMaster_ns::CinsInd, isModifyModbus),
	m_multiplicator(0)
{
	setDefaulValues();
}

ModbusCommandCins::ModbusCommandCins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent)
	: ModbusCommandDescription(ModbusMaster_ns::CinsInd, startChannel, count, isModifyModbus, parent),
	m_multiplicator(0)
{
	setDefaulValues();
}

ModbusCommandCins::~ModbusCommandCins()
{
}

void ModbusCommandCins::setDefaulValues()
{
	setOPR(ModbusCommand_ns::oprRead);
	setDataBytes(8);
	
	if (m_isModifyModbus) {	// Если модбас модифицированный, добавляем возможность создания функции через буфер обычный
		mv_modbusFunctions.append(ModbusFunctions_ns::Custom);
	}
	else {
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadDiscreteInputs);	// команда поддерживает этот тип функции
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadInputRegisters);	// команда поддерживает этот тип функции
	}
}

const QByteArray & ModbusCommandCins::getCommandDescription()
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

	QByteArray tempMuliplicator;
	tempMuliplicator.append(m_multiplicator & 0xFF);
	tempMuliplicator.append((m_multiplicator >> 8) & 0xFF);
	tempMuliplicator.append((m_multiplicator >> 16) & 0xFF);
	tempMuliplicator.append((m_multiplicator >> 24) & 0xFF);

	m_commandByteArray.append(tempHeadline);
	m_commandByteArray.append(tempMuliplicator);
	m_commandByteArray.append(tempModbusFunction);

	return m_commandByteArray;
}

bool ModbusCommandCins::calculateCommandLen()
{
	spModbusFunc_t function(getModbusFunction());
	if (!function)
		return false;

	uint16_t len = getHeadlineByteLen() + sizeof(m_multiplicator) + function->getReqLen();
	if (!setLen(len))
		return false;

	return true;
}

bool ModbusCommandCins::parseExtraInfoFromBuf(const QByteArray & buf)
{
	if (buf.size() != sizeof(m_multiplicator))	// Буфер не должен быть пуст для этой функции!
		return false;

	uint32_t *p = (uint32_t*)buf.data();

	if (!setMultilpicator(*p)) {
		return false;
	}
	return true;
}

void ModbusCommandCins::createDefaultModbusFunction()
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
