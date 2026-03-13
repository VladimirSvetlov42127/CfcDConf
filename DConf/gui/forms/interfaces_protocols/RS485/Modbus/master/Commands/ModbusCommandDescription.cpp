#include "ModbusCommandDescription.h"
#include <qdebug.h>

ModbusCommandDescription::ModbusCommandDescription() 
	: QObject(nullptr),
	m_modbusFunction(nullptr),
	m_modbusCommandStruct{ 0 },
	m_commandByteArray{ 0 },
	m_startByte(0)
{
}


ModbusCommandDescription::ModbusCommandDescription(channelType_t VarType, bool isModifyModbus)
	: QObject(nullptr),
	m_modbusFunction(nullptr),
	m_modbusCommandStruct{ 0 },
	m_commandByteArray(),
	m_startByte(0),
	m_isModifyModbus(isModifyModbus)
{
	m_modbusCommandStruct.VarType = VarType;
}

ModbusCommandDescription::ModbusCommandDescription(channelType_t VarType, startAddress_t startChannel, count_t VarCnt, bool isModifyModbus, QObject *parent)
	: QObject(parent),
	m_modbusFunction(nullptr),
	m_modbusCommandStruct{ 0 },
	m_commandByteArray{ 0 },
	m_isModifyModbus(isModifyModbus),
	m_startByte(startChannel)
{
	m_modbusCommandStruct.VarCnt = VarCnt;
	m_modbusCommandStruct.VarType = VarType;

}

ModbusCommandDescription::~ModbusCommandDescription()
{
	qDebug() << Q_FUNC_INFO;
}

bool ModbusCommandDescription::setStartAddress(startAddress_t startChannel)
{
	m_startByte = startChannel;
	if (m_modbusFunction) {	// Если есть модбас функция, обновляем ее
		m_modbusFunction->setStartAddress(startChannel);
	}
	return true;
}

bool ModbusCommandDescription::setCount(count_t count)
{
	m_modbusCommandStruct.VarCnt = count;
	if (m_modbusFunction) {	// Если есть модбас функция, высчитываем, сколько должно прийти байт в ответе
		m_modbusFunction->setCount(count);
		setExpectedBcnt(m_modbusFunction->getExpectedBcnt());
		setExpectedFirstRspBcnt(m_modbusFunction->getExpectedFirstRspBcnt());
	}
	return true;
}

bool ModbusCommandDescription::fillCommandFromFunctionInfo()
{
	if (!m_modbusFunction)
		return false;

	modbusFunctionType_t type = m_modbusFunction->getFunctionType();

	// Если функция стандартная, то вычитываем данные из модбас функции
	if (isSupportedFunctionCode(type) && type != ModbusFunctions_ns::Custom) {
		if (!setOPR(m_modbusFunction->getOPR()))	// Функия чтения или записи. Должно ли оно быть здесь?
			return false;
		if (!setExpectedBcnt(m_modbusFunction->getExpectedBcnt()))
			return false;
		if (!setAnalizedRspBcnt(m_modbusFunction->getAnalizedRspBcnt()))
			return false;
		if (!setExpectedFirstRspBcnt(m_modbusFunction->getExpectedFirstRspBcnt()))
			return false;
		m_startByte = m_modbusFunction->getStartAddress();
	}

	if (!setReqLen(m_modbusFunction->getReqLen()))	// Длина запроса, должна выполняться и для пользовательских функций
		return false;

	if (!calculateCommandLen())	// Абстрактная функция
		return false;

	return true;
}

bool ModbusCommandDescription::isSupportedFunctionCode(uint16_t code)
{
	return mv_modbusFunctions.contains(static_cast<modbusFunctionType_t>(code));
}

const QString & ModbusCommandDescription::getCommandName() const
{
	return m_commandName;
}

ModbusMaster_ns::startAddress_t ModbusCommandDescription::getStartAddress() const
{
	if (!m_modbusFunction)
		return m_startByte;	// Используется при создании команды. Это значение передается в модбас-функцию
	else 
		return m_modbusFunction->getStartAddress();
}

ModbusMaster_ns::channelType_t ModbusCommandDescription::getCommandType() const
{
	return static_cast<ModbusMaster_ns::channelType_t>(m_modbusCommandStruct.VarType);
}

