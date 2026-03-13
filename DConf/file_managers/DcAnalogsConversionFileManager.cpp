#include "DcAnalogsConversionFileManager.h"

namespace {
	const int CUSTOM_CONVERSION_TYPE_VALUE = 0x3;
}

DcAnalogsConversionFileManager::DcAnalogsConversionFileManager(DcController * device) :
	IFileManager(device, "0", "AinsConv")
{
}

QStringList DcAnalogsConversionFileManager::configFiles() const
{
    auto param = getParam(SP_AIN_CONVERS_BYTE_PARAM, 0);
	if (!param)
		return QStringList();

	QStringList result;
    for (size_t i = 0; i < param->parameter()->elementsCount(); i++) {
        if (getParam(SP_AIN_CONVERS_BYTE_PARAM, i)->value().toInt() != CUSTOM_CONVERSION_TYPE_VALUE)
			continue;

        auto fileName = getParam(SP_AIN_CONVERS_RULES_FILE, i)->value();
		if (fileName.isEmpty())
			continue;

		result << fileName;
	}

	return result;
}
