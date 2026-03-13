#pragma once

#include <file_managers/IFileManager.h>

class DcFlexLogicFileManager :public IFileManager
{
public:
	DcFlexLogicFileManager(DcController *device = nullptr);

    QString localGraphFileName(uint8_t algID) const;
    QString localImageFileName(uint8_t algID) const;
    QString localBcaFileName(uint8_t algID) const;
    QString deviceBcaFileName(uint8_t algID) const;

	virtual QStringList configFiles() const override;
};

FILE_MANAGER_REGISTER(DcFlexLogicFileManager)
