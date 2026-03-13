#include "ModbusSlaveFileSaver.h"

#include <qfile.h>
#include <qdebug.h>
#include <qdir.h>

#include <dpc/sybus/utils.h> // для функции CRC16
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "file_managers/DcModbusMasterSlavesFileManager.h"
#include "gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusFunctionsHeaders.h"
#include "gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandsHeaders.h"
#include "gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h"
#include "gui/forms/interfaces_protocols/RS485/macrosCheckPointer.h"
#include "gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusParser.h"

using namespace Dpc::Gui;

static const int MAX_SLAVE_CONFIG_TO_PARAM(115);
static const uint16_t FILE_IDENTIFIER(0x2345);


ModbusSlaveFileSaver::ModbusSlaveFileSaver(DcController *controller, uint16_t portNumber, QObject *parent)
    : noUse(false),
    m_controller(controller),
    m_slave(nullptr),
    m_portNumber(portNumber),
	m_maxSlaveByteSize(0),
    m_portInfo(controller)
{
	calculateStartSlaveIndex();
}

ModbusSlaveFileSaver::~ModbusSlaveFileSaver()
{
}

uint8_t ModbusSlaveFileSaver::saveAllSlaveConfig(const QVector<spSlave_t>& slaveVector, QString &errorMessage)
{
	uint8_t result;
	uint8_t count = slaveVector.size();

	//if (!checkSlaveNames(slaveVector)) {	// Проверяем совпадения имен файлов
	//	errorMessage = m_outErrMessage;
	//	return WRONG_FILE_NAME;
	//}

	for (int i = 0; i < count; i++) { // Проверяем размер конфигурации каждого слейва до удаления старых файлов		
		uint32_t configSize = slaveVector[i]->getSlaveFullDefinitionByteArray().size();
		if (configSize >= getMaxByteCountForSlaveConfig()) {
			errorMessage = "Превышен максимальный размер конфигурации %1 слейва (%2 байт). Уменьшите количество команд";
			errorMessage = errorMessage.arg(i + 1, getMaxByteCountForSlaveConfig());
			return WRONG_BUF_SIZE;
		}
	}

	if (!removeOldFiles(slaveVector)) {	// Удаляем все старые файлы, чтобы случайно не удалить новый со старым именем
		errorMessage = m_outErrMessage;
		return CANT_REMOVE_FILE;
	}
	
	for (int i = 0; i < count; i++) {	// Сохраняем каждый слейв	

		if (!slaveVector[i]->getCustomFileName().isEmpty())	// Если пользователь вводил имя, заменяем его
			if (!slaveVector[i]->setFileName(slaveVector[i]->getCustomFileName()))
				return WRONG_FILE_NAME;

		result = saveSlaveConfig(slaveVector[i], i, errorMessage);	// Записываем по одному
		if (result != ALL_IS_OK)
			break;

		if (!m_nameSaver.saveCommandNames(m_portNumber, i, slaveVector[i]->getCommandsHash(), makeCommandNamesFilePath(m_controller))) {
			errorMessage = "Невозможно сохранить имена команд";
			return CANT_WRITE_TO_FILE;
		}	
	}

	return result;
}

uint8_t ModbusSlaveFileSaver::saveSlaveConfig(spSlave_t slave, uint16_t slaveIndex, QString& errorMessage)
{
	if (!slave) {
		errorMessage = "Указатель на слейв равен нулю";
		return NO_SLAVE;
	}

	if (!m_controller) {
		QString errMes = "Невозможно получить контроллер для сохранения %1 слейва!";
		errorMessage = errMes.arg(slaveIndex + 1);
		return NO_CONTROLLER;
	}

	m_slaveInfoByteArray = slave->getSlaveFullDefinitionByteArray();	// Вычитываем буфер для хранения настроек слейва
	if(!slave->checkIfAllParamsCorrect()){	// Проверяем что можем
		QString errMes = "Ошибка в проверке конфигурации %1 слейва, запись не произведена!";
		errorMessage = errMes.arg(slaveIndex + 1);
		return WRONG_CONFIG;
	}

	QDir modbusDir(getFolderForLocalSave(m_controller));	// Проверяем, есть ли такая директория
	if (!modbusDir.exists())
		if (!modbusDir.mkdir(getFolderForLocalSave(m_controller))) {	// Если нету, то создаем
			errorMessage = "Невозможно создать директорию " + getFolderForLocalSave(m_controller);
			return CANT_WRITE_TO_FILE;
		}
	int res = -1;
	slave->setModbusModeActive(true);	// Ставим флаг, что слейв активен

	// Два варианта записи: если больше допустимого числа, то пишем в файл
	if (m_slaveInfoByteArray.size() >= m_maxSlaveByteSize) {

		// Выставляем бит, указыващий на расположение конфигурации в файле
		slave->setModbusModeSaveInFile(true);	// Записываем в слейв
		res = writeToBinaryFile(slave, m_portNumber,  slaveIndex);
	}
	else {	// Если в пределах допустимого значения байт, то пишем в модель
		removeSlaveCfgFile(m_controller, slave->getFileName());	// Если слейв был до этого сохранен в файле, мы должны этот файл удалить

		//if (m_controller->hasFileSystem()) {
		//	removeSlaveCfgFile(m_controller, slave->getFileName());	// Если слейв был до этого сохранен в файле, мы должны этот файл удалить
		//}
		
		slave->setModbusModeSaveInFile(false);	// Записываем в слейв
		m_slaveInfoByteArray = slave->getSlaveFullDefinitionByteArray();	// Сохраняем массив байт описания слейва
		res = writeToModel(slaveIndex);
	}

	if (res != ALL_IS_OK) {	// Если записалось неудачно
		slave->setModbusModeActive(false);
		errorMessage = "Ошибка сохранения конфигурации слейвов";
	}
	return res;
}

