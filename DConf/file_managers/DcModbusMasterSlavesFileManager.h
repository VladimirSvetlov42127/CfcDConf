#pragma once

#include <file_managers/IRs485SlaveFileManager.h>

class DcModbusMasterSlavesFileManager : public IRs485SlaveFileManager
{
public:
	DcModbusMasterSlavesFileManager(DcController *device = nullptr);
};

FILE_MANAGER_REGISTER(DcModbusMasterSlavesFileManager)