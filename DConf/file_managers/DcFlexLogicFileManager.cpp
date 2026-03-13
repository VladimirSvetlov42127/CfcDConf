#include "DcFlexLogicFileManager.h"


namespace {
	const QString FILE_NAME_TEMPLATE = "%1.%2";

	const QString EXTENSION_GRAPH = "graph";
	const QString EXTENSION_BCA = "bca";
	const QString EXTENSION_IMAGE = "png";
}

DcFlexLogicFileManager::DcFlexLogicFileManager(DcController * device) :
	IFileManager(device, "/0", "FlexLgc", "cfc")
{
}

QString DcFlexLogicFileManager::localGraphFileName(uint8_t algID) const
{
    return localPath(FILE_NAME_TEMPLATE.arg(algID).arg(EXTENSION_GRAPH));
}

QString DcFlexLogicFileManager::localImageFileName(uint8_t algID) const
{
    return localPath(FILE_NAME_TEMPLATE.arg(algID).arg(EXTENSION_IMAGE));
}

QString DcFlexLogicFileManager::localBcaFileName(uint8_t algID) const
{
    return localPath(FILE_NAME_TEMPLATE.arg(algID).arg(EXTENSION_BCA));
}

QString DcFlexLogicFileManager::deviceBcaFileName(uint8_t algID) const
{
    return devicePath(FILE_NAME_TEMPLATE.arg(algID).arg(EXTENSION_BCA));
}

QStringList DcFlexLogicFileManager::configFiles() const
{
    auto param = getParam(SP_FILE_FLEXLGFILES, 0);
	if (!param)
		return QStringList();

	QStringList result;
    for (size_t i = 0; i < param->parameter()->elementsCount(); i++) {
        auto fileName = getParam(SP_FILE_FLEXLGFILES, i)->value();
		if (fileName.size() < 4)
			continue;

		auto first = fileName[0];
		if (first != '/' && !first.isDigit())
			continue;

		auto parts = fileName.split('/');
		if (!parts.last().contains("."))
			continue;

		result << fileName;
	}

	return result;
}