uint8_t ModbusSlaveFileSaver::readSlaveConfig(uint8_t slaveCount, QVector<spSlave_t> & out_slaves)
{
	if (!m_controller) {
		MsgBox::error("Невозможно получить контроллер для сохранения");
		return NO_CONTROLLER;
	}

	if (slaveCount == 0) {
		MsgBox::error("Количество слейвов задано как ноль");
		return NO_SLAVES;
	}
	
	ModbusSlaveDefinitionAnnounce_Type slaveHeader;

	uint8_t res = 0;
	bool noAnySlaveConfig = true;
	
	for (int slaveId = 0; slaveId < slaveCount; slaveId++) {	// Проходим по всем слейвам

		uint16_t slaveStartIndex = m_slaveStartIndexOnCurrentPort + slaveId * PROFILE_SIZE;
		QByteArray headerArray;
		bool isModifyModbus = true;	// Здесь ставим тру, но потом оно поменяется! Нужно для создания файла
		spSlave_t slave(new ModbusSlaveDefinition(isModifyModbus));
		out_slaves.append(slave);

		uint16_t size = slave->getSlaveHeaderByteSize();
		for (int paramIndex = 0; paramIndex < size; paramIndex++) {
            auto param(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveStartIndex + paramIndex));
			CHECK_ADDRESS_RETURN_VALUE(param, NO_PARAM);
			headerArray.append(static_cast<uint8_t> (param->value().toUShort()));
		}

		res = parseSlaveHeadline(slave, headerArray);	// Заполняем заголовок слейва из буфера
		if (res != ALL_IS_OK) {	// Если парсинг неудачный, то будет добавен пустой слейв
			QString info = "Внимание! Невозможно прочитать заголовок настроек %1 слейва.";
			info = info.arg(slaveId + 1);
			MsgBox::error(info);
			continue;
		}
			
		// Проверяем режим слейва. В файле или нет
		if (slave->isModbusModeSaveInFile()) {	// Если в файле
			res = readFromBinaryFile(slave, m_portNumber, slaveId);
		}
		else { // Значит все в модели
			res = readCommandsFromModel(slave, slaveStartIndex);
		}

		if (res != ALL_IS_OK) {
			QString info = "Внимание! Невозможно прочитать команды из %1 слейва.\n" + m_outErrMessage;
			info = info.arg(slaveId + 1);
			MsgBox::error(info);
			continue;
		}

		noAnySlaveConfig = false;

		if(!slave->getCommandsHash().isEmpty())	// Провяряем имена, только если есть команды
			if (!m_nameSaver.readCommandNames(m_portNumber, slaveId, slave, makeCommandNamesFilePath(m_controller))) {
				//MsgBox::warning("Невозможно загрузить имена команд слейва " + QString::number(slaveId + 1) + ". Используются имена по умолчанию");
			}
				
	}	

	if (noAnySlaveConfig == false)	// Если хотя бы один один слейв был настроен
		res = ALL_IS_OK;

	return res;
}

spSlave_t ModbusSlaveFileSaver::readSlaveConfigFromFile(const QString & filename, uint16_t slaveIdInPort)
{
	if (filename.isEmpty())
		return nullptr;

	spSlave_t slave (new ModbusSlaveDefinition());
	if (readFromBinaryFile(slave, m_portNumber, slaveIdInPort, filename) != ALL_IS_OK)
		return nullptr;

	return slave;
}

bool ModbusSlaveFileSaver::writeSlaveConfigToFile(spSlave_t slave, const QString & filename)
{
	if(filename.isEmpty())
		return false;

	return writePartToFile(slave, filename);
}

//
//bool ModbusSlaveFileSaver::resetUnusedSlaves(uint16_t portNumber, uint16_t activeSlaveCount)
//{	
//	int32_t slaveForAllPorts = m_portInfo.getSummSlaveCountFromAllPorts();
//	int32_t slaveForPort = m_portInfo.getSlaveCountForEachPort();
//
//	if (slaveForAllPorts < 0 || slaveForPort < 0)
//		return false;
//
//	// Все описания слейвов для всех портов хранятся в одном параметре
//	uint8_t inactiveSlaveIndex = activeSlaveCount; // Нумерация от нуля
//
//	while (inactiveSlaveIndex < slaveForPort) {
//
//		QString filename = readFileNameFromModel(m_controller, portNumber, inactiveSlaveIndex);
//		if (!filename.isEmpty()) {
//			if (!removeSlaveCfgFile(filename))
//				return false;
//		}
//
//		if (!cleanSlaveConfig(portNumber, inactiveSlaveIndex))
//			return false;
//
//		inactiveSlaveIndex++;	// Переходим к следующему индексу слейва
//	}
//
//	return true;
//}

