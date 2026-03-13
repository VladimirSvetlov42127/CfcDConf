#pragma once

#include <qobject.h>
#include <qdatetime.h>
#include <qvariant.h>

#include <dpc/sybus/channel/Common.h>
#include <dpc/sybus/ParamPack.h>
#include <dpc/sybus/ParamAttribute.h>
#include <dpc/sybus/ParamDimension.h>

#define DEBUG_LEVEL_UP Restorer<int> __dlu(m_debugLevel, m_debugLevel + 1);

namespace Dpc::Sybus
{
	class DPC_EXPORT Channel : public QObject
	{
		Q_OBJECT

	public:
		enum ResetMode {
			HardReset = 1,
			MinimalModeReset = 3
		};

		enum WorkingMode {
			Work = 0, 
			Prepare
		};

		enum ErrorType {
			NoErrorType = 0,
			SystemErrorType,	// Ошибки в логике программы.
			ChannelErrorType,	// Ошибки канального уровня, соединение и обмен с устройством. В основном от операционой системы
			DeviceErrorType,	// Ошибки приходящие в ответах от устройства
			FileSystemErrorType // Ошибки от устройства при работе с файловой системой		
		};

		enum Error {
			// Коды приходяшие от устройства
			NoError = 0,			
			InvalidOperation = 0x01, // Rejected
			Busy = 0x02,
			IlgCmd = 0x10,
			BadCmd = 0x20,
			BadLenCmd = 0x21,
			BadPar = 0x22,
			ProtectedCmd = 0x23,
			BadID = 0x24,
			InvalidType = 0x25,
			InvalidIndex = 0x26, // EndOfFile
			InvalidCount = 0x27,
			Protected = 0x42,
			FileNotFound = 0x50, // Не реальный код, введен по согласованию
			InvalidLoginPassword = 0x62,
			InvalidDevice = 0x80,
			InvalidValue = 0x84,

			// Системные коды		 
			InvalidEOF = 0x1000,
			InvalidCRC,
			InvalidASCII,
			InvalidResponseMsgType,
			UnexpectedParamType,
			NotSetLoginPassword,
			FailedMinimalMode,
			InvalidDriveState,
			TimeoutFormatDrive,
			FailedWriteFile
		};

		enum TSyncProtocol {
			IEEE1588_UDP = 0,
			IEEE1588_ETH = 1,
			SNTP = 2,
			lPPS = 3,
			SYBUS_TSYNC,
			IEC101_TSYNC,
			IEC_104_TSYNC,
			SYBUSTCP_TSYNC,
			MODBUSRTU_TSYNC,
			MODBUSTCP_TSYNC,
			SNTPGPRS_TSYNC,
			IEC_103_TSYNC,
			GSM_TSYNC,

			MAX_TSYNCPRT,
		};
	

		Channel(const SettingsType &settings, QObject *parent = nullptr);
        virtual ~Channel();

		// Тип канала связи
		virtual ChannelType type() const = 0;
		virtual SettingsType settings() const;

		virtual bool isConnectedToDevice() const = 0;

		// Тип ошибки
		ErrorType errorType() const { return m_errorType; }
		// Код ошибки
		Error errorCode() const { return m_errorCode; }
		// Расшифровка кода ошибки
		QString errorString() const { return m_errorString; }
		// Сообщение об ошибке в формате %1(%2): %3, где 1 - тип ошибки, 2 - код ошибки, 3 - расшифровка ошибки
		// Коды ошибок от устройства и файловой системы устройства выводятся в hex, остальные в обычном виде.
		QString errorMsg() const;
		bool hasError() const;

		// Включение и отключение сигнала ошибки
        bool isErrorsEnabled() const { return m_isErrorsEnabled; }
		void setErrorsEnabled(bool enabled);

        // Включение и отключнеие тихого режима(без info и debug сообщений)
        bool isSilentMode() const { return m_isSilentMode; }
        void setSilentMode(bool mode) { m_isSilentMode = mode; }

