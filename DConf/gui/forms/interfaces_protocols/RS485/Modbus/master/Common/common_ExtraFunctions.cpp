#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h>

#include <qfileinfo.h>

#include <file_managers/DcModbusMasterSlavesFileManager.h>
#include <gui/forms/interfaces_protocols/RS485/PortInfo.h>

// Создает имя команды слейва
QString makeDefaultCommandName(uint16_t number, ModbusMaster_ns::channelType_t type, ModbusMaster_ns::count_t count, ModbusMaster_ns::startAddress_t address, bool isModify) {
	QString numberString = number == 0 ? "" : QString::number(number) + ". ";
	QString name;
	if(isModify)	// Если модифицированный, то без стартового адреса регистра
		name = numberString + ModbusMaster_ns::g_channelNames[type] + " " + QString::number(count);
	else 
		name = numberString + ModbusMaster_ns::g_channelNames[type] + " " + QString::number(address) + " " + QString::number(count);
	return name;
}

bool checkFileNameSize(const QString &name) {
	if (name.isEmpty())
		return false;
	
	return name.size() <= getMaxFileNameByteSize();
}

// Создает имя файла для хранения настроек слейва
QString makeDefaultSlaveConfigFileName(uint8_t slaveID) {
	QString name;
	name = ModbusMaster_ns::SLAVE_CONFIG_NAME + "%1.msl";
	name = name.arg(QString::number(slaveID));
	return name;
}

// Имя папки, в которой хранятся настройки слейвов
QString getModbusFolderName() {
	return ModbusMaster_ns::SLAVE_CONFIG_DIR;
}

// Получаем путь к файлу настроек слейва, хранящемуся в модели контроллера
QString getDefaultFilePathForLocalSave(DcController* controller, uint8_t slaveID) {
	QString path;

	if (controller) {
		QString name(makeDefaultSlaveConfigFileName(slaveID));
		QString folder = getFolderForLocalSave(controller);	// Добавляем в путь папку с хранением модбас настроек
		path = folder + name;	// Формируем полный путь
	}

	return path;
}

// Получаем путь к файлу настроек слейва, хранящемуся в устройстве
QString getFilePathForSaveInDevice(const QString &filename) {

	QString path = getFolderPathForSlaveInDevice() + filename;
	return path;
}

QString getFilePathForLocalSave(DcController* controller, const QString & filename)
{
	QString path;
	if (filename.isEmpty())
		return path;

	if (controller) {
		QString folder = getFolderForLocalSave(controller);	// Добавляем в путь папку с хранением модбас настроек
		path = folder + filename;	// Формируем полный путь
	}
	return path;
}

// Получаем путь к папке с настройками слейвов
QString getFolderForLocalSave(DcController* controller) 
{
	if (!controller)
		return QString();

	return QString("%1/").arg(DcModbusMasterSlavesFileManager(controller).localPath());
}

// Получаем путь как папке с настройками слейва, хранящихся на устройстве
QString getFolderPathForSlaveInDevice() {
	return QString("%1/").arg(DcModbusMasterSlavesFileManager().devicePath());
}

uint32_t getMaxByteCountForSlaveConfig() {
	uint32_t idBytes = 2, lenBytes = 2, crcBytes = 2;
	return 1000 - idBytes - lenBytes - crcBytes; // По Кбайту каждый файл слейва. Вычитаем длину заголовка
}

// Максимальный размер файла для хранения конфигурации слейва
uint8_t getMaxFileNameByteSize()
{
	int maxSizeForFilePathInDevice = 32;	// Кол-во байт под имя файла вместе с абсолютным путем на устройстве
	int folderNameSize = ModbusMaster_ns::DEVICE_DIR_FOR_SLAVE.size();
	int dimSize = 4; // 4 для записи ".msl"
	//int suffixBytes = 9; // "_p0s0.msl"
	return  maxSizeForFilePathInDevice - folderNameSize - dimSize;
	//return  maxSizeForFilePathInDevice - folderNameSize;
}

QString makeHexByteStringFromArray(const QByteArray& arr, uint16_t byteCount) {

	QString arrayStr;
	if (byteCount > arr.size())
		return arrayStr;

	uint16_t count = byteCount == 0 ? arr.size() : byteCount;

	for (int i = 0; i < count; i++) {
		QString str("%1");
		uint8_t byte = arr[i];
		arrayStr += str.arg(QString::number(byte, 16).toUpper(), 2, '0') + " ";
	}
	return arrayStr;
}

bool makeArrayFromHexString(const QString & in_strData, QByteArray& out_array)
{
	QStringList list(in_strData.split(' ', Qt::SkipEmptyParts, Qt::CaseInsensitive));

	bool ok;
	uint16_t val = 0;
	QByteArray byteArray;
	for(QString str: list)
	{
		val = str.toUShort(&ok, 16);	// Из 16-ого формата
		if (!ok)
			return false;
		if (val > 0xff)	// Должны быть по байтно
			return false;

		uint8_t byte(val);
		byteArray.append(byte);
	}

	out_array = byteArray; // В конце копируем
	return true;
}

QString makeCommandNamesFilePath(DcController* controller)
{
	QString path;

	if (controller) {
		QString controllerPath(controller->path());	// Возвращает путь к БД
		QStringList list = controllerPath.split("uconf.db");	// Разделяем на части, чтобы удалить uconf.db
		//path = list.at(0) + ModbusMaster_ns::SLAVE_CONFIG_DIR + "/" + ModbusMaster_ns::MODBUS_COMMAND_NAMES_FILE_NAME + QString::number(slaveID) + ".json";	// Формируем полный путь
		path = list.at(0) + ModbusMaster_ns::SLAVE_CONFIG_DIR + "/" + ModbusMaster_ns::MODBUS_COMMAND_NAMES_FILE_NAME + ".json";	// Формируем полный путь
	}

	return path;
}


