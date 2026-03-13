#pragma once

#include <qbytearray.h>

#include <dpc/sybus/utils.h>

namespace Dpc::Sybus
{
	// Структура запроса к устройству нижнего уровня.
	// относится к нижнему уровню.

	struct ChannelRequest
	{
		enum Type
		{
			ReadRequest,				// Чтение параметра
			WriteRequest				// Запись параметра
		};

		// Тип запроса
		Type type = ReadRequest;

		// Адрес параметра 
		uint16_t addr = 0;		

		// Начальный индекс
		uint16_t index = 0;				

		// Кол-во считываемых/записываемых данных
		uint16_t count = 1;				 

		// Плата расширения на 0-м порту Rs485. Используется когда нужно обратиться к внутреним слейвам.
		int board = -1;					

		// Адрес-аргумент при выполнении расширеного запроса. Используется когда нужно узнать метаданные о параметре: тип, имя, размерность и т.д.
		uint16_t subAddr = 0;			

		// Тип записываемых данных. Задается при записи параметра.
		uint8_t dataType = 0;			 

		// Записываемые данные. Задается при записи параметра.
		QByteArray data;				

		// Ожидать ли ответа от устройства. Введен для перезагрузки устройства. Игнорируется и сбрасывается если зарос идет через шлюз или к плате.
		bool needResponse = true;		

		// Адресат запроса Мастер или Слейв. Используется для инкапсулированых запросов в режиме шлюза, чтобы иметь возможность делать запрос мастеру. 
		bool toMaster = false;	

		QString toString() const
		{
			auto result = QString("%1").arg(toHex(addr));
			if (subAddr)
				result.append(QString("(%1)").arg(toHex(subAddr)));
			result.append(QString("[%1-%2]").arg(index).arg(index + count - 1));
			if (board > -1)
				result.append(QString(", Board(%1)").arg(board));
			if (!needResponse)
				result.append(", NoResponse");
			if (toMaster)
				result.append(", ToMaster");
			if (type == ChannelRequest::WriteRequest)
				result.append(QString(", DataType(%1)").arg(toHex(dataType)));

			return result;
		}
	};
} // namespace