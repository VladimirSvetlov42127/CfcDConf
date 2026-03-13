#include "DcIec61850FileManager.h"

DcIec61850FileManager::DcIec61850FileManager(DcController * device) :
	IFileManager(device, "0", "61850")
{
}

QStringList DcIec61850FileManager::configFiles() const
{
    auto paramCfgFile = getParam(SP_MMSCFGFILE, 0);
	if (!paramCfgFile)
		return QStringList();

	auto fileName = paramCfgFile->value();
	auto parts = fileName.split('/');
	if (parts.size() < 2)
		return QStringList();

	if (!parts.first().endsWith(':'))
		return QStringList();

    if (!parts.last().contains("."))
		return QStringList();

	return QStringList{ fileName };
}
