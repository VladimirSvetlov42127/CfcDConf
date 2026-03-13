#include "alg_service_output.h"

#include <QDebug>

#include "db/dc_db_manager.h"
#include "service_manager/signals/virtual_input_signal.h"

AlgServiceOutput::AlgServiceOutput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service)
    : ServiceOutput{id, pin, bindElement, service}
{
}

void AlgServiceOutput::onTargetChanged(VirtualInputSignal *newTarget, VirtualInputSignal *prevTarget)
{
    auto uid = bindElement()->uid();
    if (!uid)
        return;

    if (prevTarget) {
        auto query = QString("DELETE FROM matrix_alg WHERE alg_io_id = %1 and signal_id=%2;")
                .arg(id())
                .arg(prevTarget->globalID());
        gDbManager.execute(uid, query);
    }

    if (newTarget) {
        auto query = QString("INSERT INTO matrix_alg(alg_io_id, signal_id) VALUES(%1, %2);")
                .arg(id())
                .arg(newTarget->globalID());
        gDbManager.execute(uid, query);
    }
}
