#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>

class ModbusFunction02_ReadDiscreteInputs : public ModbusFunction
{
	Q_OBJECT

public:

	enum {
		function02_byteSize = 5
	};

	ModbusFunction02_ReadDiscreteInputs();
	explicit ModbusFunction02_ReadDiscreteInputs(ModbusMaster_ns::startAddress_t start, ModbusMaster_ns::count_t count, QObject *parent = Q_NULLPTR);
	~ModbusFunction02_ReadDiscreteInputs();

	// Абстрактные переопределяемые функции
		// Functional 
	virtual bool parseFromBuffer(const QByteArray &buffer);

		// Getters
	virtual const QByteArray& getModbusRequestByteArray();
	virtual uint8_t getReqLen() const  { return function02_byteSize; };
	virtual ModbusCommand_ns::opr_t getOPR() const  { return ModbusCommand_ns::oprRead; };
	virtual uint8_t getExpectedBcnt() const;
	virtual uint8_t getAnalizedRspBcnt() const  { return getExpectedFirstRspBcnt().size(); };
	virtual QByteArray getExpectedFirstRspBcnt() const;
	virtual uint16_t getModbusRegisterCount() const  { return getCount(); };

		// Setters
	//virtual bool setModbusRegisterCount(uint16_t registerCount)  { return(setCount(static_cast<ModbusMaster_ns::count_t>(registerCount))); };

private: 
	QByteArray m_requestByteArray;
};
