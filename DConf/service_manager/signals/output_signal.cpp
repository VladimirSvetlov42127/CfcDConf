#include "output_signal.h"

#include <QDebug>

#include "service_manager/signals/signal.h"
#include "service_manager/signals/input_signal.h"
#include "db/dc_db_manager.h"

OutputSignal::OutputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype,const QString &name, ParameterElement* bindElement, uint16_t subtypeID)
    : Signal{id, internalid, direction, type, subtype, name, subtypeID}
    , TargetElement{bindElement}
{
}

QString OutputSignal::name() const
{
    return Signal::name();
}

void OutputSignal::onSourceChanged(InputSignal *newSource, InputSignal *prevSource)
{
    auto uid = bindElement()->uid();
    if (!uid)
        return;

    if (prevSource) {
        auto query = QString("DELETE FROM matrix_signals WHERE signal_id_source = %1 and signal_id_destination=%2;")
                .arg(prevSource->globalID())
                .arg(globalID());
        gDbManager.execute(uid, query);
    }

    if (newSource) {
        auto query = QString("INSERT INTO matrix_signals(signal_id_source, signal_id_destination) VALUES(%1, %2);")
                .arg(newSource->globalID())
                .arg(globalID());
        gDbManager.execute(uid, query);
    }
}
