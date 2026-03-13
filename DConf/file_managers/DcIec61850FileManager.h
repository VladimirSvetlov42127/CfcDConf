#pragma once

#include <file_managers/IFileManager.h>

class DcIec61850FileManager : public IFileManager
{
public:
	DcIec61850FileManager(DcController *device = nullptr);

	virtual QStringList configFiles() const override;
};

FILE_MANAGER_REGISTER(DcIec61850FileManager)