bool ModbusSlaveFileSaver::removeSlaveCfgFile(DcController *controller, const QString & filename)
{
	if (!controller)
		return false;

	QString path = getFilePathForLocalSave(controller, filename);
	if(path.isEmpty())
		return false;

	QFile file(path);
	if (file.exists())
		if (!file.remove())
			return false;

	return true;
}

uint8_t ModbusSlaveFileSaver::writeToBinaryFile(spSlave_t slave, uint16_t portId, uint16_t slaveIndex)
{
	// Запись состоит из двух частей: в первой нужно записать все описание в файл
	// Во второй надо дублировать анонс слейва в модель + добавить имя файла, хранящего остальные настройки
	   
	int32_t slaveForPort = m_portInfo.getSlaveCountForEachPort();
	if (slaveForPort < 0)
		return WRONG_CONFIG;

	int32_t slaveNumberInAllPorts = portId * slaveForPort + slaveIndex;	// Номер слейва в параметре 0х1FE9, где все слейвы подрят лежат

	// Выбираем имя. Либо пользовательское, либо по умолчанию
	QString filePath;	// Сохраняем в модели то имя, которое указано для слейва
	QString fileName = slave->getFileName();
	if (fileName.isEmpty()) {	// Ставим по умолчанию
		filePath = getDefaultFilePathForLocalSave(m_controller, slaveNumberInAllPorts);
		fileName = makeDefaultSlaveConfigFileName(slaveNumberInAllPorts);
	}	
	else {	// Создаем файл с необходимым именем
		filePath = getFolderForLocalSave(m_controller) + fileName;
	}

	if (!writePartToFile(slave, filePath)) {
		MsgBox::error("Ошибка записи информации в файл");
		return CANT_WRITE_TO_FILE;
	}

	if (!writePartToModel(slave, fileName, portId, slaveIndex)) {
		MsgBox::error("Ошибка записи информации в файл");
		return CANT_WRITE_TO_PARAM;
	}

	return ALL_IS_OK;
}

bool ModbusSlaveFileSaver::writePartToFile(spSlave_t slave, const QString &filePath)
{
	QString configDirPath = getFolderForLocalSave(m_controller);	// Получаем путь к папке, хранящей настройки слейвов в модели
	QDir configDir(configDirPath);
	if (!configDir.exists())	// Если такой папки не существует, создаем
		QDir().mkdir(configDirPath);

	QFile slaveFileKeeper(filePath);	// Позволяет в деструкторе закрыть файл

	if (!slaveFileKeeper.open(QIODevice::WriteOnly)) {	// Подразумевается, что открываем файл на перезапись
		MsgBox::error("Ошибка открытия файла для записи");
		return false;
	}

	// Записываем в файл заголовок с CRC16 и описание команд
	QByteArray slaveInfoByteArray (slave->getSlaveFullDefinitionByteArray());
	SybusSlaveDefinitionInFileHdr_Type header;
	header.DefineId = FILE_IDENTIFIER;
	header.Len = slaveInfoByteArray.size();
	header.crc = Dpc::Sybus::crc16(reinterpret_cast<uint8_t *>(slaveInfoByteArray.data()), slaveInfoByteArray.size(), 0);

	const char* pointerToHeader = reinterpret_cast<const char*>(&header);
	QByteArray headerByteArray(pointerToHeader, sizeof(header));	// Буфер для структуры

	slaveFileKeeper.write(headerByteArray);	// Сначала записываем хэдер
	slaveFileKeeper.write(slaveInfoByteArray);	// Файл закрывается в деструкторе
	return true;
}

bool ModbusSlaveFileSaver::writePartToModel(spSlave_t slave, const QString &fileName, uint16_t portId, uint16_t slaveIndex)
{
	// Записать в модель хедер с именем файла
	QByteArray slaveHeader(slave->getSlaveHeader());	// Сохраняем анонс настройки
	if (slaveHeader.isEmpty())
		return false;

	if (fileName.size() > getMaxFileNameByteSize()) {
		MsgBox::error("Превышен размер для имени файла");
		return false;
	}

	// Записываем в модель информацию о файле с настройками
	QByteArray fileNameBytes;
	fileNameBytes.append(getFilePathForSaveInDevice(fileName).toUtf8());	// Пишем абсолютный путь, где хранится файл на устройстве
	fileNameBytes.append(char(0));	// символ конца строки

	QByteArray arrayForWriteToModel;	// Объединение массивов для записи в модель
	arrayForWriteToModel.append(slaveHeader);
	arrayForWriteToModel.append(fileNameBytes);
	arrayForWriteToModel.append(char(0));	// Конец конфигурации

	
	const int portDisplacement = portId * PROFILE_SIZE * m_portInfo.getSlaveCountForPort(portId) + slaveIndex * PROFILE_SIZE;
	for (int i = 0; i < arrayForWriteToModel.size(); i++) {
        auto param(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, portDisplacement + i));
		if (!param) {
			QString str = "Невозможно получить параметр для записи, индекс: " + QString::number(i);
			MsgBox::error(str);
			return false;
		}

        param->updateValue(QString::number(static_cast<uint8_t>(arrayForWriteToModel[i])));
//		if (!param->updateValue(QString::number(static_cast<uint8_t>(arrayForWriteToModel[i])))) {
//			QString str = "Невозможно обновить параметр, индекс: " + QString::number(i);
//			MsgBox::error(str);
//			return false;
//		}
	}
	return true;
}

