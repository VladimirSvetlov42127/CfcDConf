#pragma once

#include <QObject>
#include <QByteArray>
#include <qvector.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctions/ModbusFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandDefines.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusFunctionDefines.h>

class ModbusCommandDescription;
typedef QSharedPointer<ModbusCommandDescription> spCommandDescr_t;


class ModbusCommandDescription : public QObject
{
	Q_OBJECT

protected:
	typedef ModbusFunctions_ns::modbusFunctionType_t modbusFunctionType_t;
	typedef ModbusMaster_ns::startAddress_t startAddress_t;
	typedef ModbusMaster_ns::count_t count_t;
	typedef ModbusMaster_ns::channelType_t channelType_t;

public:
	
	ModbusCommandDescription();
	explicit ModbusCommandDescription(channelType_t VarType, bool isModifyModbus);
	explicit ModbusCommandDescription(channelType_t VarType, startAddress_t startChannel, count_t VarCnt, bool isModifyModbus, QObject *parent);
	virtual ~ModbusCommandDescription();

	// Абстрактные функции для реализации в потомках
	virtual const QByteArray& getCommandDescription() = 0;	// Индивидульно для всех функций
	virtual bool calculateCommandLen() = 0;	// Высчитывает общую длину команды с учетом специфики выбранного типа
	virtual bool parseExtraInfoFromBuf(const QByteArray & buf) = 0;
	virtual void createDefaultModbusFunction() = 0;

	// Общие функции
	bool setStartAddress(ModbusMaster_ns::startAddress_t startChannel);
	bool setCount(ModbusMaster_ns::count_t count);
	const QVector<ModbusFunctions_ns::modbusFunctionType_t> &getAvailableModbusFunctionTypes() const { return mv_modbusFunctions;  };	// Получить список разрешенных Модбас функций для этой команды
	bool fillCommandFromFunctionInfo();	// Заполняет команду данными от конкретной функции (длина, возвращаемое кол-ло байт и пр.)
	bool isSupportedFunctionCode(uint16_t code);

	// Геттеры
	const QString &getCommandName() const;
	const uint8_t getHeadlineByteLen() const { return sizeof(ModbusCommand_ns::ModbusVarUpdate_t); };

	ModbusMaster_ns::startAddress_t getStartAddress() const;
	uint8_t getLen() const									{ return m_modbusCommandStruct.Len; };
	ModbusMaster_ns::channelType_t getCommandType() const;
	ModbusMaster_ns::count_t getCount() const				{ return m_modbusCommandStruct.VarCnt; };
	uint8_t getReqLen() const								{ return m_modbusCommandStruct.ReqLen; };
	bool getOPR() const										{ return m_modbusCommandStruct.OprOpt.data.opr; };
	uint8_t getDataBytes() const							{ return m_modbusCommandStruct.OprOpt.data.DataBytes; };
	uint8_t getDataOrder() const							{ return m_modbusCommandStruct.OprOpt.data.DataOrder; };
	uint8_t getUpdAlg() const								{ return m_modbusCommandStruct.OprOpt.data.Updalg; };
	uint8_t getExpectedBcnt() const							{ return m_modbusCommandStruct.ExpectedBcnt; };
	uint8_t getAnalizedRspBcnt() const						{ return m_modbusCommandStruct.AnalizedRspBcnt; };
	QByteArray getExpectedFirstRspBcnt() const;

	const ModbusCommand_ns::ModbusVarUpdate_t & getCommandStruct() const;
	spModbusFunc_t getModbusFunction() const;
	inline bool getIsModifyModbus() const { return m_isModifyModbus; };

	// Сеттеры
	bool setCommandName(const QString & name);

	bool setLen(uint8_t value);
	bool setVarType(channelType_t value);
	bool setVarCnt(count_t value);
	bool setReqLen(uint8_t value);
	bool setOPR(ModbusCommand_ns::opr_t flag);	// Настройка структуры OprOpt
	bool setDataBytes(uint8_t treeBits);	// Настройка структуры OprOpt
	bool setDataOrder(uint8_t fourBits);	// Настройка структуры OprOpt
	bool setUpdAlg(uint8_t fourBits);	// Настройка структуры OprOpt
	bool setExpectedBcnt(uint8_t value);
	bool setAnalizedRspBcnt(uint8_t value);
	bool setExpectedFirstRspBcnt(QByteArray array);

	bool setCommandStruct(const ModbusCommand_ns::ModbusVarUpdate_t & command);
	bool setModbusFunction(spModbusFunc_t function);	// Выбираем функцию модбаса

	inline bool setIsModifyModbus(bool isModifyModbus) { m_isModifyModbus = isModifyModbus; };

private: 
	QString m_commandName;
	QByteArray m_commandByteArray;
	ModbusCommand_ns::ModbusVarUpdate_t m_modbusCommandStruct;	// Общее описание команды
	startAddress_t m_startByte;
	spModbusFunc_t m_modbusFunction;
	
protected:
	QVector<modbusFunctionType_t> mv_modbusFunctions;	// Список доступных типов для данной команды
	bool m_isModifyModbus;

protected:
	const QByteArray& getCommandHeadlineByteArray();
};
