#pragma once

#include <QObject>
#include <qvector.h>
#include <qsharedpointer.h>
#include <qhash.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>
#include "Common/common_CommandDefines.h"
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandInitialization.h>

class ModbusSlaveDefinition;
typedef QSharedPointer<ModbusSlaveDefinition> spSlave_t;



namespace ModbusMaster_ns {
	enum slaveModeBits_t { /*pre configured slave options  - Mode field*/
		ModbusSlaveMode_Active = 0, /*slave is active*/
		ModbusSlaveNotModbusPDU,	/* 0- modbus command format  ; 1- not modbus command format*/
		ModbusSlaveCrcHiEnd,         /* 1-CRC allocation high endian;  0- low endian (standard)*/
		ModbusSlaveDef_File = 7,	  /*(0)-slave definition allocated in config, (1)slave definition allocated in file */
	};
}



class ModbusSlaveDefinition
{
	typedef ModbusCommand_ns::keyPairTypeAndIndex_t key_t;
	typedef ModbusMaster_ns::channelType_t channelType_t;
	typedef QSharedPointer<ModbusCommandInitialization> spInitCommand;

public:
#pragma pack(push, 1)
	typedef struct { /*Slave definition Announce*/
		uint8_t   Mode;								/*0- passive*/
		uint8_t   TimeoutMs;
		uint8_t   Address;
		uint16_t  AinsCnt;
		uint16_t  DinsCnt;
		uint16_t  CinsCnt;
		uint16_t  rAoutsCnt;
		uint16_t  rDoutsCnt;
		uint16_t  wAoutsCnt;
		uint16_t  wDoutsCnt;
	} ModbusSlaveDefinitionAnnounce_Type;
#pragma pack(pop)

public:
	ModbusSlaveDefinition(bool isModifyModbus = false);
	~ModbusSlaveDefinition();

	bool checkIfAllParamsCorrect();
	spCommandDescr_t createCommandByType(ModbusMaster_ns::channelType_t type);

	
	// Работа с хранилищем команд
	inline const QHash<key_t, spCommandDescr_t>& getCommandsHash() const { return mh_commands; };
	spCommandDescr_t getCommand(key_t key) const;
	bool addCommand(spCommandDescr_t command);
	bool addCommand(key_t key, spCommandDescr_t command);
	bool removeCommand(key_t key);
	void removeAllTypeCommands(ModbusMaster_ns::channelType_t type);

	// Работа с командами инициализации
	bool addInitCommand(spInitCommand initCommand);
	bool removeInitCommand(uint8_t index);
	spInitCommand getInitCommand(uint8_t index) const;
	inline QVector<spInitCommand> *getInitCommandsVector() { return &mv_initCommands; };
	
	// getters
	const QByteArray & getSlaveFullDefinitionByteArray();	// Полностью описание слейва в байтах
	QByteArray getSlaveHeader() const;
	static inline uint16_t getSlaveHeaderByteSize() { return sizeof(ModbusSlaveDefinitionAnnounce_Type); };
	inline QString getFileName() const { return m_fileName; };
	inline QString getCustomFileName() const { return m_customFileName; };

	uint16_t getTypeCount(ModbusMaster_ns::channelType_t type) const;

	inline uint8_t getMode() const		{ return m_announce.Mode; };
	bool isModbusModeActive() const;
	bool isModbusModeModify() const;
	bool isModbusModeSaveInFile() const;
	bool isCRCBitSet() const;

	inline uint8_t getTimeout() const	{ return m_announce.TimeoutMs; };
	inline uint8_t getAddress() const	{ return m_announce.Address; };
	inline uint16_t getAinsCnt() const { return m_announce.AinsCnt; };
	inline uint16_t getDinsCnt() const { return m_announce.DinsCnt; };
	inline uint16_t getCinsCnt() const { return m_announce.CinsCnt; };
	inline uint16_t getRAoutCnt() const { return m_announce.rAoutsCnt; };
	inline uint16_t getRDoutCnt() const { return m_announce.rDoutsCnt; };
	inline uint16_t getWAoutCnt() const { return m_announce.wAoutsCnt; };
	inline uint16_t getWDoutCnt() const { return m_announce.wDoutsCnt; };

	// setters
	bool setFileName(const QString &name);
	bool setCustomFileName(const QString &name);
	bool setTypeCount(ModbusMaster_ns::channelType_t type, uint16_t count);

	bool setMode(uint8_t val);
	void setModbusModeActive(bool isActive);
	void setModbusModeModify(bool isModify);
	void setModbusModeSaveInFile(bool saveInFile);
	void setModbusModeCRCBit(bool crc);

	bool setTimeout(uint8_t val);
	bool setAddress(uint8_t val);
	bool setAinsCnt(uint16_t val);
	bool setDinsCnt(uint16_t val);
	bool setCinsCnt(uint16_t val);
	bool setRAoutCnt(uint16_t val);
	bool setRDoutCnt(uint16_t val);
	bool setWAoutCnt(uint16_t val);
	bool setWDoutCnt(uint16_t val);

private:
	QByteArray m_slaveByteArray;	// Массив байт для записи заголовка слейва
	ModbusSlaveDefinitionAnnounce_Type m_announce;	// Сама структура
	
	QHash<key_t, spCommandDescr_t> mh_commands;
	QVector<spInitCommand> mv_initCommands;
	QString m_fileName;	// Имя файла, из которого загружены настройки
	QString m_customFileName;	// Новое имя файла, считываемое с виджета. Если не равно m_fileName, то m_fileName удаляется
};
