#include "signal.h"

Signal::Signal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype, const QString &name, uint16_t subtypeID)
    : m_id{id}
    , m_internal_id{internalid}
    , m_direction{direction}
    , m_type{type}
    , m_subtype{subtype}
    , m_name{name}
{
    _syb_type_id = subtypeID;
}

uint8_t Signal::type() const
{
    return m_type;
}

uint8_t Signal::subType() const
{
    return m_subtype;
}

int32_t Signal::globalID() const
{
    return m_id;
}

uint16_t Signal::internalID() const
{
    return m_internal_id;
}

QString Signal::name() const
{
    return m_name;
}
QString Signal::text() const
{
    return QString("[%1] %2").arg(m_internal_id).arg(name());
}

QString Signal::fullText() const
{
    return text();
}

