#pragma once

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusCommandDefines.h>
#include <data_model/dc_controller.h>

namespace ModbusMaster_ns {

	const QString SLAVE_CONFIG_NAME("Slave_");
	const QString SLAVE_CONFIG_DIR("Modbus");	
	const QString DEVICE_ROOT_FOLDER_NAME("0:/");
	const QString DEVICE_DIR_FOR_SLAVE(DEVICE_ROOT_FOLDER_NAME + SLAVE_CONFIG_DIR + "/");
	const QString MODBUS_COMMAND_NAMES_FILE_NAME("Command_names_of_slaves");

}

// Операции с именами файлов по умолчанию
QString makeDefaultCommandName(uint16_t number, ModbusMaster_ns::channelType_t type, ModbusMaster_ns::count_t count, ModbusMaster_ns::startAddress_t address, bool isModify = false);
bool checkFileNameSize(const QString & name);
QString makeDefaultSlaveConfigFileName(uint8_t slaveID);	// Формирует имя файла слейва по умолчанию


// Получение директорий для сохранения конфигурационных файлов
QString getModbusFolderName();	// Имя папки, где хранятся настройки слейва

QString getDefaultFilePathForLocalSave(DcController* controller, uint8_t slaveID);

QString getFilePathForSaveInDevice(const QString &filename);
QString getFilePathForLocalSave(DcController* controller, const QString &filename);

QString getFolderForLocalSave(DcController * controller);

QString getFolderPathForSlaveInDevice();

uint32_t getMaxByteCountForSlaveConfig();	// Максимальное кол-во байт на каждый файл конфигурации
uint8_t getMaxFileNameByteSize();	// Возвращает кол-во байт, отведенное под имя файла для записи в модель

QString makeHexByteStringFromArray(const QByteArray& arr, uint16_t byteCount = 0);
bool makeArrayFromHexString(const QString & in_strData, QByteArray& out_array);

// Формирует пути для работы с файлами, хранящими имена команд в модбас мастере
QString makeCommandNamesFilePath(DcController* controller);
