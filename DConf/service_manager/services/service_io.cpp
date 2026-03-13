#include "service_io.h"

ServiceIO::ServiceIO(uint16_t id, uint8_t pin)
    : m_id{id}
    , m_pin{pin}
{
}

uint16_t ServiceIO::id() const
{
    return m_id;
}

uint8_t ServiceIO::pin() const
{
    return m_pin;
}
