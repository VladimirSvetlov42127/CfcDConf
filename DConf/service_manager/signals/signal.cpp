#include "signal.h"

#include <QDebug>

Signal::Signal(uint32_t globalID, uint16_t internalID, uint16_t subtypeID, const QString &name, const QString &properties)
    : m_globalID{globalID}
    , m_internalID{internalID}
    , m_subtypeID{subtypeID}
    , m_name{name}
    , m_properties{properties}
{
}

Signal::Signal(const Config &config)
    : Signal{config.globalID, config.internalID, config.subtypeID, config.name, config.properties }
{
}

uint32_t Signal::globalID() const
{
    return m_globalID;
}

uint16_t Signal::internalID() const
{
    return m_internalID;
}

uint16_t Signal::subtypeID() const
{
    return m_subtypeID;
}

QString Signal::name() const
{
    return m_name;
}
QString Signal::text() const
{
    return QString("[%1] %2").arg(m_internalID).arg(name());
}

QString Signal::fullText() const
{
    return text();
}

bool Signal::init(DcController *)
{
    return true;
}

