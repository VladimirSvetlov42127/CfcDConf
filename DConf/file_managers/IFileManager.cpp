#include "IFileManager.h"

QList<IFileManagerPtr>& fileManagersList() { static QList<IFileManagerPtr> list; return list; }

IFileManager::IFileManager(DcController* device, const QString& deviceDriveName, const QString& deviceDir, const QString& localDir) :
	m_device(device),
	m_deviceDriveName(deviceDriveName),
	m_deviceDir(deviceDir),
	m_localDir(localDir)
{
}

QString IFileManager::devicePath(const QString& fileName) const
{
	QString path = QString("%1:/%2").arg(m_deviceDriveName).arg(m_deviceDir);
	if (fileName.isEmpty())
		return path;

	return QString("%1/%2").arg(path).arg(fileName);
}

QString IFileManager::localPath(const QString& fileName) const
{
	auto dir = !m_localDir.isEmpty() ? m_localDir : m_deviceDir;
	QString path = QString("%1/%2").arg(QFileInfo(device()->path()).absolutePath()).arg(dir);
	if (fileName.isEmpty())
		return path;

	return QString("%1/%2").arg(path).arg(fileName);
}

void IFileManager::setDevice(DcController* device)
{
	m_device = device;
	onNewDeviceSet();
}

IFileManager::ConfigFileInfoList IFileManager::fileInfoList() const
{
	ConfigFileInfoList result;
	for (auto& file : configFiles())
		result.append({ file, localPath(QFileInfo(file).fileName()) });

	return result;
}

ParameterElement *IFileManager::getParam(int32_t addr, uint16_t index) const
{
    return device() ? device()->paramsRegistry().element(addr, index) : nullptr;
}
