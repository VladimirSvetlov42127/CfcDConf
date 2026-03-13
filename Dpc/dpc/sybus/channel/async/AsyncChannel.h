#pragma once

#include <queue>
#include <mutex>

#include <dpc/sybus/channel/Common.h>
#include <dpc/sybus/channel/async/ChannelReply.h>

namespace Dpc::Sybus
{
	// Асинхроный канал - абстрактный базовый класс взаимодействия с устройством по асинхроному методу. 
	// реализация конкретных интерфейсов связи, делается в наследниках.
	// относится к нижнему уровню. 

	class DPC_EXPORT AsyncChannel : public QObject
	{
		Q_OBJECT

	public:
		AsyncChannel(QObject *parent = nullptr);
		virtual ~AsyncChannel();

		// Тип канала связи. 
		// Определяется в наследнике реализующем конкретный канал связи
		virtual ChannelType type() const = 0;

		// Параметры настроек канала связи. 
		virtual SettingsType settings() const;

		// Установлена ли связь с устройством.
		virtual bool isConnectedToDevice() const = 0;

		// Установка соединения с устройством. 
		// при успешном соединении, испускается сигнал connectedToDevice. 
		// в случае ошибки испускается сигнал error.
		void connectToDevice();

		// Разрыв соединения с устройством.
		// при успешном соединении, испускается сигнал disconnectedFromDevice. 
		// в случае ошибки испускается сигнал error.
		void disconnectFromDevice();

		// Запрос на чтение/запись к устройству. Запрос выполняется асинхронно. Т.е. управление возвращается сразу, не дожидаясь выполнения запроса. 
		// Возвращает указатель на обьект ChannelReply(Ответ от устройства) у которого испустится сигнал finished, при завершении выполнения запроса. 
		// Если в данный момент идет ожидание ответа на другой запрос, текущий запрос добавляется в очередь и будет выполнен в порядке очереди.
		// Ответственность за удаление объекта ложится на вызывающую сторону.
		ChannelReply* exec(const ChannelRequest& req);

		// Размер заголовка запроса на запись. 
		// При board = -1 к устройству, при board > -1 к плате расширения.
		uint16_t writeHeaderSize(int board = -1) const;

	signals:
		// Испускается при успешном соединении с устройством.
		void connectedToDevice();

		// Испускается при успешном разрыве соединения с устройством.
		void disconnectedFromDevice();

		// Сообщения об выполняемых действиях. Для отладки.
		void infoMsg(const QString& msg);

		// Испускается в случае ошибки при работе канала связи. errMsg - описание ошибки.
		void error(ChannelReply::Error err, const QString& errMsg);

	protected:
		// !!! Должен вызывается в наследниках, при возникновенни любых ошибок.
		// Испускает сигнал error.
		// Если вызвана в момент выполнения запроса, устанавливает ошибку для текущего запроса в err и завершает его.
		// Если ошибка транспортного уровня(TransportError), разрывается соединение с устройством.
		void setError(ChannelReply::Error err, const QString& errMsg);

		// !!! Должен вызваться в наследниках при успешном установлении связи.
		// Испускает сигнал connectedToDevice.		
		void onConnectedToDevice();

		// !!! Должен вызваться в наследниках при успешном разрыве связи.
		// Испускает сигнал disconnectedFromDevice.	
		// При разрыве связи с устройством, все запросы находящиеся в очереди завершаются с ошибкой "Connection lost".
		void onDisconnectedFromDevice();

		// !!! Должен вызывается в наследниках, после получения данных от устройства.
		// Обратботка данных полученых от устройства.		
		void onReceivedData(const QByteArray& data);		
		
		// Упаковка данных перед отправлением в устройство.
		// Переопределяется в наследниках при необходимости.
		// Базовая реализация ничего не делает, т.е. возвращает те же данные.
		virtual QByteArray packBeforeSend(const QByteArray &data) const;

		// Распаковка данных полученных из устройства.
		// Переопределяется в наследниках при необходимости.
		// Базовая реализация ничего не делает, т.е. возвращает те же данные.
		virtual QByteArray unpackAfterRead(const QByteArray &data) const;

		// Указывает обязательно ли ждать ответа на запрос к устройству.
		// В случае если возвращает true, вне зависимости от значения свойства needResponse в запросе, ожидается ответ.
		// В случае если возвращает false, ответ на запрос ожидается в зависимости от значения свойства needResponse в запросе. 
		// Исключение: если запрос адресован к плате, ответ ожидается всегда.
		// Базовая реализация возвращает false. 
		virtual bool isAlwaysResponseNecessary() const;

		// Установление связи с устройством.
		// Определяется в наследнике реализующем конкретный канал связи
		virtual void doConnectToDevice() = 0;

		// Разрыв связи с устройством.
		// Определяется в наследнике реализующем конкретный канал связи
		virtual void doDisconnectFromDevice() = 0;

		// Отправка данных в устройство.
		// Определяется в наследнике реализующем конкретный канал связи
		virtual void doSendData(const QByteArray& data) = 0;

		// Добавление информационного сообщения.
		void addInfoMsg(const QString &msg);

		// Упаковка массива данных data для отправки через мастера в слейв по Rs485 порту port, абоненту abNum.  
		static QByteArray encapsulate(uint8_t port, uint8_t abNum, const QByteArray &data);		

	private:
		// Добавить запрос в очередь.
		// Возвращает умный указатель на обьект ChannelReply (Ответ от устройства).
		ChannelReply* addToQueue(const ChannelRequest& request);
 
		// Запланировать выполнение следующего запроса.
		void scheduleNext();

		// Начать выполнение следующего запроса из очереди. 
		// Формирование данных запроса и вызов функции отправки данных в устрйство.
		void execNext();

		// Закончить выполнение текущего зарпоса с ошибкой err и описанием ошибки errMsg.
		// При аргументах по умолчанию, запрос выполнен успешно.
		void finishNext(ChannelReply::Error err = ChannelReply::NoError, const QString& errMsg = QString());

	private:
		std::queue<ChannelReply*> m_queue;
		bool m_isScheduledNext;
	};

	using AsyncChannelPtr = std::shared_ptr<AsyncChannel>;
} // namespace