uint8_t ModbusSlaveFileSaver::readFromBinaryFile(spSlave_t slave, uint16_t portId, uint16_t slaveIndex, const QString &in_filePath)
{	
	QString filePath;	// Назначаем имя файла и для записи в модель
	if (in_filePath.isEmpty()) {
		QString filename = DcModbusMasterSlavesFileManager(m_controller).fileNameFor(portId, slaveIndex);
		slave->setFileName(filename);
		filePath = QString("%1/%2").arg(getFolderForLocalSave(m_controller)).arg(QFileInfo(filename).fileName());
	}		
	else {
		filePath = in_filePath;
		if (!slave->setFileName(filePath.right(filePath.size() - filePath.lastIndexOf('/') - 1))) {
			MsgBox::error("Невозможно использовать данное имя файла.");
			return WRONG_FILE_NAME;
		}
	}		

	QFile slaveFileKeeper(filePath);	// Позволяет в деструкторе закрыть файл
	if (!slaveFileKeeper.exists()) {
		m_outErrMessage = "Выбранный файл не существует! Путь: " + filePath;
		return NO_SUCH_FILE;
	}

	if (!slaveFileKeeper.open(QIODevice::ReadOnly)) {	// Проверяем, возможно ли открыть наш файл для чтения
		m_outErrMessage = "Невозможно открыть файл: " + filePath;
		return CANT_READ_FROM_FILE;
	}

	// Проверяем заголовок в начале файла
	QByteArray headerArray = slaveFileKeeper.read(sizeof(SybusSlaveDefinitionInFileHdr_Type));
	SybusSlaveDefinitionInFileHdr_Type header(*reinterpret_cast<SybusSlaveDefinitionInFileHdr_Type*>(headerArray.data()));

	quint64 posBeforeSlaveInfo = slaveFileKeeper.pos();	// Позиция для начала считывания описания хедера
	QByteArray slaveInfoArray = slaveFileKeeper.readAll();	// Читаем оставшееся 
	uint16_t calcCRC = Dpc::Sybus::crc16(reinterpret_cast<uint8_t*>(slaveInfoArray.data()), slaveInfoArray.size(), 0);

	if (calcCRC != header.crc) {
		m_outErrMessage = "При чтении файла конфигурации слейва не совпала контрольная сумма CRC16";		
		return WRONG_CRC;
	}

	slaveFileKeeper.seek(posBeforeSlaveInfo);	// Восстанавливаем указатель в файле 
	QByteArray buf = slaveFileKeeper.read(sizeof(ModbusSlaveDefinitionAnnounce_Type));
	uint8_t res = parseSlaveHeadline(slave, buf);	// Читаем и заполняем слейва
	if (res != ALL_IS_OK)
		return res;

	// Здесь может быть несколько команд
	QByteArray len = slaveFileKeeper.read(1);	// Читаем байт длины
	uint16_t commandSize = len[0];	// Сохраняем длину команды

	while (commandSize != 0) {

		QByteArray CommandInfo = slaveFileKeeper.read(commandSize - 1);	// -1 потому что длину уже вычитали
		if (CommandInfo.isEmpty())
			return NO_COMMAND_INFO;

		QByteArray commandHeader;	// Формируем заголовок команды
		commandHeader.append(commandSize);
		commandHeader.append(CommandInfo);

		spCommandDescr_t command(createCommandFromBufData(commandHeader));
		if (!command) {
			m_outErrMessage = "Ошибка парсингла файла";
			return ERROR_PARSE_FILE;
		}

		// Добавляем команду в слейв	
		ModbusCommand_ns::keyPairTypeAndIndex_t key;
		res = makeKeyForCommand(slave, command, key);	// Высчитываем подходящий ключ
		if (res != ALL_IS_OK) {
			m_outErrMessage = "Ошибка определения ключа для добавления в список команд";
			break;
		}

		if (!slave->addCommand(key, command)) {
			m_outErrMessage = "Ошибка добавления команды в слейв";
			res = CANT_ADD_COMMAND_TO_SLAVE;
			break;
		}

		if(slaveFileKeeper.atEnd()){	// Если вдруг это конец
			break;
		}

		len = slaveFileKeeper.read(1);	// Читаем следующий байт длины
		if (len.isEmpty()) {
			m_outErrMessage = "При чтении файла байт длины не был прочитан";
			return ERROR_PARSE_FILE;	// Ошибка чтения. 
		}
		
		commandSize = len[0];

		if (commandSize == 0) {	
			len = slaveFileKeeper.read(1);	// Читаем следующий байт длины команды инициализации
			if ((int)len[0] != 0) { // Если следующий байт не нулевой, то это команда инициализации
				uint pos = slaveFileKeeper.pos() - 1;		// Указывает на длину команды инициализации
				slaveFileKeeper.seek(pos);
				if (parseInitCommands(slaveFileKeeper.readAll()) != ALL_IS_OK) {
					m_outErrMessage = "Ошибка парсинга файла";
					return ERROR_PARSE_FILE;
				}
					
			}
		}
	}

	return res;
}

