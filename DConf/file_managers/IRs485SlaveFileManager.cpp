#include "IRs485SlaveFileManager.h"

#include <qdebug.h>

namespace {
	const uint16_t INDEX_PROTOCOL = 0;
	const uint16_t INDEX_QUANTITY = 4;

	const uint8_t BIT_ACTIVE = 0;
	const uint8_t BIT_INFILE = 7;
}

IRs485SlaveFileManager::IRs485SlaveFileManager(DcController* device, const QString& deviceDriveName, const QString& deviceDir, int protocolCode, int fileNameOffset) :
	IFileManager(device, deviceDriveName, deviceDir),
	m_protocolCode(protocolCode),
	m_fileNameOffset(fileNameOffset),
	m_portsCount(0), 
	m_slavesPerPort(0)
{
	onNewDeviceSet();
}

QStringList IRs485SlaveFileManager::configFiles() const
{
	// Не все устройства подерживают данные параметры, поэтому конфигурационных файлов не может быть.
	if (!portsCount() || !slavesPerPort())
		return QStringList();

	QStringList result;
	for (int portIdx = 0; portIdx < portsCount(); portIdx++) {
        auto protocolParam = getParam(SP_USARTPRTPAR_BYTE, portIdx * PROFILE_SIZE + INDEX_PROTOCOL);
		if (!protocolParam)
			return QStringList();

		if (protocolParam->value().toUInt() != m_protocolCode)
			continue;

        auto slaveCountParam = getParam(SP_USARTPRTPAR_BYTE, portIdx * PROFILE_SIZE + INDEX_QUANTITY);
		if (!slaveCountParam)
			return QStringList();

		int slaveCount = slaveCountParam->value().toInt();
		for (int i = 0; i < slaveCount; i++) {
			int slaveIdx = portIdx * slavesPerPort() + i;
            auto value = getParam(SP_USARTEXTNET_DEFINITION, slaveIdx * PROFILE_SIZE)->value().toUInt();
			bool activeFlag = value & (1 << BIT_ACTIVE);
			bool writeInFileFlag = value & (1 << BIT_INFILE);			
			if (!activeFlag || !writeInFileFlag) 
				continue;

			auto fileName = fileNameFor(slaveIdx);
			if (fileName.isEmpty())
				continue;

			result << fileName;
		}
	}

	return result;
}

QString IRs485SlaveFileManager::fileNameFor(int slaveIdx) const
{
	if (!portsCount() || !slavesPerPort())
		return QString();

	QString fileName;
	int firstParamIdx = slaveIdx * PROFILE_SIZE;
    auto slaveParam = getParam(SP_USARTEXTNET_DEFINITION, firstParamIdx);
    for (size_t j = m_fileNameOffset; j < slaveParam->parameter()->elementsCount(); j++) {
        auto p = getParam(SP_USARTEXTNET_DEFINITION, firstParamIdx + j);
		uint8_t byte = static_cast<uint8_t>(p->value().toUShort());
		if (!byte)
			break;

		fileName.append(QChar(byte));
	}

	return fileName;
}

QString IRs485SlaveFileManager::fileNameFor(int port, int slaveInPort)
{
	return fileNameFor(port * slavesPerPort() + slaveInPort);
}

void IRs485SlaveFileManager::onNewDeviceSet()
{
	m_portsCount = 0;
	m_slavesPerPort = 0;

    if (auto param = getParam(SP_USARTPRTPAR_BYTE, 0); param)
        m_portsCount = param->parameter()->profilesCount();

    if (auto param = getParam(SP_USARTEXTNET_DEFINITION, 0); param && portsCount())
        m_slavesPerPort = param->parameter()->profilesCount() / portsCount();
}
