#pragma once

#ifndef BIT0

#define BIT0		1
#define BIT1		( BIT0 << 1 )
#define BIT2		( BIT0 << 2 )
#define BIT3		( BIT0 << 3 )
#define BIT4		( BIT0 << 4 )
#define BIT5		( BIT0 << 5 )
#define BIT6		( BIT0 << 6 )
#define BIT7		( BIT0 << 7 )

#define BIT8		( BIT0 << 8 )
#define BIT9		( BIT0 << 9 )
#define BIT10		( BIT0 << 10 )
#define BIT11		( BIT0 << 11 )
#define BIT12		( BIT0 << 12 )
#define BIT13		( BIT0 << 13 )
#define BIT14		( BIT0 << 14 )
#define BIT15		( BIT0 << 15 )

#define BIT16		( BIT0 << 16 )
#define BIT17		( BIT0 << 17 )
#define BIT18		( BIT0 << 18 )
#define BIT19		( BIT0 << 19 )
#define BIT20		( BIT0 << 20 )
#define BIT21		( BIT0 << 21 )
#define BIT22		( BIT0 << 22 )
#define BIT23		( BIT0 << 23 )

#endif // !BIT0

namespace ns_slaveModeDefines {

	static const int	SLAVE_MODE =				0;				// Смещения от начального адреса
	static const int	TIMEOUT =					1;
	static const int	XID_BLOCK_LEN =				2;				// Узнать длину XID!!!
	static const int	XID_BLOCK_START =			3;				
	static const int	FLAGS_CHANNELS_1 =			19;				// Два байта на маски каналов
	static const int	FLAGS_CHANNELS_2 =			20;				// 


	// Маски имеют разный размер!!! В RTU она 4 байта, а в DeProtec - 8!!!
	static const int	MASK_READ_ANALOG_INPUT =				FLAGS_CHANNELS_2 + 1;				// 4 для маски канала 
	//static const int	MASK_READ_DISCRETE_INPUT =				MASK_READ_ANALOG_INPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_READ_COUNT_INPUT =					MASK_READ_DISCRETE_INPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_READ_ANALOG_OUTPUT =				MASK_READ_COUNT_INPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_READ_DISCRETE_OUTPUT =				MASK_READ_ANALOG_OUTPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_WRITE_SET_OF_ANALOG_OUTPUT =		MASK_READ_DISCRETE_OUTPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_WRITE_STATE_OF_DISCRETE_OUTPUT =	MASK_WRITE_SET_OF_ANALOG_OUTPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	//static const int	MASK_SERVICE_MANADGMENT_AND_INDICATION = MASK_WRITE_STATE_OF_DISCRETE_OUTPUT + BYTES_IN_CHANNEL_MASK;				// 4 для маски канала 
	////static const int	MASK_TIME_SYNCHRONIZATION = 53;				// Не используется
	//static const int	MASK_POLLON_THE_LINK = 57;				//  Не используется 

//static const int  REQUEST_FLAGS	// Должно обрабатываться динамически!

	// Флаги для настроек особенностей
	static const int FLAG_SLAVE_ACTIVE =							BIT0;		// Слейв  активный (сконфигурирован) 1- сконфигурирован
	static const int FLAG_SLAVE_OLD =								BIT1;		// Слейв из устаревшей линейки модулей на 8-ми разрядных  процессорах  
	static const int FLAG_SLAVE_IMPULSE =							BIT2;		// Слейв поддерживает  только импульсные команды управления ТУ 	
	static const int FLAG_SLAVE_EMZM_TYPE =							BIT3;		// Слейв из категории представления счетчиков  ЭЭ EM3M: 0- нет ; 1 да
	static const int FLAG_SLAVE_MODE_OR_COUNT =						BIT4;		// Если слейв из категории представления счетчиков  ЭЭ EM3M -1-суммарный  режим ,0- пофазный режим. Если нет :1- 8 байтовый счетчик, 0- 4-х байтовый счетчик.

	// Флаги для включения запросов на конкретный тип канала
	//		В первом байте FLAGS_CHANNELS_1:
	static const int FLAG_READ_ANALOG_INPUT =					BIT0;     // ("Чтение аналоговых каналов ТИ");
	static const int FLAG_READ_DISCRETE_INPUT =					BIT1;     // ("Чтение дискретных каналов ТС");
	static const int FLAG_READ_COUNT_INPUT =					BIT2;     // ("Чтение счетных каналов ТИИ");
	static const int FLAG_READ_ANALOG_OUTPUT =					BIT3;     // ("Чтение состояния аналоговых выходов");
	static const int FLAG_READ_DISCRETE_OUTPUT =				BIT4;     // ("Чтение состояния дискретных выходов");
	static const int FLAG_WRITE_SET_OF_ANALOG_OUTPUT =			BIT5;     // ("Запись установок аналоговых выходов");
	static const int FLAG_WRITE_STATE_OF_DISCRETE_OUTPUT =		BIT6;     // ("Запись состояний управления ТУ дискретных выходов");
	static const int FLAG_SERVICE_MANADGMENT_AND_INDICATION =	BIT7;     // ("Обслуживание устройства отображения и индикации");

	//		Во втором байте FLAGS_CHANNELS_2:
	static const int FLAG_TIME_SYNCHRONIZATION =				BIT6;    // ("Синхронизация времени");
	static const int FLAG_POLLON_THE_LINK =						BIT7;    // ("Опрос на наличие связи");


	static const int FLAG_MASTER_TIME_SYNC_1980 = BIT0;
	static const int FLAG_MASTER_TIME_SYNC_1970 = BIT1;
}