		// Установка соединения и разрыв с устройством
		bool connect(bool makeLogin = true);
		bool disconnect();

		// Установка значений параметров из пакета. Возвращает реально установленое количество.
		// Если установлены не все элементы пакета, будет возведена ошибка. 
		int setParam(ParamPackPtr pack, int board = -1);

		// Чтение параметра addr c индекса index, count элементов. 
		// При ошибке будет возврашен null или пакет с меньшим чем count элементов, при этом будет возведена ошибка.
		ParamPackPtr param(uint16_t addr, uint16_t index = 0, uint16_t count = 1, int board = -1);

		// Чтение атрибутов параметра
		ParamAttribute attribute(uint16_t addr, int board = -1);

		// Чтение размерности параметра
		ParamDimension dimension(uint16_t addr, int board = -1);

		// Чтение имен параметра 
		// 1. Если размерность параметра 1, будет возвращено 1 значение.
		// 2. Если параметр имеет размерность больше 1, но при этом возвращено 1 значение, это означает что у всех подпараметров одно и тоже имя.
		// 3. Если параметр имеет размерность больше 1, то будет возвращено N + 1 для 1-мерных параметров и М * N + 1 для 2х-мерных парамтеров имён.
		// При этом имя под индексом 0 в пакете, обобщенное имя параметра, а имена подпараметров начинаются с 1го индекса. 
		ParamPackPtr names(uint16_t addr, int board = -1);

		// Выполнить вход под администратором
		bool login(int board = -1);

		// Методы для работы с файловой системой. Для работы с файловой системой необходимо авторизоваться в нее
		bool fileSystemLogin();
		bool fileSystemQuit();
		bool downloadFile(const QString &sourceFileName, const QString &destinationFileName);
		bool uploadFile(const QString &sourceFileName, const QString &destinationFileName);
		bool deleteFile(const QString &fileName);
		bool mkdirectory(const QString &path);
		QStringList directoryFileList(const QString &devicePath);

		// Форматирование том.
		bool formatDrive(int drive);

		// Обновление ПО конфигурации и перезагрузка устройства. После перезагрузки ожидается таймаут.
        bool updateDevice(const QString &fileName, int board = -1);

		// Сохранение конфигурации и перезагрузка устройства. После перезагрузки ожидается таймаут.
		bool restartDevice(ResetMode mode = HardReset, int board = -1);		
		bool saveConfig(int board = -1);

		// Сброс конфигурации в заводские настройки
		bool resetDevice(int board = -1);

		// Установка режима работы устройства
		bool setDeviceWorkMode(WorkingMode mode);

		// Получение и установка времени устройства в UTC
		QDateTime dateTime();
		bool setDateTime(const QDateTime &dt);
		
		// Получение и установка смещения времени устройства в UTC
		// При ошибке или некоректно установленом протоколе синхронизации возвращается Invalid
		QVariant dateTimeOffset();
		bool setDateTimeOffset(int8_t offset);
		
		static QString formatErrorMsg(Dpc::Sybus::Channel::ErrorType errType, int errCode, const QString& errMsg);

	signals:
		// Отладочные сообщения, level - уровень вложености, начиная с 1.
		void debug(int level, const QString &msg);
		// Информационные сообщения - отладочные сообщения 0 уровня, сделано для удобсвта
		void info(const QString& msg);
		// Возникновение ошибки
		void error(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg);
		// Прогресс длительной операции. Передача или загрузка файла
		void progress(int state);

	protected:
		virtual bool onConnect() = 0;
		virtual bool onDisconnect() = 0;
		virtual bool onSendData(const QByteArray &data) = 0;
		virtual QByteArray onReadData() = 0;

		virtual bool beginChangeDateTime();
		virtual bool endChangeDateTime();

		// Первое значение буфер приема, второе значение буфер передачи
		virtual std::pair<uint16_t, uint16_t> onGetBuffersSize();
		virtual QByteArray packBeforeSend(const QByteArray &data) const;
		virtual QByteArray unpackAfterRead(const QByteArray &data) const;
		void setWaitAfterReset(bool wait) { m_waitAfterReset = wait; }
		int getSyncProtocol();
		bool setSyncProtocol(uint8_t protocol);

