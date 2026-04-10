#include "dout_signal.h"

#include <QDebug>

#include "service_manager/signals/signal.h"
#include "service_manager/signals/din_signal.h"
#include "db/dc_db_manager.h"

DoutSignal::DoutSignal(const Config &config)
    : Signal{config}
    , TargetElement{}
{
}

QString DoutSignal::name() const
{
    return Signal::name();
}

bool DoutSignal::init(DcController *config)
{
    auto bindElement = config->paramsRegistry().element(SP_CROSSTABLEDOUT, internalID());
    setBindElement(bindElement);

    m_impulse = config->paramsRegistry().element(SP_DOUT_SAMPLTIM, internalID());
    return bindElement;
}

std::optional<uint32_t> DoutSignal::impulse() const
{
    if (m_impulse)
        return m_impulse->value().toUInt();

    return {};
}

void DoutSignal::setImpulse(uint32_t value)
{
    if (m_impulse)
        m_impulse->updateValue(QString::number(value));
}

void DoutSignal::onSourceChanged(DinSignal *newSource, DinSignal *prevSource)
{
    if (!bindElement() || !bindElement()->uid())
        return;

    auto uid = bindElement()->uid();
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
