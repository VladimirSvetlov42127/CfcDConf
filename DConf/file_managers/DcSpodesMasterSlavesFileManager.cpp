#include "DcSpodesMasterSlavesFileManager.h"

#include <gui/forms/interfaces_protocols/RS485/Spodes/master/SpodesMasterCommon.h>

DcSpodesMasterSlavesFileManager::DcSpodesMasterSlavesFileManager(DcController * device) :
	IRs485SlaveFileManager(device, "0", "Spodes", DcController::SpodesMasterProtocol, sizeof(SpodesMaster::SlaveDefinition))
{
}