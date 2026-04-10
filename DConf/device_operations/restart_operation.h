#pragma once

#include <optional>

#include <device_operations/abstract_operation.h>

class RestartOperation : public AbstractOperation
{
	Q_OBJECT

public:
    using RestartMode = Dpc::Sybus::Channel::ResetMode;
    using WorkMode = std::optional<Dpc::Sybus::Channel::WorkingMode>;
    using DecontMode = std::optional<Dpc::Sybus::Channel::DecontMode>;

    RestartOperation(RestartMode mode, WorkMode workMode, DecontMode decontMode, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write; }

protected:
	virtual bool exec() override;

private:
    RestartMode m_mode;
    WorkMode m_workMode;
    DecontMode m_decontMode;
};
