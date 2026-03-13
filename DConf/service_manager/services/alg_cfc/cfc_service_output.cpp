#include "cfc_service_output.h"

#include <QDebug>

#include "service_manager/signals/virtual_input_signal.h"
#include "service_manager/services/alg_cfc/cfc_alg_service.h"
#include "db/dc_db_manager.h"

CfcServiceOutput::CfcServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, CfcAlgService *service)
    : ServiceOutput { id, pin, bindElement, service }
    , m_parent{service}
{
}

QString CfcServiceOutput::name() const
{
    return QString("Выход: алг %1, пин %2").arg(m_parent->id()).arg(pin() + 1);
}

QString CfcServiceOutput::text() const
{
    return target() ? target()->text() : name();
}

void CfcServiceOutput::onTargetChanged(VirtualInputSignal *newTarget, VirtualInputSignal *prevTarget)
{
    auto uid = bindElement()->uid();
    if (!uid)
        return;

    if (prevTarget) {
        auto query = QString("DELETE FROM matrix_alg_cfc WHERE alg_cfc_io_id = %1 and signal_id=%2;")
                .arg(id())
                .arg(prevTarget->globalID());
        gDbManager.execute(uid, query);
    }

    if (newTarget) {
        auto query = QString("INSERT INTO matrix_alg_cfc(alg_cfc_io_id, signal_id) VALUES(%1, %2);")
                .arg(id())
                .arg(newTarget->globalID());
        gDbManager.execute(uid, query);
    }
}
