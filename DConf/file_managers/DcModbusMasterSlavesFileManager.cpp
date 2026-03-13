#include "DcModbusMasterSlavesFileManager.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveDefinition.h>

DcModbusMasterSlavesFileManager::DcModbusMasterSlavesFileManager(DcController * device) :
	IRs485SlaveFileManager(device, "0", "Modbus", DcController::ModbusMasterPrototcol, ModbusSlaveDefinition::getSlaveHeaderByteSize())
{
}