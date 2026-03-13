#pragma once

#include <qpair.h>
#include "common_ModbusCommandDefines.h"
#include "qobject.h"

namespace ModbusCommand_ns {
	
    typedef QPair<ModbusMaster_ns::channelType_t, ModbusMaster_ns::commandTableIndex_t> keyPairTypeAndIndex_t;

	enum opr_t {
		oprWrite = 0,
		oprRead
	};

	enum {
		ExpectedFirstRspBcnt_Count = 4
	};

#pragma pack(push, 1)
	typedef struct {
		uint16_t  opr : 1;			/*0-write  1-read*/
		uint16_t  DataBytes : 3; 	/*длина элемента в ответе в байтах (0 соответствует 8 байт)*/
		uint16_t  DataOrder : 4;	/*последовательность байт в элементе данных */
		uint16_t  Updalg : 4;		/*алгоритм интерпретации*/
		uint16_t  Rsv : 4;
	} MB_UpdateOptions_t;

	typedef union {	// Для копирования битовых полей и записи
		uint16_t all;
		MB_UpdateOptions_t data;
	} MB_UpdOpt_Type;

	typedef struct {/*header of request to slave definition  for origin modbus */
		uint8_t			Len;				/*length  ; 0-> end of requests*/
		ModbusMaster_ns::varType_t		VarType;    		/*Modbus_Ain  ....*/
		ModbusMaster_ns::count_t		VarCnt;				/*count of _Ain  ....*/
		uint8_t			ReqLen;				/*transmitt request length*/
		MB_UpdOpt_Type  OprOpt;		/*update options - define represantation alg etc*/
		uint8_t   		ExpectedBcnt;		/*expected response byte count*/
		uint8_t   		AnalizedRspBcnt; 	/*response header template length*/
		uint8_t   		ExpectedFirstRspByte[ExpectedFirstRspBcnt_Count];/*expected first bytes in response*/
	} ModbusVarUpdate_t;
#pragma pack(pop)

}

Q_DECLARE_METATYPE(ModbusCommand_ns::keyPairTypeAndIndex_t);
