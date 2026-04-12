#include "signal_icustom_name.h"

#include "data_model/dc_controller.h"

ISignalCustomName::ISignalCustomName(uint16_t addr, uint16_t elementIdx)
    : m_addr{addr}
    , m_elementIdx{elementIdx}
{
}

QString ISignalCustomName::customName() const
{
    return nameElement() ? nameElement()->value() : QString();
}

void ISignalCustomName::setCustomName(const QString &name)
{
    if (nameElement() && nameElement()->value() != name)
        nameElement()->updateValue(name);
}

bool ISignalCustomName::init(DcController *config)
{
    m_nameElement = config->paramsRegistry().element(m_addr, m_elementIdx);
    return true;
}

ParameterElement *ISignalCustomName::nameElement() const
{
    return m_nameElement;
}
