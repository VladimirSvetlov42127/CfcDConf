#include "cfc_service_input.h"

#include <QDebug>

#include "service_manager/signals/input_signal.h"
#include "service_manager/services/alg_cfc/cfc_alg_service.h"
#include "db/dc_db_manager.h"

CfcServiceInput::CfcServiceInput(uint16_t id, uint8_t pin, ParameterElement *bindElement, CfcAlgService *service)
    : ServiceInput{id, pin, bindElement, service},
    m_parent { service }
{
}

QString CfcServiceInput::name() const
{
    return QString("Вход: алг %1, пин %2").arg(m_parent->id()).arg(pin() + 1);
}

QString CfcServiceInput::text() const
{
    return source() ? source()->text() : name();
}

void CfcServiceInput::onSourceChanged(InputSignal *newSource, InputSignal *prevSource)
{
    auto uid = bindElement()->uid();
    if (!uid)
        return;

    if (prevSource) {
        auto query = QString("DELETE FROM matrix_alg_cfc WHERE alg_cfc_io_id = %1 and signal_id=%2;")
                .arg(id())
                .arg(prevSource->globalID());
        gDbManager.execute(uid, query);
    }

    if (newSource) {
        auto query = QString("INSERT INTO matrix_alg_cfc(alg_cfc_io_id, signal_id) VALUES(%1, %2);")
                .arg(id())
                .arg(newSource->globalID());
        gDbManager.execute(uid, query);
    }
}
