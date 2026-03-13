#pragma once

#include <device_operations/abstract_operation.h>

class ResetOperation : public AbstractOperation
{
	Q_OBJECT

public:
    ResetOperation(QObject *parent = nullptr);

	QString ipAddress() const;
    Flags flags() const override { return Flag::Write; }

protected:
	virtual bool exec() override;

private:
	bool clearFiles();

private:
	QString m_ip;
};