uint8_t ModbusSlaveFileSaver::writeToModel(uint16_t slaveIndex)
{
	if (m_slaveInfoByteArray.isEmpty())
		return CANT_GET_SLAVE_ANNOUNCE_BUF;

	uint16_t size = m_slaveInfoByteArray.size();

	if (size > m_maxSlaveByteSize) {
		MsgBox::error("Попытка записать в модель конфигурацию, превыщающую максимальный размер");
		return CANT_WRITE_TO_PARAM;
	}

	uint16_t slaveStartIndex = m_slaveStartIndexOnCurrentPort + PROFILE_SIZE * slaveIndex;

	for (int i = 0; i < size; i++) {
        auto param(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveStartIndex + i));
		if (!param) {
			QString str = "Невозможно получить параметр для записи, индекс: " + QString::number(i);
			MsgBox::error(str);
			return NO_PARAM;
		}

		uint8_t oneByte = m_slaveInfoByteArray[i];
        param->updateValue(QString::number(oneByte));
//		if (!param->updateValue(QString::number(oneByte))) {
//			QString str = "Невозможно обновить параметр, индекс: " + QString::number(i);
//			MsgBox::error(str);
//			return CANT_WRITE_TO_PARAM;
//		}
	}

	// В конце ставим ДВА нуля для ограничения считывания 
    //auto param1(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveStartIndex + size));
	//if (param1) {
	//	param1->updateValue(QString::number(0));	// Ставим ноль для завершения
	//}
    //auto param2(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, slaveStartIndex + size + 1));
	//if (param2) {
	//	param2->updateValue(QString::number(0));	// Ставим ноль для завершения
	//}

	return ALL_IS_OK;

}

uint8_t ModbusSlaveFileSaver::readCommandsFromModel(spSlave_t slave, uint16_t slaveStartIndex)
{
	int headerSize = sizeof(ModbusSlaveDefinitionAnnounce_Type);
	uint16_t posFromBufStart = slaveStartIndex + headerSize;	// Позиция с начала описания команд

    auto paramLen(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart));
	CHECK_ADDRESS_RETURN_VALUE(paramLen, NO_PARAM);

    // Пробуем узнать размерность параметра
	uint16_t maxParamIndex = slaveStartIndex + 117;	// Используется для старых устройств до cfg_v2
    maxParamIndex = slaveStartIndex + paramLen->parameter()->elementsCount() + m_slaveStartIndexOnCurrentPort;

	uint16_t commandLen = paramLen->value().toUInt();	// Узнаем длину команды
	uint8_t res;

	// В одном слейве могут быть несколько команд, поэтому ищем их все

	while (commandLen != 0) {	// ПРОДУМАТЬ ВЫХОД ИЗ ЦИКЛА!

		if ((posFromBufStart + commandLen) >= maxParamIndex) {
			MsgBox::error("Ошибка распознавания команды. Выход за пределы индекса.");
			break;
		}

		QByteArray commandArray;	// Массив байт для хранения команды

		// Длина не нулевая, вычитываем все в массив
		for (int i = 0; i < commandLen; i++) {
            auto param(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart + i));
			CHECK_ADDRESS_RETURN_VALUE(param, NO_PARAM);
			commandArray.append(static_cast<uint8_t> (param->value().toUShort()));	// Сохраняем в массив для байты команды
		}

		spCommandDescr_t command(createCommandFromBufData(commandArray)) ;
		if (!command) {
			MsgBox::error("Невозможно создать команду из прочитанного массива байт");
			return ERROR_PARSE_FILE;
		}

		// Добавляем команду в слейв	
		ModbusCommand_ns::keyPairTypeAndIndex_t key;
		res = makeKeyForCommand(slave, command, key);
		if (res != ALL_IS_OK) {	// Высчитываем подходящий ключ
			MsgBox::error("Ошибка определения ключа для добавления в список команд");
			return res;
		}
		
		if (!slave->addCommand(key, command)) {
			MsgBox::error("Ошибка добавления команды в слейв");
			return CANT_ADD_COMMAND_TO_SLAVE;
		}

		// Читаем следующую длину 
		posFromBufStart += commandLen;	// Прибавляем к указателю от начало кол-во байт текущей команды
        auto paramLen(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart));
		CHECK_ADDRESS_RETURN_VALUE(paramLen, NO_PARAM);

		commandLen = paramLen->value().toInt();	// Узнаем длину следующей команды		
	}
	
	// Дошли до нуля. Проверяем, есть после него команды инициализации
	posFromBufStart++;
    paramLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart);
	CHECK_ADDRESS_RETURN_VALUE(paramLen, NO_PARAM);
	if (paramLen->value().toInt() != 0) {	// Значит есть команды инициализации

		commandLen = paramLen->value().toInt();	// Длина команды инициализации

		while (commandLen != 0) {

			if ((posFromBufStart + commandLen) >= maxParamIndex) {
				MsgBox::error("Ошибка распознавания команды импорта. Выход за пределы индекса.");
				break;
			}

			QByteArray initCommandArray;
			
			for (int i = 0; i < commandLen; i++) {	// Длина не нулевая, вычитываем все в массив
                auto param(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart + i));
				CHECK_ADDRESS_RETURN_VALUE(param, NO_PARAM);
				initCommandArray.append(static_cast<uint8_t> (param->value().toUShort()));	// Сохраняем в массив для байты команды
			}

			spInitCommand command(new ModbusCommandInitialization);
			if (command->parseFromBuf(initCommandArray)) {
				slave->getInitCommandsVector()->append(command);
			}
			else {
				MsgBox::error("Ошибка парсинга команды инициализации");
				return ERROR_PARSING;
			}
				

			posFromBufStart += commandLen;	// Прибавляем к указателю от начало кол-во байт текущей команды
            auto paramLen(m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, posFromBufStart));
			CHECK_ADDRESS_RETURN_VALUE(paramLen, NO_PARAM);

			commandLen = paramLen->value().toInt();	// Узнаем длину следующей команды				
		}		
	}

	return ALL_IS_OK;
}

