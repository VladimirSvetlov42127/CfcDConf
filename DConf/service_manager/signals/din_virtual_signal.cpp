#include "din_virtual_signal.h"

#include "service_manager/services/service_output.h"
#include "service_manager/signals/target_element.h"
#include "db/dc_db_manager.h"

DinVirtualSignal::DinVirtualSignal(const Config &config)
    : DinSignal{config}
    , ISignalCustomName{SP_VIRTDIN_NAME, config.subtypeID}
{
}

QString DinVirtualSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Виртуальный вход");
}

QString DinVirtualSignal::text() const
{
    if (!source())
        return DinSignal::text();

    return QString("%1 (%2)").arg(DinSignal::text(), source()->name());
}

QString DinVirtualSignal::fullText() const
{
    QStringList targetNames;
    for(auto target: this->targets())
        targetNames.append(target->name());

    if (targetNames.empty())
        return text();

    return QString("%1 -> {%2}").arg(text(), targetNames.join(" | "));
}

bool DinVirtualSignal::init(DcController *config)
{
    m_saveBit = config->paramsRegistry().elementBit(SP_DIN_VDINSAVED, subtypeID());
    m_fix1Bit = config->paramsRegistry().elementBit(SP_DIN_VDINFIXED, subtypeID());
    ISignalCustomName::init(config);
    return DinSignal::init(config);
}

QString DinVirtualSignal::baseText() const
{
    return DinSignal::text();
}

void DinVirtualSignal::setSource(ServiceOutput *newSource)
{
    if (source() == newSource)
        return;

    auto prevSource = source();
    m_source = newSource;
    if (prevSource && prevSource->target() == this)
        prevSource->setTarget(nullptr);

    if (newSource)
        newSource->setTarget(this);

    onSourceChanged(newSource, prevSource);
}

ServiceOutput *DinVirtualSignal::source() const
{
    return m_source;
}

void DinVirtualSignal::onSourceChanged(const ServiceOutput *newSource, const ServiceOutput *prevSource) const
{
    uint16_t uid = 0;
    if (newSource && newSource->bindElement())
        uid = newSource->bindElement()->uid();
    else if (prevSource && prevSource->bindElement())
        uid = prevSource->bindElement()->uid();

    if (!uid)
        return;

    QString newName = newSource ? newSource->name() : QString("Вирутальный вход %1").arg(subtypeID());
    QString query = QString("UPDATE signals SET name = '%1' WHERE signal_id = %2;")
            .arg(newName)
            .arg(globalID());
    gDbManager.execute(uid, query);
}
