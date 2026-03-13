#include "virtual_input_signal.h"

#include "service_manager/services/service_output.h"
#include "service_manager/signals/target_element.h"

VirtualInputSignal::VirtualInputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID)
    : InputSignal{id, internalid, direction, type, subtype, name, subtypeID}
{
}

QString VirtualInputSignal::name() const
{
    return QString("Виртуальный вход");
}

QString VirtualInputSignal::text() const
{
    if (!source())
        return InputSignal::text();

    return QString("%1 (%2)").arg(InputSignal::text(), source()->name());
}

QString VirtualInputSignal::fullText() const
{
    QStringList targetNames;
    for(auto target: this->targets())
        targetNames.append(target->name());

    if (targetNames.empty())
        return text();

    return QString("%1 -> {%2}").arg(text(), targetNames.join(" | "));
}

QString VirtualInputSignal::baseText() const
{
    return InputSignal::text();
}

void VirtualInputSignal::setSource(ServiceOutput *newSource)
{
    if (source() == newSource)
        return;

    auto prevSource = source();
    m_source = newSource;
    if (prevSource && prevSource->target() == this)
        prevSource->setTarget(nullptr);

    if (newSource)
        newSource->setTarget(this);
}

ServiceOutput *VirtualInputSignal::source() const
{
    return m_source;
}
