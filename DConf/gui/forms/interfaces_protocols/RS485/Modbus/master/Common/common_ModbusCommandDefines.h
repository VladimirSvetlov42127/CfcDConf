#pragma once
#include <qstring.h>
#include <qvector.h>

namespace ModbusMaster_ns {

	typedef uint16_t commandTableIndex_t;	// Тип для обозначения номера команда из таблицы соответствующего типа
	typedef uint16_t startAddress_t;	// Название для типа, если потом будет меняться
	typedef uint16_t count_t;
	typedef uint8_t varType_t;	// Тип преобразуется в channelType_t 


	enum channelType_t {	// Тип команды/таблицы для нее. Размер 4 байта! 
		AinsInd = 0,
		DinsInd,
		CinsInd,
		RAoutsInd,
		RDoutsInd,
		WAoutsInd,
		WDoutsInd
	};

	const QVector<channelType_t> qv_channelTypes{ AinsInd , DinsInd, CinsInd, RAoutsInd, RDoutsInd, WAoutsInd, WDoutsInd };

	struct structCommand_t { // Структура, передаваемая в сигнале для создания команды из таблицы в чекбокс
		channelType_t type;	// Тип команды
		commandTableIndex_t index;	// Порядковый номер в таблице
		startAddress_t startChannel;	// Стартовый адрес
		count_t count;	// Кол-во
		QString name;	// Имя команды
	};

	typedef QString channelName_t;
	static const channelName_t CHANNEL_NAME_AINS("Аналоговые входы");
	static const channelName_t CHANNEL_NAME_DINS("Дискретные входы");
	static const channelName_t CHANNEL_NAME_CINS("Счетные входы");
	static const channelName_t CHANNEL_NAME_RAINS("Аналоговые выходы");
	static const channelName_t CHANNEL_NAME_RDINS("Дискретные выходы");
	static const channelName_t CHANNEL_NAME_WAINS("Управление аналоговыми выходами");
	static const channelName_t CHANNEL_NAME_WDINS("Управление дискретными выходами");


	const QVector<channelName_t> g_channelNames{ CHANNEL_NAME_AINS, CHANNEL_NAME_DINS, CHANNEL_NAME_CINS,
	CHANNEL_NAME_RAINS, CHANNEL_NAME_RDINS, CHANNEL_NAME_WAINS, CHANNEL_NAME_WDINS };
}

