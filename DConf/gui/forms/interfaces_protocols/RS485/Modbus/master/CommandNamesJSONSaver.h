#pragma once
#include <qhash.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveDefinition.h>

class CommandNamesJSONSaver
{
	typedef ModbusCommand_ns::keyPairTypeAndIndex_t key_t;

public:
	CommandNamesJSONSaver();
	~CommandNamesJSONSaver();
	
	bool saveCommandNames(uint8_t portNumber, uint8_t slaveInd, const QHash<key_t, spCommandDescr_t> &commands, const QString &filepath);
	bool readCommandNames(uint8_t portNumber, uint8_t slaveInd, spSlave_t slave, const QString &filepath);


};
