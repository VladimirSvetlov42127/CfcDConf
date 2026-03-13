#pragma once

#include <device_operations/abstract_config_operation.h>

class QTemporaryDir;

class ConfigWriteOperation : public AbstractConfigOperation
{
	Q_OBJECT

public:
    ConfigWriteOperation(DcController *config, QObject *parent = nullptr);
    ~ConfigWriteOperation();

	QString ipAddress() const;

    Flags flags() const override { return Flag::Write; }

protected slots:
    virtual void onChannelProgress(int state) override;

protected:
	virtual bool before() override;
	virtual bool exec() override;

private:
	bool packConfigArchive();
	bool setPrepareWorkMode();
	bool prepareRestartFlag();
	bool checkRestartFlag();
	bool uploadFiles();
	bool writeParams();
	bool readIpAddress();
	bool saveAndRestart();
	
    bool writeParam(Parameter* param, DcBoard* board = nullptr);

private:
	QTemporaryDir *m_tempDir;
	QString m_configArchiveFile;
	QList<DcBoard*> m_writtenBoards;
	int m_filesCount;
	int m_currentFile;
	QString m_ip;
};
