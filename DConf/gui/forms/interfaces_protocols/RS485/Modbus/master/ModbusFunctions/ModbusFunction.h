#pragma once

#include <QObject>
#include <qbytearray.h>
#include <qsharedpointer.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusFunctionDefines.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>

class ModbusFunction;
typedef QSharedPointer<ModbusFunction> spModbusFunc_t;

class ModbusFunction : public QObject
{
	Q_OBJECT
	
protected:
	typedef ModbusMaster_ns::startAddress_t startAddress_t;
	typedef ModbusMaster_ns::count_t count_t;
	typedef ModbusFunctions_ns::modbusFunctionType_t modbusFunctionType_t;

public:

	enum {
		MODBUS_REGISTER_BYTE_SIZE = 2
	};

	ModbusFunction();
	explicit ModbusFunction(modbusFunctionType_t fuctionCode);
	explicit ModbusFunction(modbusFunctionType_t fuctionCode, startAddress_t start, count_t count, QObject *parent);
	~ModbusFunction();

	// Абстрактные виртуальные функции
		// Functional 
	virtual bool parseFromBuffer(const QByteArray &buffer) = 0;

		// Getters
	virtual const QByteArray& getModbusRequestByteArray() = 0;
	virtual uint8_t getReqLen() const = 0;
	virtual ModbusCommand_ns::opr_t getOPR() const = 0;
	virtual uint8_t getExpectedBcnt() const	= 0;
	virtual uint8_t getAnalizedRspBcnt() const = 0;
	virtual QByteArray getExpectedFirstRspBcnt() const = 0;
	virtual uint16_t getModbusRegisterCount() const = 0;

		// Setters
	//virtual bool setModbusRegisterCount(uint16_t registerCount) = 0;

	// Невиртуальные функции
	// Getters
	inline modbusFunctionType_t getFunctionType() const { return m_functionCode; };
	inline startAddress_t getStartAddress() const		{ return m_startAddress; };
	inline count_t getCount() const						{ return m_count; };
	inline uint16_t getDataBytes() const				{ return m_dataByteLen; };

	// Setters
	bool setFunctionCode(modbusFunctionType_t code);
	bool setStartAddress(startAddress_t start);
	bool setCount(count_t count);
	bool setDataByte(uint16_t bytes);

private:
	modbusFunctionType_t m_functionCode;
	startAddress_t m_startAddress;
	count_t m_count;
	uint16_t m_dataByteLen; 
};
