#include "Channel.h"
#include "qsettings.h"

#include <qthread.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <qdebug.h>
#include <QCoreApplication>

#include <math.h>


#include <dpc/sybus/utils.h>

#define DISABLE_DEBUG_VALUES Restorer<bool> __ddv(m_allowDebugPackValues, false);

namespace {
	using namespace Dpc::Sybus;

	// Константы
	const uint8_t MSG_TYPE_REQUEST = 0x0;
	const uint8_t MSG_TYPE_RESPONSE = 0x1;

	const uint16_t REQUEST_WRITE = 0x01;
	const uint16_t REQUEST_WRITE_EXTENDED = 0x41;
	const uint16_t REQUEST_READ = 0x21;
	const uint16_t REQUEST_READ_EXTENDED = 0x61;
	const uint16_t REQUEST_ENCAPSULATE = 0x1E;
	
	const uint16_t DEFAULT_BUFFER_SIZE = 0x40;
    const uint16_t DEFAULT_UPDATE_BUFFER_SIZE = 0x80;

	const QString BOARD_LOGIN = "admin";
	const QString BOARD_PASSWORD = "admin";

	const QString FACTORY_LOGIN = "DepAdmin";
	const QString FACTORY_PASSWORD = "DepAdmin";

	const QString FILE_ACCESS_R = "RO";
	const QString FILE_ACCESS_W = "RW";

	const uint16_t TIME_RESTART = 4000; // msec, Задержка после рестарта
	const quint32 TIMEOUT_FORMAT_DRIVE = 1200000;// 20 * 60 * 1000; // msec, Вермя на форматирование тома
	const uint16_t MAX_IDLE_TIME = 1000; // msec, максимальное время простоя, после которого устройство рвёт соединение

	const QString SETTING_SPLITER = ";";

	namespace SystemErrors {
		const QHash <uint8_t, QString> text = {
			{Channel::InvalidResponseMsgType, "Неверный формат ответа"},
			{Channel::UnexpectedParamType, "Тип параметра в ответе отличается от ожидаемого"},
			{Channel::NotSetLoginPassword, "Не задан логин или пароль"},
			{Channel::FailedMinimalMode, "Не удалось перейти в минимальный режим"},
			{Channel::InvalidDriveState, "Неверное состояние тома"},
			{Channel::TimeoutFormatDrive, "Таймаут форматирования тома"},
			{Channel::FailedWriteFile, "Ошибка записи файла"}
		};
	}

	namespace DeviceErrors {
		const QHash <uint8_t, QString> text = {
			{Channel::InvalidOperation, "Неверная операция(ресурс недоступен)"},
			{Channel::Busy, "Устройство занято(запрос принят но не может  быть выполнен)"},
			{Channel::ProtectedCmd, "Операция запрещена"},
			{Channel::BadID, "Параметр не поддерживается"},
			{Channel::InvalidType, "Неверный Тип данных"},
			{Channel::InvalidIndex, "Неверный индекс"},
			{Channel::InvalidCount, "Ошибка количества или соотношения индекса + количество"},
			{Channel::Protected, "Параметр защищен"},
			{Channel::InvalidLoginPassword, "Неверный логин или пароль"},
			{Channel::InvalidDevice, "Неверное устройство(плата)"},
			{Channel::InvalidValue, "Неверное значение параметра"},
		};
	}

	namespace FileSystemErrors {
		const QHash <uint8_t, QString> text = {
			{Channel::InvalidOperation, "Отказ"},
			{Channel::Busy, "Операция невозможна"},
			{Channel::IlgCmd, "Неизвестная команда"},
			{Channel::BadCmd, "Неправильный формат команды"},
			{Channel::BadLenCmd, "Неправильный формат команды"},
			{Channel::BadPar, "Неверный параметр"},
			{Channel::ProtectedCmd, "Операция запрещена"},
			{Channel::BadID, "Несуществующий параметр"},
			{Channel::InvalidIndex, "Достигнут конец файла"},
			{Channel::Protected, "Файл защищен"},
			{Channel::FileNotFound, "Файл не найден"},
			{Channel::InvalidLoginPassword, "Неверный логин или пароль"},
			{Channel::InvalidValue, "Неверное значение параметра"}			
		};
	}

	// Структуры для взаимодействий с устройством
#pragma pack(push, 1)
	struct BaseRequest 
	{
		uint8_t m_code;
		uint16_t m_addr;
	};

	struct ReadRequest : BaseRequest
	{
		ReadRequest(uint16_t addr, uint16_t index, uint8_t count) : 
			BaseRequest{ REQUEST_READ, addr }, m_count(count), m_index(index) {};

