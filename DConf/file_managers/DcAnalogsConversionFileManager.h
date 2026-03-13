#pragma once

#include <file_managers/IFileManager.h>

class DcAnalogsConversionFileManager : public IFileManager
{
public:
	DcAnalogsConversionFileManager(DcController *device = nullptr);

	virtual QStringList configFiles() const override;
};

FILE_MANAGER_REGISTER(DcAnalogsConversionFileManager)