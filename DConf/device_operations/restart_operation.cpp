#include "restart_operation.h"

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

RestartOperation::RestartOperation(int mode, QObject * parent) :
    AbstractOperation("Перезагрузка устройства", 0, parent),
	m_mode(mode)
{
}

bool RestartOperation::exec()
{
	if (!channel()->restartDevice((Channel::ResetMode) m_mode)) {
		QString msg("Не удалось перезагрузить устройство");
		if (m_mode == Channel::MinimalModeReset)
			msg.append(" в минимальный режим");
        addError(QString("%1: %2").arg(msg, channel()->errorMsg()));
		return false;
	}

	return true;
}
