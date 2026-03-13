#pragma once
#include <QObject>
#include <stdint.h>
#include <qhash.h>

namespace ModbusFunctions_ns {

	
	enum modbusFunctionType_t {
		Unknown = 0,
		ReadDiscreteInputs = 2,
		ReadInputRegisters = 4,
		Custom = 255	// Пользовательская функция, вводится для проверок. Коды записываются первым байтом в массиве
    };

	//QHash<QString, int> MY_HASH({ {"one",1},{"two",2} });
	const QHash<modbusFunctionType_t, QString> HASH_FUNCTION_NAME_BY_TYPE{ 
		{ ReadDiscreteInputs, "Read Discrete Inputs" },
		{ ReadInputRegisters, "Read Input Registers" },
		{ Custom, "Custom function" }
	};

	inline bool isStandartFunctionCode(uint8_t functionCode) {
		
		bool isStandart = false;

		switch (functionCode)
		{
		case ModbusFunctions_ns::Unknown:
			isStandart = true;
			break;
		case ModbusFunctions_ns::ReadDiscreteInputs:
			isStandart = true;
			break;
		case ModbusFunctions_ns::ReadInputRegisters:
			isStandart = true;
			break;
		}

		return isStandart;
	};
}


Q_DECLARE_METATYPE(ModbusFunctions_ns::modbusFunctionType_t);

