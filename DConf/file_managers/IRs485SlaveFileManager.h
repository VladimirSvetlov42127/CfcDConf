#pragma once

#include <file_managers/IFileManager.h>

class IRs485SlaveFileManager : public IFileManager
{
public:
	IRs485SlaveFileManager(DcController* device, const QString& deviceDriveName, const QString& deviceDir, int protocolCode, int fileNameOffset);

	virtual QStringList configFiles() const override;

	QString fileNameFor(int slaveIdx) const;
	QString fileNameFor(int port, int slaveInPort);

protected:
	virtual void onNewDeviceSet() override;

private:
	uint16_t portsCount() const { return m_portsCount; }
	uint16_t slavesPerPort() const { return m_slavesPerPort; }

private:
	int m_protocolCode;
	int m_fileNameOffset;

	uint16_t m_portsCount;
	uint16_t m_slavesPerPort;
};