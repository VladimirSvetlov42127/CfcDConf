#include "ModbusCommandDins.h"
#include <qdebug.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction02_ReadDiscreteInputs.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunctionCustom.h>

ModbusCommandDins::ModbusCommandDins(bool isModifyModbus)
	: ModbusCommandDescription(ModbusMaster_ns::DinsInd, isModifyModbus)
{
	setDefaultValues();
	setAvailableModbusFunctions();
}

ModbusCommandDins::ModbusCommandDins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent)
	: ModbusCommandDescription(ModbusMaster_ns::DinsInd, startChannel, count, isModifyModbus, parent)
{
	setDefaultValues();
	setAvailableModbusFunctions();
}

ModbusCommandDins::~ModbusCommandDins()
{
	qDebug() << Q_FUNC_INFO;
}

void ModbusCommandDins::setDefaultValues()
{
	setOPR(ModbusCommand_ns::oprWrite);
}

void ModbusCommandDins::setAvailableModbusFunctions()
{	
	if (m_isModifyModbus) {	// Если модбас модифицированный, добавляем возможность создания функции через буфер обычный
		mv_modbusFunctions.append(ModbusFunctions_ns::Custom);
	}
	else {
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadDiscreteInputs);	// команда поддерживает этот тип функции
		mv_modbusFunctions.append(ModbusFunctions_ns::ReadInputRegisters);	// команда поддерживает этот тип функции
	}
}

const QByteArray & ModbusCommandDins::getCommandDescription()
{	
	m_commandByteArray.clear();
	
	if (!calculateCommandLen())	// Если это пользовательская функция, то пересчитываем
		return m_commandByteArray;

	QByteArray tempHeadline (getCommandHeadlineByteArray());	// Вызываем из предка. Получаем общий буфер
	if (tempHeadline.isEmpty())
		return m_commandByteArray;	// Возвращаем пустой

	QByteArray tempModbusFunction(getModbusFunction()->getModbusRequestByteArray());
	if (tempModbusFunction.isEmpty())
		return m_commandByteArray;	// Возвращаем пустой
	
	m_commandByteArray.append(tempHeadline);
	m_commandByteArray.append(tempModbusFunction);
	
	return m_commandByteArray;
}

bool ModbusCommandDins::calculateCommandLen()
{
	spModbusFunc_t function(getModbusFunction());
	if(!function)
		return false;

	uint16_t len = getHeadlineByteLen() + function->getReqLen();
	if (!setLen(len))
		return false;

	return true;
}

bool ModbusCommandDins::parseExtraInfoFromBuf(const QByteArray & buf)
{
	if(!buf.isEmpty())	// Для данной команды буфер должен быть пуст!
		return false;
	return true;
}

void ModbusCommandDins::createDefaultModbusFunction()
{
	spModbusFunc_t function;
	if (!m_isModifyModbus) {
		function.reset(new ModbusFunction02_ReadDiscreteInputs(getStartAddress(), getCount()));
	}
	else {
		function.reset(new ModbusFunctionCustom(getStartAddress(), getCount()));
	}

	setModbusFunction(function);
}






