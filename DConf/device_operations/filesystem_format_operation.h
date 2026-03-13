#pragma once

#include <device_operations/abstract_operation.h>

class FilesystemFormatOperation : public AbstractOperation
{
	Q_OBJECT

public:
    FilesystemFormatOperation(int drive, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write | Flag::Filesystem; }

protected slots:
    void onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg) override;

protected:
    virtual bool exec() override;

private:
	int m_drive;
};
