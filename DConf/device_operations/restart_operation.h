#pragma once

#include <device_operations/abstract_operation.h>

class RestartOperation : public AbstractOperation
{
	Q_OBJECT

public:
    RestartOperation(int mode, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write; }

protected:
	virtual bool exec() override;

private:
	int m_mode;
};
