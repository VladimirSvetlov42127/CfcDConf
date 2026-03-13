#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

class ModbusFunction04_ReadInputRegisters : public ModbusFunction
{
	Q_OBJECT

public:
	enum {
		function04_byteSize = 5
	};

	ModbusFunction04_ReadInputRegisters();
	ModbusFunction04_ReadInputRegisters(ModbusMaster_ns::startAddress_t start, ModbusMaster_ns::count_t count, QObject *parent = Q_NULLPTR);
	~ModbusFunction04_ReadInputRegisters();

	// Абстрактные виртуальные функции
		// Functional 
	virtual bool parseFromBuffer(const QByteArray &buffer);

		// Getters
	virtual const QByteArray& getModbusRequestByteArray();
	virtual uint8_t getReqLen() const { return function04_byteSize; };
	virtual ModbusCommand_ns::opr_t getOPR() const { return ModbusCommand_ns::oprRead; };
	virtual uint8_t getExpectedBcnt() const;
	virtual uint8_t getAnalizedRspBcnt() const { return getExpectedFirstRspBcnt().size(); };
	virtual QByteArray getExpectedFirstRspBcnt() const;
	virtual uint16_t getModbusRegisterCount() const;

		// Setters
	//virtual bool setModbusRegisterCount(uint16_t registerCount);

private:
	QByteArray m_requestByteArray;
};
