#pragma once

#include <qdatetime.h>

#include <device_operations/abstract_operation.h>

class DatetimeOperation : public AbstractOperation
{
	Q_OBJECT

public:
    DatetimeOperation(const QDateTime &dateTime, const QVariant &offset, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write; }

protected slots:
    void onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg) override;

protected:
    virtual bool exec() override;

private:
	QDateTime m_dateTime;
	QVariant m_offset;
};
