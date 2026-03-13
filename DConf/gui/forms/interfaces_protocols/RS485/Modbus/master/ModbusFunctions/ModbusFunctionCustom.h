#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>

class ModbusFunctionCustom : public ModbusFunction
{
	Q_OBJECT

	typedef ModbusFunctions_ns::modbusFunctionType_t modbusFunctionType_t;
	typedef ModbusMaster_ns::startAddress_t startAddress_t;
	typedef ModbusMaster_ns::count_t count_t;
	typedef ModbusCommand_ns::opr_t opr_t;

public:

	ModbusFunctionCustom();
	explicit ModbusFunctionCustom(modbusFunctionType_t customType);
	explicit ModbusFunctionCustom(startAddress_t start, count_t count, QObject *parent = Q_NULLPTR);

	~ModbusFunctionCustom();
	
	// Абстрактные виртуальные функции
		// Functional 
	virtual bool parseFromBuffer(const QByteArray &buffer);

		// Getters
	virtual const QByteArray& getModbusRequestByteArray() { return m_requestByteArray; };
	virtual uint8_t getReqLen() const { return m_requestByteArray.size(); };
	virtual ModbusCommand_ns::opr_t getOPR() const { return m_opt; };
	virtual uint8_t getExpectedBcnt() const { return 0; };	// Нельзя подсчитать в пользовательском типе
	virtual uint8_t getAnalizedRspBcnt() const { return 0; };	// Нельзя подсчитать в пользовательском типе
	virtual QByteArray getExpectedFirstRspBcnt() const { return 0; };	// Нельзя подсчитать в пользовательском типе
	virtual uint16_t getModbusRegisterCount() const { return m_modbusRegisterCount; };	

		// Setters
	virtual bool setModbusRegisterCount(uint16_t registerCount) { m_modbusRegisterCount = registerCount; return true; };	// Нет реализации

	// Невиртуальные функции
	bool setRequestArray(const QByteArray &array);

private:
	QByteArray m_requestByteArray;
	uint16_t m_modbusRegisterCount;
	opr_t m_opt;	// Команда для чтения или записи 
};
