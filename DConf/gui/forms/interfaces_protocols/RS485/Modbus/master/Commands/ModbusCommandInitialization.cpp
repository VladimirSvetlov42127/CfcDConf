#include "ModbusCommandInitialization.h"

ModbusCommandInitialization::ModbusCommandInitialization(QObject *parent)
	: QObject(parent)
{
	m_Len = 0;
	m_ReqDataLen = 0;
	m_ExpectedBcnt = 0;
	m_AnalizedRspBcnt = 0;
}

ModbusCommandInitialization::~ModbusCommandInitialization()
{
}

bool ModbusCommandInitialization::parseFromBuf(const QByteArray & arr)
{
	const uint8_t minByteCount = sizeof(m_Len) + sizeof(m_ReqDataLen) + sizeof(m_ExpectedBcnt) + sizeof(m_AnalizedRspBcnt) + ModbusCommand_ns::ExpectedFirstRspBcnt_Count + sizeof(uint8_t);
	if (arr.size() < minByteCount)
		return false;
	
	if (!setExpectedBcnt(arr[EXPECTED_BCNT_POS]))
		return false;

	if (!setExpectedFirstRspByte(arr.mid(EXPECTED_FIRST_RSP_BYTE_POS, ModbusCommand_ns::ExpectedFirstRspBcnt_Count)))
		return false;

	if (!setAnalizedRspBcnt(arr[ANALIZED_RSP_BCNT]))	// Должно быть после присвоения буфера, т.к. в буфере заполняются неиспользуемые поля нулями!
		return false;
	
	if (!setInitCmdBuf(arr.mid(INIT_COMMAND_POS, arr[REG_DATA_LEN_POS])))
		return false;

	uint16_t len = arr[LEN_POS];
	if (len != m_Len)
		return false;

	return true;
}

const QByteArray & ModbusCommandInitialization::getInitFullCommandByteArray()
{
	m_FullInitCommandBuff.clear();

	if (m_Len == 0 || m_ReqDataLen == 0 || m_ExpectedBcnt == 0 || m_InitCmdBuf.isEmpty())
		return m_FullInitCommandBuff;

	m_FullInitCommandBuff.append(m_Len);
	m_FullInitCommandBuff.append(m_ReqDataLen);
	m_FullInitCommandBuff.append(m_ExpectedBcnt);
	m_FullInitCommandBuff.append(m_AnalizedRspBcnt);
	m_FullInitCommandBuff.append(m_ExpectedFirstRspByte);
	m_FullInitCommandBuff.append(m_InitCmdBuf);
	   
	return m_FullInitCommandBuff;
}

bool ModbusCommandInitialization::setExpectedBcnt(uint8_t value)
{
	m_ExpectedBcnt = value;
	return true;
}

bool ModbusCommandInitialization::setExpectedFirstRspByte(const QByteArray & arr)
{
	if (arr.isEmpty())
		return false;

	if (arr.size() > ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
		return false;

	m_AnalizedRspBcnt = arr.size();	// Сохраняем размер по кол-ву введенных байт
	m_ExpectedFirstRspByte = arr;
	
	// Буфер у нас должен быть из 4 значений всегда
	while (m_ExpectedFirstRspByte.size() < ModbusCommand_ns::ExpectedFirstRspBcnt_Count) {
		m_ExpectedFirstRspByte.append(char(0));
	}

	return true;
}

bool ModbusCommandInitialization::setAnalizedRspBcnt(uint8_t value)
{
	if(value > ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
		return false;

	m_AnalizedRspBcnt = value;
	return true;
}

bool ModbusCommandInitialization::setInitCmdBuf(const QByteArray & arr)
{
	if(arr.isEmpty())
		return false;

	if (arr.size() > MAX_INIT_COMMAND_BYTE_LEN)
		return false;

	m_InitCmdBuf = arr;
	m_Len = sizeof(m_Len) + sizeof(m_ReqDataLen) + sizeof(m_ExpectedBcnt) + sizeof(m_AnalizedRspBcnt) + ModbusCommand_ns::ExpectedFirstRspBcnt_Count + m_InitCmdBuf.size();
	m_ReqDataLen = m_InitCmdBuf.size();
	return true;
}


