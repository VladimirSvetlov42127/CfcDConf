#pragma once

#include <memory>

#include <qobject.h>

#include <dpc/dpc_global.h>
#include <dpc/sybus/channel/async/ChannelRequest.h>

namespace Dpc::Sybus
{
	class AsyncChannel;

	// Класс объекта ответа на запрос нижнего уровня.
	// объект данного класса возвращается на запрос к асинхроному каналу. 
	// после завершения издает сигнал finished.
	// относится к нижнему уровню.

	class DPC_EXPORT ChannelReply : public QObject
	{
		Q_OBJECT

	public:
		
		// Состояние запроса
		enum State 
		{
			Idle,						// Простой (создан)
			Pending,					// Ожидает в очереди
			Running,					// Выполняется
			Finished					// Завершен
		};

		// Коды ошибок происходящих во время выполнения запроса
		// начинаются от 0x1000, чтобы не пересекаться с кодами приходящими от устройства
		enum Error : uint16_t
		{
			NoError = 0,				// Нет ошибки
			NotConnected = 0x1000,		// Соединение не установлено
			TransportError,				// Ошибка транспортного уровня
			InvalidEOF,					// Некорректный символ конца пакета
			InvalidCRC,					// Некорректный CRC
			InvalidASCII,				// Некорректный ASCII символ
			InvalidResponseMsgType,		// Неверный формат ответа
			UnexpectedParamType,		// Тип параметра не указан в ответе 
		};

		// Коды приходяшие от устройства (0-255)
		enum ReturnCode : uint8_t
		{
			Good = 0,							// "Операция выполнена успешно"
			InvalidOperation = 0x01,			// "Неверная операция"
			Busy = 0x02,						// "Устройство занято"
			IlgCmd = 0x10,						// "Неизвестная команда"
			BadCmd = 0x20,						// "Неправильный формат команды"
			BadLenCmd = 0x21,					// "Неправильный формат команды"
			BadPar = 0x22,						// "Неверный параметр"
			ProtectedCmd = 0x23,				// "Операция запрещена"
			BadID = 0x24,						// "Параметр не поддерживается"
			InvalidType = 0x25,					// "Неверный Тип данных"
			InvalidIndex = 0x26,				// "Неверный индекс" / "Конец файла"
			InvalidCount = 0x27,				// "Неверное количества или соотношения индекса + количество"
			Protected = 0x42,					// "Параметр защищен" / "Файл защищен"
			InvalidLoginPassword = 0x62,		// "Неверный логин или пароль"
			InvalidDevice = 0x80,				// "Неверное устройство(плата)"
			InvalidValue = 0x84,				// "Неверное значение параметра"
		};

		ChannelReply(const ChannelRequest& request, QObject* parent = nullptr);
		~ChannelReply();

		// Запрос 
		ChannelRequest request() const;

		// Состояние 
		State state() const;

		// Ошибка и описание ошибки возникщией при выполнении запроса.
		Error error() const;
		QString errorString() const;

		// Код возврата и описание кода возврата опреации от устройства
		ReturnCode returnCode() const;
		QString returnCodeString() const;

		// Для применения изменений необходима перезагурзка устройства.
		// Может содержаться в ответе при запросах на запись. 
		bool needRestart() const;

		// Устройство занято. 
		// Может содержаться в ответе при запросах на запись.
		bool isBusy() const;

		// Время которое устройство занято, в милисекундах. 
		// Может содержаться в ответе при запросах на запись.  
		uint16_t delay() const;		

		// Тип данных в ответе на запрос чтения.
		uint8_t dataType() const;		

		// Данные в ответе на запрос чтения.
		QByteArray data() const;

	signals:
		// Сигнал о завершении выполнении запроса.
		void finished();

	private:
		void setState(ChannelReply::State s);
		void setError(Error err, const QString& errStr = QString());
		void setReturnCode(uint8_t code);
		void setNeedRestart(bool need);
		void setBusy(bool busy);
		void setDelay(uint16_t d);
		void setDataType(uint8_t d);
		void setData(const QByteArray& d);

	private:
		friend class AsyncChannel;

		ChannelRequest m_request;
		State m_state;
		Error m_error;
		QString m_errorString;

		ReturnCode m_retCode;
		bool m_needRestart;
		bool m_isBusy;		
		uint16_t m_delay;
		uint8_t m_dataType;
		QByteArray m_data;
	};
} // namespace