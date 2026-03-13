#include "alg_service_input.h"

#include <QDebug>

#include "db/dc_db_manager.h"
#include "service_manager/signals/input_signal.h"

AlgServiceInput::AlgServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, Service *service)
    : ServiceInput{id, pin, bindElement, service}
{
}

void AlgServiceInput::onSourceChanged(InputSignal *newSource, InputSignal *prevSource)
{
    auto uid = bindElement()->uid();
    if (!uid)
        return;

    if (prevSource) {
        auto query = QString("DELETE FROM matrix_alg WHERE alg_io_id = %1 and signal_id=%2;")
                .arg(id())
                .arg(prevSource->globalID());
        gDbManager.execute(uid, query);
    }

    if (newSource) {
        auto query = QString("INSERT INTO matrix_alg(alg_io_id, signal_id) VALUES(%1, %2);")
                .arg(id())
                .arg(newSource->globalID());
        gDbManager.execute(uid, query);
    }
}