		void setError(ErrorType type, int code, const QString &text = QString());
		void clearError();
		void emitError();
		void emitProgress(int state);
		void addDebugMsg(const QString &msg);

		QByteArray encapsulate(uint8_t port, uint8_t abNum, const QByteArray &data) const;
		uint16_t readBufferSize() const { return m_readBufferSize; }
		uint16_t writeBufferSize() const { return m_writeBufferSize; }

	private:

		// Отправка данных в канальный уровень, перед отправкой происходит упаковка определеная в наследниках.
		// Также выдерживается необходимый таймаут, полученый на предыдущий запрос.
		bool sendData(const QByteArray &data);

		// Получение данных из канального уровня, перед возвращением происходит распаковка определеная в наследниках.
		QByteArray readData();

		// Выполнение запроса на запись. 
		bool execWriteRequest(uint16_t addr, const QByteArray &requestData, int board = -1);

		// Выполнение запроса на чтение. Возвращается первое значение - тип, второе - данные.
		std::pair<uint8_t, QByteArray> execReadRequest(uint16_t addr, const QByteArray &requestData, int board = -1);

		// Чтение параметра addr с индекса idx в количестве count.
		// При укзании subAddr используется расширеный формат запроса. При этом addr = номер параметра, с поддержкой расширенного формата,
		// а subAddr = номер параметра для чтения.
		// При ошибке будет возврашен null или пакет с меньшим чем count элементов, при этом будет возведена ошибка.
		ParamPackPtr read(uint16_t addr, uint16_t index = 0, uint16_t count = 1, int board = -1, uint16_t subAddr = 0);

		// Запись параметра из пакета.
		// При укзании subAddr используется расширеный формат запроса. Параметр из пакета = номер параметра, с поддержкой расширенного формата,
		// а subAddr = номер параметра для записи. Возвращает реально записаное количество.
		// Если записаны не все элементы пакета, будет возведена ошибка. 
		int write(ParamPackPtr pack, int board = -1, uint16_t subAddr = 0);

		int writeHeaderSize(int board = -1) const;
		void onReadWriteParamError(uint16_t addr, uint8_t errorCode);
		// Костыль для установки ошибки 'не найден файл' на устройстве. Сделано потому что от устройства такая ошибка не приходит.
		void checkFileNotFoundError();

		void sleepFor(int msec);
		bool getBuffersSize();
		bool getFileDataSize();
		bool execAuthorization(int board = -1, bool asFactory = false);

		bool setFileSystemRequest(const QString &request);
		QString getFileSystemResponse();
		int getFileSize(const QString &filename);
		bool openFile(const QString &filename, const QString &openMode);
		bool closeFile(const QString &filename);

		bool setEnableDebugService(bool enable);
		bool setFormatDrive(int drive);
		int getDriveState(int drive);

	private:
		class OpenFileScope;

	protected:
		int m_debugLevel;

	private:
		uint16_t m_readBufferSize;
		uint16_t m_writeBufferSize;
		uint16_t m_fileDataSize;
		bool m_alreadyRecivedBuffersSize;

		QString m_login;
		QString m_password;

		ErrorType m_errorType;
		Error m_errorCode;
		QString m_errorString;
		bool m_isErrorsEnabled;
        bool m_isSilentMode;

		bool m_waitAfterReset;		
		bool m_allowDebugPackValues;
	};

	using ChannelPtr = std::shared_ptr<Channel>;

	struct FileSystemLoginScope
	{
		ChannelPtr ch;
		bool isValid;

		FileSystemLoginScope(ChannelPtr channel) : ch(channel) { isValid = ch->fileSystemLogin(); }
		~FileSystemLoginScope() { if (isValid) ch->fileSystemQuit(); }
	};
} // namespace

Q_DECLARE_METATYPE(Dpc::Sybus::Channel::ErrorType)
