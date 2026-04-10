#include "restart_operation.h"

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

RestartOperation::RestartOperation(RestartMode mode, WorkMode workMode, DecontMode decontMode, QObject * parent)
    : AbstractOperation("Перезагрузка устройства", 0, parent)
    , m_mode(mode)
    , m_workMode{workMode}
    , m_decontMode{decontMode}
{
}

bool RestartOperation::exec()
{
    if (m_workMode) {
        auto p = ParamPack::create(T_BYTE, SP_NEWPROFILE);
        p->appendValue(m_workMode.value());
        if (!channel()->setParam(p)) {
            addError(QString("Не удалось сменить режим работы устройства: %1")
                     .arg(channel()->errorMsg()));
            return false;
        }
    }

    // Пока переводим только в режим совместимости DecontT2(depRTU)
    if (m_decontMode && m_decontMode.value() == Channel::DecontT2) {
        auto errorMsg = QString("Не удалось сменить режим совместимости устройства: %1");
        auto pItem = ParamPack::create(T_BYTE, SP_SETALTCFGTEMPLATE, 0);
        pItem->appendValue(0);
        if (!channel()->setParam(pItem)) {
            addError(errorMsg.arg(channel()->errorMsg()));
            return false;
        }

        pItem = ParamPack::create(T_BYTE, SP_ALTERNATE_MODE, 0);
        pItem->appendValue(m_decontMode.value());
        if (!channel()->setParam(pItem)) {
            addError(errorMsg.arg(channel()->errorMsg()));
            return false;
        }

        if (!channel()->saveConfig()) {
            addError(errorMsg.arg(channel()->errorMsg()));
            return false;
        }
    }

    if (!channel()->restartDevice(m_mode)) {
		QString msg("Не удалось перезагрузить устройство");
		if (m_mode == Channel::MinimalModeReset)
			msg.append(" в минимальный режим");
        addError(QString("%1: %2").arg(msg, channel()->errorMsg()));
		return false;
	}

	return true;
}
