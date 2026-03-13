#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>

class ModbusCommandCins : public ModbusCommandDescription
{
	Q_OBJECT
		
public:
	explicit ModbusCommandCins(bool isModifyModbus);
	explicit ModbusCommandCins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent);
	~ModbusCommandCins();

	void setDefaulValues();

	// Обязательные функции
	virtual const QByteArray& getCommandDescription();
	virtual bool calculateCommandLen();
	virtual bool parseExtraInfoFromBuf(const QByteArray & buf);
	virtual void createDefaultModbusFunction();

	// Getters

	// Setters
	inline bool setMultilpicator(uint32_t value) { m_multiplicator = value; return true; };

private:
	QByteArray m_commandByteArray;
	uint32_t m_multiplicator;
};