bool ModbusSlaveFileSaver::calculateStartSlaveIndex()
{
	// Выполняем расчеты по определению начального индекса слейва
    auto param_v1 = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, 0);
	CHECK_ADDRESS_RETURN_BOOL(param_v1);
    auto paramSlaveInfo = param_v1->parameter();
	if (!paramSlaveInfo) {
		noUse = true;
		MsgBox::error("Вы используете ПО со старой версией cfg. Использование данного модуля невозможно. Вам нужно обновить ПО и использовать более новую версию модели контроллера либо обратиться к разработчикам.");
		return false;
	}
    auto paramPortInfo = m_controller->paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);
	CHECK_ADDRESS_RETURN_BOOL(paramPortInfo);
    uint16_t commomSlaveCountForAllPorts = paramSlaveInfo->profilesCount();
    uint16_t portCount = paramPortInfo->profilesCount();
	if (commomSlaveCountForAllPorts % portCount != 0) {
		MsgBox::error("Невозможно определить количество слейвов на порту!");
		return false;
	}

	const uint16_t slaveForPort = commomSlaveCountForAllPorts / portCount;	// Количество слейвов на порту

	m_slaveStartIndexOnCurrentPort = m_portNumber * PROFILE_SIZE * slaveForPort;
	m_maxSlaveByteSize = m_portInfo.getSlaveConfigSize();	// Размер подпрофиля определяет максимальное кол-во байт, которое может быть записано в модель контроллера

	return true;
}

//bool ModbusSlaveFileSaver::isFileNameBusyInPort(QStringList &existingNamesInPort, spSlave_t slave)
//{
//	//// Проверяем имена слейвов. Если пользователь изменил, удаляем старый
//	//QString customName = slave->getCustomFileName();
//	//QString nameInSlave = slave->getFileName();
//	//nameInSlave = nameInSlave.left(nameInSlave.size() - 4);	// Убираем формат
//	//QString resultSlaveName = nameInSlave;	// Имя, которое будет у файла в итоге
//	//QString templ = "%1_p%2s%3";
//
//	//QString newName = templ.arg(resultSlaveName, QString::number(portId), QString::number(slaveInd));
//	//if (newName.size() >= getMaxFileNameByteSize()) {
//	//	MsgBox::error("Данное имя уже занято другим слейвом, но преобразовать к шаблонному типу нельзя, т.к. превышается максимальный размер. Измените имя файла для сохранения");
//	//	return false;
//	//}
//	//MsgBox::warning("Внимание, файл с именем <" + resultSlaveName + "> уже существует, для сохранения оно преобразовано в <" + newName + ">");
//	//resultSlaveName = newName;
//	//slave->setFileName(newName + ".msl");
//
//	QString fileName = slave->getFileName();
//	QString fileNameWithoutDim = fileName.left(fileName.size() - 4);	// Убираем формат
//	if (existingNamesInPort.contains(fileNameWithoutDim)) {
//		return true;
//	}	
//	existingNamesInPort.append(fileNameWithoutDim);
//
//	return false;
//}

bool ModbusSlaveFileSaver::isFileNameBusyInFolder(const QString &slaveName) {
	QDir saveDir(getFolderForLocalSave(m_controller));	// Директория для сохранения конфигураций слейвов
	QStringList currentNames = saveDir.entryList();

	if (currentNames.contains(slaveName)) {	// Если в директории остался такой файл	
		return true;
	}
	return false;
}

