#pragma once

#include <device_operations/abstract_operation.h>

class SoftUpdateOperation : public AbstractOperation
{
	Q_OBJECT

public:
    SoftUpdateOperation(const QString& fileName, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write; }

protected slots:
    void onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg) override;
    void onChannelProgress(int state) override;

protected:
    virtual bool exec() override;

private:
    const QString m_fileName;
};