QByteArray ModbusCommandDescription::getExpectedFirstRspBcnt() const
{
	QByteArray buf;
	for (int i = 0; i < ModbusCommand_ns::ExpectedFirstRspBcnt_Count; i++) {
		buf.append(m_modbusCommandStruct.ExpectedFirstRspByte[i]);
	}
	
	return buf;
}

const ModbusCommand_ns::ModbusVarUpdate_t & ModbusCommandDescription::getCommandStruct() const
{
	return m_modbusCommandStruct;
}

bool ModbusCommandDescription::setCommandName(const QString & name)
{
	if(name.isEmpty() || name.isNull())
		return false;

	m_commandName = name;
	return true;
}

bool ModbusCommandDescription::setLen(uint8_t value)
{
	m_modbusCommandStruct.Len = value;
	return true;
}

bool ModbusCommandDescription::setVarType(ModbusMaster_ns::channelType_t value)
{
	m_modbusCommandStruct.VarType = value;
	return true;
}

bool ModbusCommandDescription::setVarCnt(ModbusMaster_ns::count_t value)
{
	m_modbusCommandStruct.VarCnt = value;
	return true;
}

bool ModbusCommandDescription::setReqLen(uint8_t value)
{
	m_modbusCommandStruct.ReqLen = value;
	return true;
}

bool ModbusCommandDescription::setOPR(ModbusCommand_ns::opr_t flag)
{
	m_modbusCommandStruct.OprOpt.data.opr = flag;
	return true;
}

bool ModbusCommandDescription::setDataBytes(uint8_t treeBits)
{
	if (treeBits > 8)
		return false;

	if (treeBits == 8)	// Особый случай 
		m_modbusCommandStruct.OprOpt.data.DataBytes = 0;
	else
		m_modbusCommandStruct.OprOpt.data.DataBytes = treeBits & 0b0111;
	return true;
}

bool ModbusCommandDescription::setDataOrder(uint8_t fourBits)
{
	m_modbusCommandStruct.OprOpt.data.DataOrder = fourBits & 0x0F;
	return true;
}

bool ModbusCommandDescription::setUpdAlg(uint8_t fourBits)
{
	m_modbusCommandStruct.OprOpt.data.Updalg = fourBits & 0x0F;
	return true;
}

bool ModbusCommandDescription::setExpectedBcnt(uint8_t value)
{
	m_modbusCommandStruct.ExpectedBcnt = value;
	return true;
}

bool ModbusCommandDescription::setAnalizedRspBcnt(uint8_t value)
{
	m_modbusCommandStruct.AnalizedRspBcnt = value;
	return true;
}

bool ModbusCommandDescription::setExpectedFirstRspBcnt(QByteArray array)
{
	if (array.size() > ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
		return false;

	uint8_t emptyBuf[ModbusCommand_ns::ExpectedFirstRspBcnt_Count] = { 0 };

	// Обнуляем буфер
	memcpy(m_modbusCommandStruct.ExpectedFirstRspByte, emptyBuf, ModbusCommand_ns::ExpectedFirstRspBcnt_Count);

	// Копируем новое значение
	memcpy(m_modbusCommandStruct.ExpectedFirstRspByte, array.data(), array.size());
	return true;
}

bool ModbusCommandDescription::setCommandStruct(const  ModbusCommand_ns::ModbusVarUpdate_t & command)
{
	m_modbusCommandStruct = command;
	return true;
}

spModbusFunc_t ModbusCommandDescription::getModbusFunction() const
{
	return m_modbusFunction;
}

bool ModbusCommandDescription::setModbusFunction(spModbusFunc_t function)
{
	if(function == nullptr)
		return false;

	m_modbusFunction = function;
	m_modbusFunction->setDataByte(getDataBytes());
	m_modbusFunction->setCount(getCount());
	if (!fillCommandFromFunctionInfo())
		return false;

	return true;
}


const QByteArray & ModbusCommandDescription::getCommandHeadlineByteArray() 
{
	// По умолчанию при формировании буфера из байт, младший байт идет впереди
	m_commandByteArray.clear();

	const char *pointer = (const char *)&m_modbusCommandStruct;
	int size = sizeof(m_modbusCommandStruct);	// Размер 18!!! Неверно
	m_commandByteArray = QByteArray(pointer, size);


	return m_commandByteArray;
}