		uint8_t m_count;
		uint16_t m_index;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ReadRequest)); }
	};

	struct ExtendedReadRequest : BaseRequest
	{
		ExtendedReadRequest(uint16_t addr, uint16_t subAddr, uint16_t index, uint8_t count) :
			BaseRequest{ REQUEST_READ_EXTENDED, addr }, m_subAddr(subAddr), m_count(count), m_index(index) {};

		uint16_t m_subAddr;
		uint8_t m_count;
		uint16_t m_index;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ExtendedReadRequest)); }
	};

	struct WriteRequest : BaseRequest
	{
		WriteRequest() = default;
		WriteRequest(uint16_t addr, uint16_t index, uint8_t type, uint8_t count, const QByteArray &data) : 
			BaseRequest{ REQUEST_WRITE, addr }, m_type(type), m_count(count), m_index(index), m_data(data) {};

		uint8_t m_type;
		uint8_t m_count;
		uint16_t m_index;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(WriteRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct ExtendedWriteRequest : BaseRequest
	{
		ExtendedWriteRequest() = default;
		ExtendedWriteRequest(uint16_t addr, uint16_t subAddr, uint16_t index, uint8_t type, uint8_t count, const QByteArray &data) :
			BaseRequest{ REQUEST_WRITE, addr }, m_subAddr(subAddr), m_type(type), m_count(count), m_index(index), m_data(data) {};

		uint16_t m_subAddr;
		uint8_t m_type;
		uint8_t m_count;
		uint16_t m_index;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(ExtendedWriteRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct EncapsulatedRequest
	{
		EncapsulatedRequest() = default;
		EncapsulatedRequest(uint8_t port, uint8_t abNum, const QByteArray &data) :
			m_port(port), m_abNum(abNum), m_sum(0), m_data(data) 
		{
			m_len = data.size();

			for (size_t i = 0; i < m_hdrLen - 1; i++)
				m_sum += ((uint8_t*)this)[i];
			m_sum = ~m_sum;
		}

		const uint8_t m_hdrType = REQUEST_ENCAPSULATE;
		const uint8_t m_hdrLen = sizeof(EncapsulatedRequest) - sizeof(QByteArray);
		uint16_t m_len;
		uint8_t m_port;
		uint8_t m_abNum;
		const uint8_t m_rsv = 0x0;
		uint8_t m_sum;
		QByteArray m_data;

		QByteArray toByteArray() const { return QByteArray((const char*)this, sizeof(EncapsulatedRequest) - sizeof(QByteArray)) + m_data; }
	};

	struct BaseResponse
	{
		uint8_t taskNum : 5;
		uint8_t needRestart : 1;
		uint8_t isBusy : 1;
		uint8_t msgType : 1;	
		uint8_t ret;
	};

	struct WriteResponse : BaseResponse
	{
		WriteResponse(const QByteArray buff) {
			memcpy(this, (unsigned char *) buff.data(), sizeof(WriteResponse));
		}

		uint16_t msec = 0;
	};

	struct ReadResponse : BaseResponse
	{
		ReadResponse(const QByteArray &buff) {
			size_t structSize = sizeof(ReadResponse) - sizeof(QByteArray);
			memcpy(this, (unsigned char *) buff.data(), structSize);
			data = QByteArray(buff.data() + structSize, buff.size() - structSize);
		}

		uint8_t type = 0;
		QByteArray data;
	};
#pragma pack(pop)

	// Вспомогательные классы

	struct DisabledErrorsScope
	{
		Channel *channel;
		bool isValid;
        bool restoreSilentMode;

        DisabledErrorsScope(Channel *ch, bool silentMode = false) : channel(ch), isValid(false), restoreSilentMode(false) {
			if (ch->isErrorsEnabled()) {
				isValid = true;
				ch->setErrorsEnabled(false);
			}

            if (silentMode && !ch->isSilentMode()) {
                restoreSilentMode = true;
                ch->setSilentMode(true);
            }
		}

		~DisabledErrorsScope() {
            if (restoreSilentMode)
                channel->setSilentMode(false);
			if (isValid)
				channel->setErrorsEnabled(true);
		}
	};	

	struct MinimalModeScope	
	{
		Channel *channel;
		bool isValid;
		MinimalModeScope(Channel *ch) : channel(ch) { isValid = channel->restartDevice(Channel::MinimalModeReset); }
		~MinimalModeScope() { if (!isValid) return; channel->restartDevice(); }
	};

	// Вспомогательные функции

	// Основное устройство(мастер) или дополнительная плата(слейв)
	bool isMaster(int board) { return board < 0; }
} // namespace

namespace Dpc::Sybus
{
	struct Channel::OpenFileScope
	{
		Channel* ch;
		QString fileName;
		bool isValid;

		OpenFileScope(Channel* channel, const QString& file, const QString& openMode) : ch(channel), fileName(file) {
			isValid = ch->openFile(fileName, openMode);
		}

		~OpenFileScope() {
			if (isValid) {
				auto error = std::make_tuple(ch->errorType(), ch->errorCode(), ch->errorString());
				ch->closeFile(fileName);
				ch->m_errorType = std::get<0>(error);
				ch->m_errorCode = std::get<1>(error);
				ch->m_errorString = std::get<2>(error);
			}
		}
	};

	Channel::Channel(const SettingsType& settings, QObject* parent) :
		QObject(parent),
		m_login(settings.value(LoginSetting).toString()),
		m_password(settings.value(PasswordSetting).toString())
	{
		qRegisterMetaType<Dpc::Sybus::Channel::ErrorType>();

		m_readBufferSize = DEFAULT_BUFFER_SIZE;
		m_writeBufferSize = DEFAULT_BUFFER_SIZE;
		m_fileDataSize = 0;
		m_alreadyRecivedBuffersSize = false;
		m_errorType = NoErrorType;
		m_errorCode = NoError;
		m_isErrorsEnabled = true;
        m_isSilentMode = false;
		m_waitAfterReset = false;
		m_debugLevel = -1;
        m_allowDebugPackValues = true;
    }

    Channel::~Channel()
    {
//        qDebug() << "~Channel";
    }

	SettingsType Channel::settings() const
	{
		SettingsType res;
		res[TypeSetting] = type();
		res[LoginSetting] = m_login;
		res[PasswordSetting] = m_password;
		return res;
	}

	QString Channel::errorMsg() const
	{
		return formatErrorMsg(errorType(), errorCode(), errorString());
	}

	bool Channel::hasError() const
	{
		return errorType() != NoError;
	}

	void Channel::setErrorsEnabled(bool enabled)
	{
		if (m_isErrorsEnabled == enabled)
			return;

		m_isErrorsEnabled = enabled;
		if (enabled)
			emitError();
	}

	bool Channel::connect(bool makeLogin)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg("Установка соединения");

		clearError();
		if (!onConnect())
			return false;

		if (!m_alreadyRecivedBuffersSize && !getBuffersSize())
			return false;
		m_alreadyRecivedBuffersSize = true;

		if (makeLogin && !login())
			return false;

		return true;
	}

	bool Channel::disconnect()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg("Разрыв соединения");

		clearError();
		return onDisconnect();
	}

	int Channel::setParam(ParamPackPtr pack, int board)
	{
		DEBUG_LEVEL_UP;
		QString indexStr = QString::number(pack->index());
		if (pack->count() > 1)
			indexStr.append(QString("-%1").arg(pack->index() + pack->count() - 1));
		QString msg = QString("Запись параметра %1[%2]").arg(Sybus::toHex(pack->addr()), indexStr);
		if (!isMaster(board))
			msg.append(QString(" в плату %1").arg(board));
		if (m_allowDebugPackValues)
			msg = QString("%1: (%2)").arg(msg, pack->toString());
		addDebugMsg(msg);

		return write(pack, board);
	}

	ParamPackPtr Channel::param(uint16_t addr, uint16_t index, uint16_t count, int board)
	{
		DEBUG_LEVEL_UP;
		QString indexStr = QString::number(index);
		if (count > 1)
			indexStr.append(QString("-%1").arg(index + count - 1));
		QString msg = QString("Чтение параметра %1[%2]").arg(Sybus::toHex(addr), indexStr);
		if (!isMaster(board))
			msg.append(QString(" из платы %1").arg(board));
		addDebugMsg(msg);

		auto p = read(addr, index, count, board);
		if (p && m_allowDebugPackValues)
			addDebugMsg(QString("Значения: (%1)").arg(p->toString()));

		return p;
	}

	ParamAttribute Channel::attribute(uint16_t addr, int board)
	{
		DEBUG_LEVEL_UP;
		QString msg = QString("Чтение атрибутов параметра %1").arg(Sybus::toHex(addr));
		if (!isMaster(board))
			msg.append(QString(" из платы %1").arg(board));
		addDebugMsg(msg);

		auto p = param(SP_PARATTR, addr, 1, board);
		if (!p)
			return ParamAttribute();

		ParamAttribute res(p->value<uint16_t>());
		addDebugMsg(QString("Атрибуты: %1").arg(res.toString()));
		return res;
	}

	ParamDimension Channel::dimension(uint16_t addr, int board)
	{
		DEBUG_LEVEL_UP;
		QString msg = QString("Чтение размерности параметра %1").arg(Sybus::toHex(addr));
		if (!isMaster(board))
			msg.append(QString(" из платы %1").arg(board));
		addDebugMsg(msg);

		auto attr = attribute(addr, board);
		if (!attr)
			return ParamDimension();

		auto p = param(SP_PARDIM, addr, 1, board);
		if (!p)
			return ParamDimension();

		uint8_t profileCount = attr[ParamAttribute::_2D] ? p->value<uint8_t>(1) : 1;
		uint16_t subProfileCount = attr[ParamAttribute::_2D] ? p->value<uint8_t>(0) : p->value<uint16_t>();
		ParamDimension pd(profileCount, subProfileCount);
		addDebugMsg(QString("Размерность: %1").arg(pd.toString()));
		return pd;
	}

	ParamPackPtr Channel::names(uint16_t addr, int board)
	{
		DEBUG_LEVEL_UP;
		QString msg = QString("Чтение имён параметра %1").arg(Sybus::toHex(addr));
		if (!isMaster(board))
			msg.append(QString(" из платы %1").arg(board));
		addDebugMsg(msg);

		DisabledErrorsScope des(this);
		auto p = read(SP_PARNAME, 0, std::numeric_limits<uint16_t>::max(), board, addr);
		if (p && DeviceErrorType == errorType() && InvalidIndex == errorCode())
			clearError();

		if (p)
			addDebugMsg(QString("Значения: (%1)").arg(p->toString()));

		return p;
	}

	bool Channel::login(int board)
	{
		return execAuthorization(board, false);
	}

	bool Channel::fileSystemLogin()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg("Авторизация в файловую систему устройства");

		clearError();
		return setFileSystemRequest(QString("LGIN %1 %2").arg(m_login, m_password));
	}

	bool Channel::fileSystemQuit()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg("Выход из файловой системы устройства");

		clearError();
		return setFileSystemRequest(QString("QUIT"));
	}

	bool Channel::downloadFile(const QString& sourceFileName, const QString& destinationFileName)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Загрузка файла %1 из устройства в файл %2 ...").arg(sourceFileName, destinationFileName));

		if (!getFileDataSize())
			return false;

		OpenFileScope ops(this, sourceFileName, FILE_ACCESS_R);
		if (!ops.isValid)
			return false;

		int fileSize = getFileSize(sourceFileName);
		if (fileSize < 0)
			return false;

		auto p = ParamPack::create(T_DWORD, SP_FILE_DATAOFFS);
		p->appendValue(0);
		if (!setParam(p))
			return false;

		QFile file(destinationFileName);
		if (!file.open(QIODevice::WriteOnly)) {
			setError(SystemErrorType, file.error(), file.errorString());
			return false;
		}

		DISABLE_DEBUG_VALUES;
		size_t bufferSize = std::min(m_fileDataSize, readBufferSize()); // наименьшее из буфера чтения и размерности параметра SP_FILE_DATA.
		size_t writtenSize = 0;
		do {
			size_t requestSize = fileSize - writtenSize > bufferSize ? bufferSize : fileSize - writtenSize;
			auto p = param(SP_FILE_DATA, 0, requestSize);
			if (!p || hasError()) {
				if (Channel::Busy != errorCode()) {
					if (Channel::InvalidIndex == errorCode()) {
						auto remainSize = fileSize - writtenSize;
						if (p)
							remainSize -= p->count();
						
						addDebugMsg(QString("Потеряно %1 байт").arg(remainSize));
					}

					file.remove();
					return false;
				}

				addDebugMsg(QString("Устройство занято, повтор запроса"));
				sleepFor(10);
				auto offsetP = ParamPack::create(T_DWORD, SP_FILE_DATAOFFS);
				offsetP->appendValue(writtenSize);
				if (!setParam(offsetP)) {
					file.remove();
					return false;
				}

				p = param(SP_FILE_DATA, 0, requestSize);
				if (!p || hasError()) {
					file.remove();
					return false;
				}
			}

			int wr = file.write(p->data());
			if (wr < 0) {
				file.remove();
				setError(SystemErrorType, file.error(), file.errorString());
				return false;
			}

			if (wr != p->count()) {
				file.remove();
				setError(SystemErrorType, FailedWriteFile);
				return false;
			}

			writtenSize += p->count();
			int progressState = static_cast<double>(writtenSize) / fileSize * 100;
			emitProgress(progressState);

			// для отладки
			/*auto offRead = param(SP_FILE_DATAOFFS);
			if (!offRead) {
				if (Channel::Busy != errorCode()) {
					file.remove();
					return false;
				}

				addDebugMsg(QString("Устройство занято, повтор запроса"));
				sleepFor(10);
				offRead = param(SP_FILE_DATAOFFS);
				if (!offRead) {
					file.remove();
					return false;
				}
			}
			auto offReadVal = offRead->value<uint32_t>();
			if (offReadVal != writtenSize)
				addDebugMsg(QString("Считано %1 байт, указатель на устройстве на %2").arg(writtenSize).arg(offReadVal));*/

			
		} while (writtenSize < fileSize);

		return true;
	}

	bool Channel::uploadFile(const QString& sourceFileName, const QString& destinationFileName)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Передача файла %1 в файл %2 на устройстве...").arg(sourceFileName, destinationFileName));

		if (!getFileDataSize())
			return false;

		QFile localFile(sourceFileName);
		if (!localFile.open(QIODevice::ReadOnly)) {
			setError(SystemErrorType, localFile.error(), localFile.errorString());
			return false;
		}

		OpenFileScope ops(this, destinationFileName, FILE_ACCESS_W);
		if (!ops.isValid)
			return false;

		DISABLE_DEBUG_VALUES;
		size_t bufferSize = std::min(m_fileDataSize, writeBufferSize()); // наименьшее из буфера записи и размерности параметра SP_FILE_DATA.
		size_t uploadedCount = 0;
		size_t fileSize = QFileInfo(localFile).size();
		QByteArray fileData;
		while (!(fileData = localFile.read(bufferSize)).isEmpty()) {
			auto p = ParamPack::create(T_BYTE, SP_FILE_DATA);
			p->appendData(fileData);
			if (!setParam(p))
				return false;

			uploadedCount += fileData.size();
			int progressState = static_cast<double>(uploadedCount) / fileSize * 100;
			emitProgress(progressState);
		}

		if (localFile.error() != QFile::NoError) {
			setError(SystemErrorType, localFile.error(), localFile.errorString());
			return false;
		}

		return true;
	}

	bool Channel::deleteFile(const QString& fileName)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Удаление файла %1 на устройстве").arg(fileName));

		DisabledErrorsScope des(this);
		if (setFileSystemRequest(QString("DELF %1").arg(fileName)))
			return true;

		checkFileNotFoundError();
		return false;
	}

	bool Channel::mkdirectory(const QString& path)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Создание директории %1 на устройстве").arg(path));

		return setFileSystemRequest(QString("MKDR %1").arg(path));
	}

	QStringList Channel::directoryFileList(const QString& devicePath)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Получения списка файлов и папок из папки %1 на устройстве...").arg(devicePath));

		DisabledErrorsScope des(this);
		if (!setFileSystemRequest(QString("LIST %1").arg(devicePath))) {
			checkFileNotFoundError();
			return QStringList();
		}

		QStringList result;
		QString item;
		do {
			QString line = getFileSystemResponse();
			if (line.isEmpty())
				return QStringList();

			QString prefix = "LIST>>";
			if (!line.contains(prefix))
				return result;

			item = line.split(prefix).value(1).trimmed();
			if (item == "./" || item == "../")
				continue;

			if (!item.isEmpty())
				result << item;

		} while (!item.isEmpty());

		addDebugMsg(QString("Список файлов и папок: (%1)").arg(result.join(", ")));
		return result;
	}

	bool Channel::formatDrive(int drive)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Форматирование тома %1 ...").arg(drive));

		MinimalModeScope mm(this);
		if (!mm.isValid)
			return false;

		// Для выполнения операции необходима авторизация под производителем
		if (!execAuthorization(-1, true))
			return false;

		if (!setEnableDebugService(true))
			return false;

		if (!setFormatDrive(drive))
			return false;

		const int STATE_FORMATING = 3;
		const int STATE_GOOD = 5;

		QElapsedTimer  timer;
		timer.start();
		int driveState = STATE_FORMATING;
		while (driveState == STATE_FORMATING && timer.elapsed() < TIMEOUT_FORMAT_DRIVE) {
			driveState = getDriveState(drive);
			if (driveState < 0)
				return false;
			if (driveState == STATE_GOOD)
				break;

			{
				DEBUG_LEVEL_UP
				sleepFor(MAX_IDLE_TIME - 100);
			}
		}

		if (driveState != STATE_GOOD) {
			setError(SystemErrorType, timer.elapsed() < TIMEOUT_FORMAT_DRIVE ? InvalidDriveState : TimeoutFormatDrive);
			return false;
		}

		return true;
	}

    bool Channel::updateDevice(const QString& fileName, int board)
    {
		typedef enum : quint16 {
			UPG_PreLoadImg = 0x1234, /*предварительно загруженный образ  ПО*/
			UPG_PreLoadImgWithBoot = 32,	/* предварительно загруженный образ  ПО  и загрузчика*/
			UPG_OnLine = 0x4321		/*обновление в реальном времени (не используется)*/
		}UpgradeType;
#pragma pack(push, 1)
		typedef struct ImageFileHdr    /*header of PC image file 28 байт*/
		{
			quint32 FwStrtOffs;		/*смещение  кода  от  начала  памяти программ */
			quint32 size;			/*размер  в байтах*/
			quint16 poID;			/*код ПО*/
			UpgradeType UpgID;			/*тип обновления */
			quint8 MD5Hash[16];	/*16 байт MD5 образа ПО*/
		}ImageFileHdr_Type;
#pragma pack(pop)

        const QString device = isMaster(board) ? "устройства" : QString("платы %1").arg(board);
        const QString msg = QString("Обновление %1").arg(device);

        addDebugMsg(msg);

		QFile localFile(fileName);
		if (!localFile.open(QIODevice::ReadOnly)) {
			setError(SystemErrorType, localFile.error(), localFile.errorString());
			return false;
		}

        const size_t ImageFileHdr_Size = sizeof(ImageFileHdr_Type); //28 байт
		size_t uploadedCount = 0;
		quint16 uploadedPart = 0;
		quint16 uploadedProfile = 0;
		size_t fileSize = QFileInfo(localFile).size();
        if (fileSize < ImageFileHdr_Size)
		{
			setError(SystemErrorType, 0, "Выбран не корректный файл обновления устройства (слишком маленький).");
			return false;
		}
        QByteArray headerData = localFile.read(ImageFileHdr_Size);
        ImageFileHdr_Type headerInfo = *(reinterpret_cast<ImageFileHdr_Type*>(headerData.data()));
		if (headerInfo.size != fileSize)
		{
			setError(SystemErrorType, 0, "Выбран не корректный файл обновления устройства (не правильный размер).");
			return false;
		}
		if (headerInfo.UpgID != UPG_PreLoadImg)
		{
			setError(SystemErrorType, 0, QString("Файл обновления не поддерживается (%1).").arg(headerInfo.UpgID));
			return false;
		}
		if (headerInfo.poID != param(SP_SOFTWARE_ID, 0, 1, board)->value<quint16>())
		{
			setError(SystemErrorType, 0, QString("Файл обновления не подходит для устройства (%1).").arg(headerInfo.poID));
			return false;
		}

		// Для выполнения операции необходима авторизация под производителем
		if (!execAuthorization(board, true))
			return false;
        {
            auto p = ParamPack::create(T_BYTE, SP_FIRMUPGRADE); //28 байт
            //записать ImageFileHdr прочитанный из ISO файла
            p->appendData(headerData);
            emitProgress(ImageFileHdr_Size / (fileSize * 100.0));
            if (!setParam(p, board)) {
                return false;
            }
        }
        DISABLE_DEBUG_VALUES;
//        size_t bufferSize = std::min(DEFAULT_UPDATE_BUFFER_SIZE, writeBufferSize()); // наименьшее из буфера записи и размерности параметра SP_FILE_DATA.
        size_t bufferSize = DEFAULT_UPDATE_BUFFER_SIZE;                              // В текущем алгоритме буффер обязательно должен быть кратен 64.
		QByteArray fileData;
		while (!(fileData = localFile.read(bufferSize)).isEmpty()) {
			if (uploadedPart == 0)
			{
                auto p1 = ParamPack::create(T_DWORD, SP_EXTENADDRESS, 0);
				p1->appendValue((quint32)((quint32)uploadedProfile | ((quint32)(uploadedPart) << 16)));
				if (!setParam(p1, board)) {
                    restartDevice(HardReset, board);
					return false;
				}
                uploadedProfile++;
			}


            auto p0 = ParamPack::create(T_BYTE, SP_FIRMWARECODE, uploadedPart);
			uploadedPart += fileData.size();
			uploadedCount += fileData.size();
			p0->appendData(fileData);
			if (!setParam(p0, board)) {
                restartDevice(HardReset, board);
				return false;
			}
			int progressState = static_cast<double>(uploadedCount) / fileSize * 100;
            emitProgress(progressState);
		}

		if (localFile.error() != QFile::NoError) {
			setError(SystemErrorType, localFile.error(), localFile.errorString());
			restartDevice(HardReset, board);
			return false;
		}

		{
            auto p2 = ParamPack::create(T_BYTE, SP_FIRMUPGRADE);
			//записать подтвеждающий обновление бит
			p2->appendValue(0);

            if (!setParam(p2, board)) {
                restartDevice(HardReset, board);
				return false;
            }
        }
        disconnect();
        restartDevice(HardReset, board);
        m_alreadyRecivedBuffersSize = false;
        m_fileDataSize = 0;

        bool isSucses = false;
        {
            DisabledErrorsScope des(this, true);
            QFileInfo fi(fileName);
            QSettings s = QSettings(QDir(fi.absolutePath()).filePath( fi.baseName() + ".ini"), QSettings::IniFormat);
            const uint timeout_count = s.value("SYSTEM_SET/DEATH_TIME", 20).toInt() * 1000;
            QElapsedTimer et;
            et.start();
            while (!isSucses && (et.elapsed() < timeout_count)) {
                // Без задержки нестабильное поведение при работе через VCOM.
                // Переодические подвисание перед открытием соединения на последующую глобальную операцию.
                QThread::msleep(1000);
                isSucses = connect(false);
            }
        }

        if (!isSucses) {
            setError(SystemErrorType, 0, QString("Устройство не отвечает по установленому таймауту"));
            return false;
        }

		return true;
	}

	bool Channel::restartDevice(ResetMode mode, int board)
	{
		DEBUG_LEVEL_UP;
		QString device = isMaster(board) ? "устройства" : QString("платы %1").arg(board);
		QString msg = QString("Перезагрузка %1").arg(device);
		if (mode == MinimalModeReset)
			msg.append(" в минимальный режим");
		addDebugMsg(msg);

		auto p = ParamPack::create(T_BYTE, SP_SOFTRESET);
		p->appendValue(mode);
		if (!setParam(p, board))
			return false;

		disconnect();
		{
			DEBUG_LEVEL_UP
				sleepFor(TIME_RESTART);
		}

		if (MinimalModeReset == mode) {
			if (!connect(false))
				return false;

			DEBUG_LEVEL_UP;
			addDebugMsg("Проверка минимального режима");

			auto p = param(SP_SM_ENTITY_STATUS, 7);
			if (!p)
				return false;

			if (!p->value<uint8_t>()) {
				setError(SystemErrorType, FailedMinimalMode);
				return false;
			}
		}

		return true;
	}

	bool Channel::saveConfig(int board)
	{
		DEBUG_LEVEL_UP;
		QString device = isMaster(board) ? "устройстве" : QString("плате %1").arg(board);
		addDebugMsg(QString("Сохранение конфигурации на %1").arg(device));

		auto p = ParamPack::create(T_BYTE, SP_SAVECFG);
		p->appendValue(1);
		return setParam(p, board);
	}

	bool Channel::resetDevice(int board)
	{
		DEBUG_LEVEL_UP;
		QString device = isMaster(board) ? "устройстве" : QString("плате %1").arg(board);
		addDebugMsg(QString("Сброс конфигурации в заводские настройки на %1").arg(device));

		auto p = ParamPack::create(T_BYTE, SP_RESETDATA);
		p->appendValue(1);
		return setParam(p, board);
	}

	bool Channel::setDeviceWorkMode(WorkingMode mode)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Установка режима работы устройства в режим '%1'").arg(mode == Work ? "Рабочмй" : "Подготовка"));

		auto p = ParamPack::create(T_BYTE, SP_NEWPROFILE);
		p->appendValue(mode);
		return setParam(p);
	}

	QDateTime Channel::dateTime()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Чтение даты и времени на устройстве"));

		auto p = param(SP_DATETIME);
		if (!p)
			return QDateTime();

		QDateTime dt = QDateTime::fromSecsSinceEpoch(p->value<uint32_t>(), Qt::UTC);		
		addDebugMsg(QString("Системные дата и время на устройстве: %1").arg(dt.toString("dd.MM.yyyy hh:mm:ss")));
		return dt;
	}

	bool Channel::setDateTime(const QDateTime& dt)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Запись даты и времени(%1) в устройство").arg(dt.toString("dd.MM.yyyy hh:mm:ss")));
		
		{
			DEBUG_LEVEL_UP;
			addDebugMsg(QString("Коректировка протокола синхронизации"));
			if (!beginChangeDateTime())
				return false;
		}

		auto p = ParamPack::create(T_DATETIME, SP_DATETIME);
		p->appendValue(dt.toSecsSinceEpoch());
		auto result = setParam(p);

		{
			DEBUG_LEVEL_UP;
			addDebugMsg(QString("Востановление протокола синхронизации"));
			endChangeDateTime();
		}

		return result;
	}

	QVariant Channel::dateTimeOffset()
	{		
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Чтение смещения времени на устройстве"));
		auto resultMsg = QString("Смещение времени на устройстве в часах: %1");
		
		// Если есть параметр SP_TIMEZONE_HOUR, читаем из него.
		// В противном случае вычитывается на основе протокла синхронизации времени.
		// Если протокол синхронизации равен IEEE1588_UDP или IEEE1588_ETH, часовой пояс читается из SP_TIMESYNCROPARS[0][13].
		// Если протокол синхронизации SNTP, часовой пояс читается из SP_TIMESYNCROPARS[1][6]. 
		// В остальных случаях часовой пояс не поддерживается. Каким бы не были значения в параметрах, фактически оно считается равным 0.
		auto p = param(SP_TIMEZONE_HOUR);
		if (p) {
			auto offset = p->value<int8_t>();
			addDebugMsg(resultMsg.arg(offset));
			return offset;
		}

		if (Channel::BadID != errorCode())
			return QVariant();
		
		auto syncProtocol = getSyncProtocol();
		if (syncProtocol < 0)
			return QVariant();
		
		if (syncProtocol > SNTP) {
			addDebugMsg(resultMsg.arg("Не поддерживается протоколом синхронизации"));
			return QVariant();
		}

		if (syncProtocol == SNTP) {
			p = param(SP_TIMESYNCROPARS, 256 + 6);			
		}
		else {
			p = param(SP_TIMESYNCROPARS, 13);
		}

		if (!p)
			return QVariant();

		auto offset = p->value<int8_t>();
		addDebugMsg(resultMsg.arg(offset));
		return offset;
	}

	bool Channel::setDateTimeOffset(int8_t offset)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Установка смещения времени на устройстве в часах: %1").arg(offset));

		// Если есть параметр SP_TIMEZONE_HOUR, пишется в него.
		// В противном случае пишется в SP_TIMESYNCROPARS[0][13] и SP_TIMESYNCROPARS[1][6]. 
		auto p = ParamPack::create(T_SBYTE, SP_TIMEZONE_HOUR);
		p->appendValue(offset);
		if (setParam(p))
			return true;

		if (Channel::BadID != errorCode())
			return false;

		p = ParamPack::create(T_SBYTE, SP_TIMESYNCROPARS, 13);
		p->appendValue(offset);
		if (!setParam(p))
			return false;

		p = ParamPack::create(T_SBYTE, SP_TIMESYNCROPARS, 256 + 6);
		p->appendValue(offset);
		if (!setParam(p))
			return false;

		return true;
	}

	QString Channel::formatErrorMsg(Dpc::Sybus::Channel::ErrorType errType, int errCode, const QString& errMsg)
	{
		if (!errType)
			return QString();

		QString typeString;
		switch (errType)
		{
		case Channel::SystemErrorType: typeString = "системы"; break;
		case Channel::ChannelErrorType: typeString = "канального уровня"; break;
		case Channel::DeviceErrorType: typeString = "устройства"; break;
		case Channel::FileSystemErrorType: typeString = "файловой системы устройства"; break;
		default: break;
		}

		QString codeString;
		switch (errType)
		{
		case Channel::SystemErrorType:
		case Channel::ChannelErrorType: codeString = QString::number(errCode); break;
		case Channel::DeviceErrorType:
		case Channel::FileSystemErrorType: codeString = Sybus::toHex(uint8_t(errCode));
		default: break;
		}

		return QString("Ошибка %1(%2): %3").arg(typeString, codeString, errMsg);
	}

	bool Channel::beginChangeDateTime()
	{
		return true;
	}

	bool Channel::endChangeDateTime()
	{
		return true;
	}

	std::pair<uint16_t, uint16_t> Channel::onGetBuffersSize()
	{
		auto p = param(SP_MAX_BLOCK, 0, 2);
		if (!p)
			return std::make_pair(0, 0);

		return std::make_pair(p->value<uint16_t>(0), p->value<uint16_t>(1));
	}

	QByteArray Channel::packBeforeSend(const QByteArray& data) const
	{
		return data;
	}

	QByteArray Channel::unpackAfterRead(const QByteArray& data) const
	{
		return data;
	}

	int Channel::getSyncProtocol()
	{
		auto p = param(SP_PROTOCOL);
		if (!p)
			return -1;

		return p->value<uint8_t>();
	}

	bool Channel::setSyncProtocol(uint8_t protocol)
	{
		auto p = ParamPack::create(T_BYTE, SP_PROTOCOL);
		p->appendValue(protocol);
		return setParam(p);
	}

	void Channel::setError(ErrorType type, int code, const QString& text)
	{
		m_errorType = type;
		m_errorCode = static_cast<Error>(code);
		m_errorString = text;

		if (m_errorString.isEmpty()) {
			switch (type)
			{
			case Channel::SystemErrorType: m_errorString = SystemErrors::text[errorCode()]; break;
			case Channel::DeviceErrorType: m_errorString = DeviceErrors::text[errorCode()]; break;
			case Channel::FileSystemErrorType: m_errorString = FileSystemErrors::text[errorCode()]; break;
			default:  break;
			}
		}

		if (!isErrorsEnabled())
			return;

		emitError();
	}

	void Channel::clearError()
	{
		m_errorType = NoErrorType;
		m_errorCode = NoError;
		m_errorString = QString();
	}

	void Channel::emitError()
	{
		if (!errorType())
			return;

		emit error(errorType(), errorCode(), errorString());
	}

	void Channel::emitProgress(int state)
	{
		emit progress(state);
	}

	void Channel::addDebugMsg(const QString& msg)
    {
        if (isSilentMode())
            return;

		if (m_debugLevel)
			emit debug(m_debugLevel, msg);
		else
			emit info(msg);
	}

	QByteArray Channel::encapsulate(uint8_t port, uint8_t abNum, const QByteArray& data) const
	{
		return EncapsulatedRequest(port, abNum, data).toByteArray();
	}

	bool Channel::sendData(const QByteArray& data)
	{
		DEBUG_LEVEL_UP;

		auto packedData = packBeforeSend(data);
		addDebugMsg(QString("[SEND] (%1) -> ").arg(packedData.size()).append(packedData.toHex(':')).toUpper());
		return onSendData(packedData);
	}

	QByteArray Channel::readData()
	{
		DEBUG_LEVEL_UP;

		QByteArray data = onReadData();
		if (errorType() != ChannelErrorType)
			addDebugMsg(QString("[RECV] (%1) <- ").arg(data.size()).append(data.toHex(':')).toUpper());

		return unpackAfterRead(data);;
	}

	bool Channel::execWriteRequest(uint16_t addr, const QByteArray& requestData, int board)
	{
		quint8 slaveIdx = board & 0xFF;       // Младший байт
		quint8 portIdx = (board >> 8) & 0xFF; // Старший байт
		auto data = isMaster(board) ? requestData : encapsulate(portIdx, slaveIdx, requestData);
		if (!sendData(data))
			return false;

		bool isResetParam = addr == SP_SOFTRESET;
		if (isResetParam && isMaster(board) && !m_waitAfterReset)
			return true;

		QByteArray responseData = readData();
		if (errorType() != NoError)
			return false;

		WriteResponse response(responseData);
		if (response.msgType != MSG_TYPE_RESPONSE) {
			setError(SystemErrorType, InvalidResponseMsgType);
			return false;
		}

		if (response.ret != NoError) {
			if (isResetParam && response.ret == Busy)
				return true;

			onReadWriteParamError(addr, response.ret);
			return false;
		}

		if (response.isBusy) {
			addDebugMsg(QString("Устройство занято на %1 милисекунд").arg(response.msec));
			disconnect();
            sleepFor(response.msec);
		}

		return true;
	}

	std::pair<uint8_t, QByteArray> Channel::execReadRequest(uint16_t addr, const QByteArray& requestData, int board)
	{
		quint8 slaveIdx = board & 0xFF;       // Младший байт
		quint8 portIdx = (board >> 8) & 0xFF; // Старший байт
		auto data = isMaster(board) ? requestData : encapsulate(portIdx, slaveIdx, requestData);
		if (!sendData(data))
			return {};

		QByteArray responseData = readData();
		if (errorType() != NoError)
			return {};

		ReadResponse response(responseData);
		if (response.msgType != MSG_TYPE_RESPONSE) {
			setError(SystemErrorType, InvalidResponseMsgType);
			return {};
		}

		if (response.ret != NoError) {
			onReadWriteParamError(addr, response.ret);
			return {};
		}

		if (!response.type) {
			setError(SystemErrorType, UnexpectedParamType);
			return {};
		}

		return std::make_pair(response.type, response.data);
	}

	ParamPackPtr Channel::read(uint16_t addr, uint16_t index, uint16_t count, int board, uint16_t subAddr)
	{
		clearError();
		auto pack = ParamPackPtr();
		int readedCount = 0;
		while (readedCount < count) {
			static const uint8_t maxCountPerRead = 0xFF;
			uint8_t nextCount = readedCount + maxCountPerRead < count ? maxCountPerRead : count - readedCount;

			QByteArray requestData = subAddr ? ExtendedReadRequest(addr, subAddr, index + readedCount, nextCount).toByteArray() :
				ReadRequest(addr, index + readedCount, nextCount).toByteArray();
			auto res = execReadRequest(addr, requestData, board);
			if (!res.first)
				return pack;

			if (!pack)
				pack = std::make_shared<ParamPack>(res.first, addr, index);
			else if (res.first != pack->typeId()) {
				setError(SystemErrorType, UnexpectedParamType);
				return ParamPackPtr();
			}

			pack->appendData(res.second);
			readedCount = pack->count();
		}

		return pack;
	}

	int Channel::write(ParamPackPtr pack, int board, uint16_t subAddr)
	{
		clearError();
		int maxDataSize = writeBufferSize() - writeHeaderSize(board);
		int writtenCount = 0;
		while (writtenCount < pack->count()) {
			size_t nextCount = 0;
			QByteArray nextData;
			for (; writtenCount + nextCount < pack->count(); nextCount++) {
				auto data = pack->data(writtenCount + nextCount);
				if (nextData.size() + data.size() > maxDataSize)
					break;

				nextData.append(data);
			}

			uint16_t addr = pack->addr();
			uint16_t index = pack->index() + writtenCount;
			uint8_t type = pack->typeId();
			QByteArray requestData = subAddr ? ExtendedWriteRequest(addr, subAddr, index, type, nextCount, nextData).toByteArray() :
				WriteRequest(addr, index, type, nextCount, nextData).toByteArray();
			if (!execWriteRequest(pack->addr(), requestData, board))
				break;

			writtenCount += nextCount;
		}

		return writtenCount;
	}

	int Channel::writeHeaderSize(int board) const
	{
		int size = packBeforeSend(WriteRequest().toByteArray()).size();
		if (!isMaster(board))
			size += EncapsulatedRequest().toByteArray().size();
		return size;
	}

	void Channel::onReadWriteParamError(uint16_t addr, uint8_t errorCode)
	{
		bool isFileSystemParam = addr == SP_FILE_CTRLR ||
			addr == SP_FILE_CTRLW ||
			addr == SP_FILE_DATA ||
			addr == SP_FILE_DATAOFFS;

		auto errorType = isFileSystemParam ? FileSystemErrorType : DeviceErrorType;
		setError(errorType, errorCode);
	}

	void Channel::checkFileNotFoundError()
	{
		if (errorType() == FileSystemErrorType && errorCode() == InvalidOperation)
			setError(FileSystemErrorType, FileNotFound);
	}

	void Channel::sleepFor(int msec)
	{
		if (!msec)
			return;

		addDebugMsg(QString("Задержка %1 милисекунд").arg(msec));
		QThread::msleep(msec);
	}

	bool Channel::getBuffersSize()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg("Получение размеров буферов приема/переачи устройства");

		auto res = onGetBuffersSize();
		if (!res.first && !res.second)
			return false;

		// устанавливаем наоборот: буфер примеа dconf = буфер передачи утсройства и т.д.
		m_readBufferSize = res.second;
		m_writeBufferSize = res.first;

		addDebugMsg(QString("Буфер приема = %1, буфер передачи = %2").arg(res.first).arg(res.second));
		addDebugMsg(QString("Установка буфера приема в %1 и буфера передачи в %2").arg(readBufferSize()).arg(writeBufferSize()));
		return true;
	}

	bool Channel::getFileDataSize()
	{
		if (!m_fileDataSize) {
			auto dim = dimension(SP_FILE_DATA);
			if (!dim)
				return false;

			m_fileDataSize = dim.subProfileCount();
		}

		return true;
	}

	bool Channel::execAuthorization(int board, bool asFactory)
	{
		DEBUG_LEVEL_UP;
		QString device = isMaster(board) ? "устройстве" : QString("плате %1").arg(board);
		QString user = asFactory ? "производителем" : "администратором";
		addDebugMsg(QString("Авторизация на %1 под %2").arg(device, user));

		QString login = asFactory ? FACTORY_LOGIN : isMaster(board) ? m_login : BOARD_LOGIN;
		QString password = asFactory ? FACTORY_PASSWORD : isMaster(board) ? m_password : BOARD_PASSWORD;
		uint16_t addr = asFactory ? SP_SYSLOGIN : SP_NETLOGIN;

		clearError();
		if (login.isEmpty() || password.isEmpty()) {
			setError(SystemErrorType, NotSetLoginPassword);
			return false;
		}

		DISABLE_DEBUG_VALUES
			auto p = ParamPack::create(T_STRING, addr);
		p->appendValue(login);
		p->appendValue(password);
		return setParam(p, board);
	}

	bool Channel::setFileSystemRequest(const QString& request)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Запрос к файловой системе: %1").arg(request));

		auto p = ParamPack::create(T_STRING, SP_FILE_CTRLW);
		p->appendValue(request);
		return setParam(p);
	}

	QString Channel::getFileSystemResponse()
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Получение очередного овтета на запрос к файловой системе..."));

		auto p = param(SP_FILE_CTRLR);
		if (!p)
			return QString();

		QString response = p->value<QString>();
		addDebugMsg(QString("Ответ от файловой системы: %1").arg(response));
		return response;
	}

	int Channel::getFileSize(const QString& filename)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Получение размера файла %1 на устройстве").arg(filename));

		DisabledErrorsScope des(this);
		if (!setFileSystemRequest(QString("FATR %1").arg(filename))) {
			checkFileNotFoundError();
			return -1;
		}

		QString fatrstr = getFileSystemResponse();
		if (fatrstr.isEmpty())
			return -1;

		// Вырезаем то что находится между 'SIZE=' и ' kB'
		const QString beginStr = "SIZE=";
		const QString endStr = " kB";
		int beginIdx = fatrstr.indexOf(beginStr) + beginStr.size();
		int endIdx = fatrstr.indexOf(endStr);
		auto valList = fatrstr.mid(beginIdx, endIdx - beginIdx).split('.');
		int result = valList.value(0).toInt() * 1000 + valList.value(1).toInt();

		addDebugMsg(QString("Размера файла: %1 байт").arg(result));
		return result;
	}

	bool Channel::openFile(const QString& filename, const QString& openMode)
	{
		DEBUG_LEVEL_UP;
		QString openModeString = openMode == FILE_ACCESS_R ? "чтения" : "записи";
		addDebugMsg(QString("Открытие файла %1 в режиме %2 на устройстве").arg(filename, openModeString));

		DisabledErrorsScope des(this);
		if (setFileSystemRequest(QString("OPFL %1 %2").arg(filename).arg(openMode)))
			return true;

		checkFileNotFoundError();
		return false;
	}

	bool Channel::closeFile(const QString& filename)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Закрытие файла %1 на устройстве").arg(filename));

		DisabledErrorsScope des(this);
		if (setFileSystemRequest(QString("CLFL %1").arg(filename)))
			return true;

		checkFileNotFoundError();
		return false;
	}

	bool Channel::setEnableDebugService(bool enable)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("%1 блокировки отладочных сервисов").arg(enable ? "Отключение" : "Включение"));

		auto p = ParamPack::create(T_BYTE, SP_SERVICELOCK);
		p->appendValue(!enable);
		if (!setParam(p))
			return false;

		return true;
	}

	bool Channel::setFormatDrive(int drive)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Команда форматирования тома %1").arg(drive));

		auto p = ParamPack::create(T_BYTE, SP_FORMAT_VOLUME, drive);
		p->appendValue(1);
		if (!setParam(p))
			return false;

		return true;
	}

	int Channel::getDriveState(int drive)
	{
		DEBUG_LEVEL_UP;
		addDebugMsg(QString("Получение состояния тома %1").arg(drive));

		auto p = param(SP_VOLUME_STATE_IND, drive);
		if (!p)
			return -1;

		int32_t res = p->value<uint32_t>(0, -1);
		addDebugMsg(QString("Состояние тома: %1").arg(res));
		return res;
	}
} // namespace
