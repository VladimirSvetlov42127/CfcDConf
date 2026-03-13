#pragma once

#include <file_managers/IRs485SlaveFileManager.h>

class DcSpodesMasterSlavesFileManager : public IRs485SlaveFileManager
{
public:
	DcSpodesMasterSlavesFileManager(DcController *device = nullptr);
};

FILE_MANAGER_REGISTER(DcSpodesMasterSlavesFileManager)