#pragma once

#include <qfiledevice.h>

#include <device_operations/abstract_config_operation.h>
#include <dpc/sybus/ParamPack.h>

class QTemporaryDir;

class ConfigReadOperation : public AbstractConfigOperation
{
	Q_OBJECT

public:
    ConfigReadOperation(DcController *config, QObject *parent = nullptr);
    ~ConfigReadOperation();

    QList<QPair<ParameterElement*, DcBoard*>> failedParams() const;
	QStringList failedFiles() const;
	QHash<int, Dpc::Sybus::ParamPackPtr> specialParams() const;

    Flags flags() const override { return Flag::Read; }

protected slots:
    virtual void onChannelProgress(int state) override;

protected:
	virtual bool before() override;
	virtual bool exec() override;
	virtual bool after() override;

private:
    DcController* tempConfig();
	bool clearDeviceFolder();
    bool copyFilesToConfig();
	bool readParams();
	bool readSpecialParams();
	bool readFiles();
	bool readConfigArchive();
	bool unpackConfigArchive();

    bool copyParams();
    bool clearBindings();
    bool configureBindings();

    bool readParam(Parameter* param, DcBoard* board = nullptr);
	bool createFile(const QString &fileName, const QByteArray &data, QFlags<QFileDevice::Permission> permissions);

private:
	QTemporaryDir *m_tempDir;
    DcController::UPtr m_tempConfig;
    QList<QPair<ParameterElement*, DcBoard*>> m_failedParams;
    QStringList m_failedFiles;
    QString m_configArchiveFile;
	int m_filesCount;
	int m_currentFile;
	
	QHash<int, Dpc::Sybus::ParamPackPtr> m_specialParams;
};
