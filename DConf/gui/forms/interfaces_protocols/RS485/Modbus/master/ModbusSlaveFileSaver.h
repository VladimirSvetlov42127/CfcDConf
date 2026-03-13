#pragma once

#include <QObject>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveDefinition.h>
#include "Common/common_CommandDefines.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/CommandNamesJSONSaver.h>
#include <gui/forms/interfaces_protocols/RS485/PortInfo.h>


class ModbusSlaveFileSaver
{
	typedef ModbusSlaveDefinition::ModbusSlaveDefinitionAnnounce_Type ModbusSlaveDefinitionAnnounce_Type;
	typedef QSharedPointer<ModbusCommandInitialization> spInitCommand;
	typedef ModbusCommand_ns::keyPairTypeAndIndex_t key_t;

#pragma pack(push, 1)	// Описание, которое должно быть помещено в начало файла перед заголовком описания слейва
	typedef struct {
		uint16_t	DefineId;	/*идентификатор файла 0x2345 */
		uint16_t	Len;		/*длина описания*/
		uint16_t	crc;		/*crc описания*/
	} SybusSlaveDefinitionInFileHdr_Type;
#pragma pack(pop)


public:

	enum {
		ALL_IS_OK = 0,
		UNKNOWN = 1,

		FILENAME_EMPTY,
		NO_SLAVES,
		CANT_GET_SLAVE_ANNOUNCE_BUF,
		NO_COMMAND_FOR_SLAVE,

		NO_SUCH_FILE,
		CANT_OPEN_FILE,
		CANT_READ_FROM_FILE,
		CANT_WRITE_TO_FILE,
		CANT_WRITE_TO_PARAM,
		CANT_ADD_COMMAND_TO_SLAVE,
		CANT_REMOVE_FILE,

		EMPTY_BUFFER,
		EMPTY_POINTER_TO_SLAVE,
		EMPTY_POINTER_TO_COMMAND,

		WRONG_BUF_SIZE,
		WRONG_FILE_NAME,
		WRONG_CONFIG,
		WRONG_CRC,
		WRONG_CFG_PARAM_VERSION,
		ERROR_PARSE_FILE,
		ERROR_PARSE_MODEL,
		ERROR_PARSING,
		ERROR_COMMAND_KEY,

		NO_COMMAND_INFO,
		NO_CONTROLLER,
		NO_PARAM,
		NO_SLAVE,


	};

    ModbusSlaveFileSaver(DcController* controller, uint16_t portNumber, QObject *parent = Q_NULLPTR);
	~ModbusSlaveFileSaver();

	uint8_t saveAllSlaveConfig(const QVector<spSlave_t> &slaveVector, QString& errorMessage);
	uint8_t saveSlaveConfig(spSlave_t slave, uint16_t slaveIndex, QString& errorMessage);
	uint8_t readSlaveConfig(uint8_t slaveCount, QVector<spSlave_t> &out_slaves);

	spSlave_t readSlaveConfigFromFile(const QString& filename, uint16_t slaveIdInPort);
	bool writeSlaveConfigToFile(spSlave_t slave, const QString& filename);

	//bool resetUnusedSlaves(uint16_t portNumber, uint16_t activeSlaveCount);
	static bool removeSlaveCfgFile(DcController *controller, const QString& filename);

private:
    bool noUse;
	DcController *m_controller;
	QByteArray m_slaveInfoByteArray;
	spSlave_t m_slave;
	uint16_t m_slaveStartIndexOnCurrentPort;
	uint16_t m_portNumber;
	uint16_t m_maxSlaveByteSize;

	CommandNamesJSONSaver m_nameSaver;
	PortInfo m_portInfo;
	QString m_outErrMessage;	// Для сообщений об ошибках

private:

	//bool cleanSlaveConfig(uint16_t portId, uint16_t slaveId);

	uint8_t parseSlaveHeadline(spSlave_t slave, const QByteArray &readedBuffer);
	uint8_t parseInitCommands(const QByteArray &readedBuffer);
	spCommandDescr_t createCommandFromBufData(const QByteArray & readedBuffer);
	spCommandDescr_t createCommandByType(const ModbusCommand_ns::ModbusVarUpdate_t & info, const QByteArray &extraInfo);


	uint8_t makeKeyForCommand(spSlave_t slave, spCommandDescr_t command, ModbusCommand_ns::keyPairTypeAndIndex_t &key);

	// Функции для записи конфигурации в файл
	uint8_t writeToBinaryFile(spSlave_t slave, uint16_t portId, uint16_t slaveIndex);
	bool writePartToFile(spSlave_t slave, const QString &filePath);	// Записываем конфигурацию в файл с требуемым заголовком
	bool writePartToModel(spSlave_t slave, const QString &fileName, uint16_t portId, uint16_t slaveIndex);	// Записываем заголовок слейва и имя файла, где хранятся команды

	uint8_t readFromBinaryFile(spSlave_t slave, uint16_t portId, uint16_t slaveIndex, const QString &in_filename = QString());

	uint8_t writeToModel(uint16_t slaveStartIndex);
	uint8_t readCommandsFromModel(spSlave_t slave, uint16_t slaveStartIndex);

	bool calculateStartSlaveIndex();

	//bool isFileNameBusyInPort(QStringList &existingNamesInPort, spSlave_t slave);	// Проверяет совпадения в именах файлов
	bool isFileNameBusyInFolder(const QString &slaveName);	// Есть ли такое имя в папке проекта Модбас
	bool checkSlaveNames(const QVector<spSlave_t> &slaveVector);	

	bool removeOldFiles(const QVector<spSlave_t> &slaveVector);	// Удаляет все старые файлы
};
