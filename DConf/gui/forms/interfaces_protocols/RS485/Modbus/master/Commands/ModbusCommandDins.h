#pragma once

#include <qvector.h>
#include <qsharedpointer.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

class ModbusCommandDins : public ModbusCommandDescription
{
	Q_OBJECT

public:
	explicit ModbusCommandDins(bool isModifyModbus);
	explicit ModbusCommandDins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent);
	virtual ~ModbusCommandDins();

	// Инициализация при создании
	void setDefaultValues();	// При создании заполняем некоторые поля по умолччанию
	void setAvailableModbusFunctions();	// Передает доступные Модбас функции
	
	// Обязательные функции
	virtual const QByteArray& getCommandDescription(); 
	virtual bool calculateCommandLen();
	virtual bool parseExtraInfoFromBuf(const QByteArray & buf);
	virtual void createDefaultModbusFunction();


private:
	QByteArray m_commandByteArray;	// Упакованный буфер запроса

};
