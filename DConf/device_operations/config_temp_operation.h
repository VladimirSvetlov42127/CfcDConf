#pragma once

#include <device_operations/abstract_config_operation.h>

class QTemporaryDir;

class ConfigTempOperation : public AbstractConfigOperation
{
	Q_OBJECT

public:
    ConfigTempOperation(const QString &name, DcController *config, int stepsCount = 1, QObject *parent = nullptr);
    ~ConfigTempOperation();

    Flags flags() const override { return Flag::Read; }

protected:
	virtual bool before() override;
	virtual bool exec() override;

    DcController* tempConfig() const { return m_tempConfig; }

private:
	bool readConfig();

private:
	QTemporaryDir *m_tempDir;
    DcController *m_tempConfig;
};
