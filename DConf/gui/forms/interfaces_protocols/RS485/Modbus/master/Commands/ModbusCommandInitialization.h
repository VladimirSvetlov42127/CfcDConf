#pragma once

#include <QObject>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>

class ModbusCommandInitialization : public QObject
{
	Q_OBJECT

public:
	enum {
		MAX_ANALIZED_ANSWER_BYTE_COUNT = 4,
		MAX_INIT_COMMAND_BYTE_LEN = 16
	};
	enum {	// Позиция в массиве байт
		LEN_POS = 0,
		REG_DATA_LEN_POS = 1,
		EXPECTED_BCNT_POS = 2,
		ANALIZED_RSP_BCNT = 3,
		EXPECTED_FIRST_RSP_BYTE_POS = 4,
		INIT_COMMAND_POS = 8
	};

public:
	ModbusCommandInitialization(QObject *parent = Q_NULLPTR);
	~ModbusCommandInitialization();

	bool parseFromBuf(const QByteArray& arr);

	// Getters
	inline uint8_t getLen() const				{ return m_Len; };
	inline uint8_t getReqDataLen() const		{ return m_ReqDataLen; };
	inline uint8_t getExpectedBcnt() const		{ return m_ExpectedBcnt; };
	inline uint8_t getAnalizedRspBcnt() const	{ return m_AnalizedRspBcnt; };
	inline const QByteArray& getExpectedFirstRspByte() const { return m_ExpectedFirstRspByte; };
	inline const QByteArray& getInitCmdBuf() const { return m_InitCmdBuf; };

	const QByteArray& getInitFullCommandByteArray();
	
	// Setters
	bool setExpectedBcnt(uint8_t value);
	bool setExpectedFirstRspByte(const QByteArray& arr);
	bool setAnalizedRspBcnt(uint8_t value);
	bool setInitCmdBuf(const QByteArray& arr);

private:
	uint8_t	m_Len;				/*length of field  0- end*/
	uint8_t	m_ReqDataLen;		/*length of transaction in InitCmd[]*/
	uint8_t m_ExpectedBcnt;		/*expected response byte count*/
	uint8_t m_AnalizedRspBcnt;	/*count of analized response byte*/
	QByteArray m_ExpectedFirstRspByte;	/*expected first bytes in response (if AnalizedRspBcnt !=0) [ModbusCommand_ns::ExpectedFirstRspBcnt_Count] размер*/ 
	QByteArray m_InitCmdBuf;		/*init command*/

	QByteArray m_FullInitCommandBuff;	// Массив для хранения команды


};