bool ModbusSlaveFileSaver::checkSlaveNames(const QVector<spSlave_t>& slaveVector)
{
	
	QStringList namesInCurrentPort;	// Храним имена слейвов для сравнения на совпадение 
	QDir saveDir(getFolderForLocalSave(m_controller));	// Директория для сохранения конфигураций слейвов
	QStringList namesInFolder = saveDir.entryList();
	QStringList removeNames;

	for (int i = 0; i < slaveVector.size(); i++) {
		QString cusmonName = slaveVector[i]->getCustomFileName();	// Берется из формы, вводится пользователем
		QString checkingName;

		if (!cusmonName.isEmpty()) {
			checkingName = cusmonName;	// Имя, которое будет проверяться на совпадение. Если пользователь ввел свое, то оно будет заменено
		}
		else {
			checkingName = slaveVector[i]->getFileName();
		}
		removeNames.append(slaveVector[i]->getFileName());	// Добавляем имя в список для удаления.
				
		if (namesInCurrentPort.contains(checkingName)) {	// Есть ли такое имя уже или нет
			QString shortName(checkingName); // Отрезаем формат файла
			shortName.chop(4);
			m_outErrMessage = "На текущем порту существуют слейвы с одинаковыми именами. \nЗамените конфиликтное имя: " + shortName;
			return false;
		}  		
			   		 
		namesInCurrentPort.append(checkingName);
	}

	// После проверяем файлы в папке
	for (int i = 0; i < slaveVector.size(); i++) {
		QString cusmonName = slaveVector[i]->getCustomFileName();	// Берется из формы, вводится пользователем
		QString checkingName = cusmonName.isEmpty() ? slaveVector[i]->getFileName() : cusmonName;	// Имя, которое будет проверяться на совпадение. Если пользователь ввел свое, то оно будет заменено
		
		if (namesInFolder.contains(checkingName)) {	// Если в директории есть такой файл
			if (removeNames.contains(checkingName))	// Если имя будет удалено, не проверяем его
				continue;

			QString shortName(checkingName); // Отрезаем формат файла
			shortName.chop(4);
			m_outErrMessage = "Совпадение имен!\nВ папке проекта обнаружен файл с таким же именем, возможно, в нем сохранены настройки слейва из другого порта. \nЗамените конфиликтное имя: " + shortName;
			return false;
		}
	}
		
	return true;
}

bool ModbusSlaveFileSaver::removeOldFiles(const QVector<spSlave_t>& slaveVector)
{
	for (int i = 0; i < slaveVector.size(); i++) {
		removeSlaveCfgFile(m_controller, slaveVector[i]->getFileName());
	}
	return true;
}


uint8_t ModbusSlaveFileSaver::parseSlaveHeadline(spSlave_t slave, const QByteArray & readedBuffer)
{
	// При хранении данных в массиве байт, младший байт всегда идет первым
	
	if (readedBuffer.isEmpty())
		return EMPTY_BUFFER;
	if (slave == nullptr)
		return EMPTY_POINTER_TO_SLAVE;

	if (readedBuffer.size() != sizeof(ModbusSlaveDefinitionAnnounce_Type))
		return WRONG_BUF_SIZE;

	ModbusSlaveDefinitionAnnounce_Type newAnnounse (*(ModbusSlaveDefinitionAnnounce_Type*)readedBuffer.data());
		
	slave->setMode(newAnnounse.Mode);
	slave->setTimeout(newAnnounse.TimeoutMs);
	slave->setAddress(newAnnounse.Address);
	slave->setAinsCnt(newAnnounse.AinsCnt);
	slave->setDinsCnt(newAnnounse.DinsCnt);
	slave->setCinsCnt(newAnnounse.CinsCnt);
	slave->setRAoutCnt(newAnnounse.rAoutsCnt);
	slave->setRDoutCnt(newAnnounse.rDoutsCnt);
	slave->setWAoutCnt(newAnnounse.wAoutsCnt);
	slave->setWDoutCnt(newAnnounse.wDoutsCnt);

	m_slave = slave;

	return ALL_IS_OK;
}

uint8_t ModbusSlaveFileSaver::parseInitCommands(const QByteArray & readedBuffer)
{
	if (readedBuffer.isEmpty())
		return EMPTY_BUFFER;

	uint8_t commandLen = readedBuffer[0];	// Первый байт- длина
	uint32_t pos = 0;
	bool ok = false;

	while (commandLen != 0) {
		QByteArray comBuf(readedBuffer.mid(pos, commandLen));	// Отсекаем буфер по размеру команды

		spInitCommand command (new ModbusCommandInitialization);
		ok = command->setExpectedBcnt(comBuf[ModbusCommandInitialization::EXPECTED_BCNT_POS]);
		if (!ok)
			break;

		ok = command->setExpectedFirstRspByte(comBuf.mid(ModbusCommandInitialization::EXPECTED_FIRST_RSP_BYTE_POS, ModbusCommand_ns::ExpectedFirstRspBcnt_Count));
		if (!ok)
			break;

		ok = command->setAnalizedRspBcnt(comBuf[ModbusCommandInitialization::ANALIZED_RSP_BCNT]);
		if (!ok)
			break;
		
		ok = command->setInitCmdBuf(comBuf.mid(ModbusCommandInitialization::INIT_COMMAND_POS, comBuf[ModbusCommandInitialization::REG_DATA_LEN_POS]));
		if (!ok)
			break;

		m_slave->getInitCommandsVector()->append(command);	// Добавляем команду в слейв

		pos += commandLen;
		if (pos >= readedBuffer.size())	// Если дошли до конца и там нет нулей
			break;

		commandLen = readedBuffer[pos];	// Считываем следующую длину
	}

	if (!ok)
		return ERROR_PARSING;
	return ALL_IS_OK;
}

