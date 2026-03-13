#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>

class ModbusCommandAins : public ModbusCommandDescription
{
	Q_OBJECT

public:
	explicit ModbusCommandAins(bool isModifyModbus);
	explicit ModbusCommandAins(startAddress_t startChannel, count_t count, bool isModifyModbus, QObject *parent);
	ModbusCommandAins(const ModbusCommandAins& rhs);
	~ModbusCommandAins();

	ModbusCommandAins& operator=(const ModbusCommandAins& rhs);

	void setDefaulValues();
	bool setValues(startAddress_t startAddr, count_t count, ModbusCommand_ns::opr_t opr,
		uint8_t dataBytes, uint8_t dataOrder, uint8_t updAlg, uint8_t expectedByteCount, 
		const QByteArray& expectedFirstByte, const QByteArray &modbusFunction, float k0, float k1);

	// Обязательные функции
	virtual const QByteArray& getCommandDescription(); 
	virtual bool calculateCommandLen();
	virtual bool parseExtraInfoFromBuf(const QByteArray & buf);
	virtual void createDefaultModbusFunction();

	// Getters
	float getK0() const { return m_k0;  };
	float getK1() const { return m_k1; };

	// Setters
	bool setK0(float k);
	bool setK1(float k);

private:
	QByteArray m_commandByteArray;
	float m_k0;
	float m_k1;
};