spCommandDescr_t ModbusSlaveFileSaver::createCommandFromBufData(const QByteArray & readedBuffer)
{
	if (readedBuffer.isEmpty())
		return nullptr;
	if (readedBuffer.size() < sizeof(ModbusCommand_ns::ModbusVarUpdate_t))	// В буфере минимум должен быть заголовок
		return nullptr;

	// Мы не знаем, какая команда пока не рассмотрим заголовок команды
	QByteArray header(readedBuffer.left(sizeof(ModbusCommand_ns::ModbusVarUpdate_t)));
	ModbusCommand_ns::ModbusVarUpdate_t commandHeadline;
	commandHeadline = *(ModbusCommand_ns::ModbusVarUpdate_t*)header.data();	// Копируем в структуру

	// Оставляем данные для Модбас функции, размер которой указан
	QByteArray modbusFunctionBuf(readedBuffer.right(commandHeadline.ReqLen));	// Оставляем выбранное кол-во байт справа

	// И в середине МОГУТ остаться байты, используемый в качестве дополнительных параметров после заголовка
	QByteArray extraInfo(readedBuffer.left(readedBuffer.size() - commandHeadline.ReqLen));	// Отсекаем модбас функцию
	extraInfo = extraInfo.right(extraInfo.size() - sizeof(ModbusCommand_ns::ModbusVarUpdate_t));	// Оставляем байты справа

	spCommandDescr_t command = createCommandByType(commandHeadline, extraInfo);	//  Выделяем память под команду зная определенный ее тип
	ModbusParser parser;
	spModbusFunc_t function = parser.createFunctionFromBufAndCommand(modbusFunctionBuf, command);	// Создаем и заполняем фунцию значениями из буфера

	if (!command || !function)
		return nullptr;

	if (!command->setCommandStruct(commandHeadline))	// Записываем структуру в команду
		return nullptr;
	if (!command->setModbusFunction(function))	// Записываем выбранную функцию с ее параметрами
		return nullptr;

	return command;
}


spCommandDescr_t ModbusSlaveFileSaver::createCommandByType(const ModbusCommand_ns::ModbusVarUpdate_t &info, const QByteArray &extraInfo) {
	spCommandDescr_t fullCommand(nullptr);
	if (!m_slave)
		return fullCommand;

	bool isModbusModify = m_slave->isModbusModeModify();

	switch (info.VarType)
	{
	case ModbusMaster_ns::AinsInd: {
		fullCommand = spCommandDescr_t(new ModbusCommandAins(isModbusModify));
		break;
	}
	case ModbusMaster_ns::DinsInd: {	// Дополнительных полей нет, только Модбас команда. Неизвестного пока типа (может выбираться пользователем)
		fullCommand = spCommandDescr_t(new ModbusCommandDins(isModbusModify));
		break;
	}
	case ModbusMaster_ns::CinsInd: {
		fullCommand = spCommandDescr_t(new ModbusCommandCins(isModbusModify));
		break;
	}
	case ModbusMaster_ns::RAoutsInd: {

		break;
	}
	case ModbusMaster_ns::RDoutsInd: {

		break;
	}
	case ModbusMaster_ns::WAoutsInd: {

		break;
	}
	case ModbusMaster_ns::WDoutsInd: {

		break;
	}
	}

	if (!fullCommand)	// Найдена ли соответствующая функция
		return nullptr;
	
	if (!fullCommand->setCommandStruct(info))	// Верно ли заполнился заголовок команды
		return nullptr;

	if (!fullCommand->parseExtraInfoFromBuf(extraInfo))	// Правильно ли распозналась дополнительная информация
		return nullptr;	

	return fullCommand;
}

 uint8_t ModbusSlaveFileSaver::makeKeyForCommand(spSlave_t slave, spCommandDescr_t command, ModbusCommand_ns::keyPairTypeAndIndex_t &key)
{	
	 if (!command)
		 return EMPTY_POINTER_TO_COMMAND;
	 if (!slave)
		 return EMPTY_POINTER_TO_SLAVE;

	 // Ищем все данные данного типа и находим максимальный свободный индекс для нового ключа
	 const QHash<ModbusCommand_ns::keyPairTypeAndIndex_t, spCommandDescr_t> hash(slave->getCommandsHash());

	 uint16_t index = 0;
	 ModbusCommand_ns::keyPairTypeAndIndex_t keyIndex(command->getCommandType(), index++);
	 QHash<ModbusCommand_ns::keyPairTypeAndIndex_t, spCommandDescr_t>::const_iterator it = hash.find(keyIndex);
	 
	 while (it != hash.end()) {	// Проверяем, есть ли запись с таким индексом
		 keyIndex.second = index++;
		 it = hash.find(keyIndex);
	 }

	 key = keyIndex;

	return ALL_IS_OK;
}




