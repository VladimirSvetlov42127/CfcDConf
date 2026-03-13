#include "alg_service.h"

AlgService::AlgService(uint16_t id, const QString &name, uint16_t addr)
    : Service{ name }
    , m_addr{addr}
    , m_id{id}
{
}

Service::Type AlgService::type() const
{
    return Service::AlgType;
}

uint16_t AlgService::addr() const
{
    return m_addr;
}

uint16_t AlgService::id() const
{
    return m_id;
}

AlgServiceInput *AlgService::makeInput(uint16_t id, ParameterElement *bindElement)
{
    auto sio = std::make_unique<AlgServiceInput>(id, inputs().size(), bindElement, this);
    auto raw = sio.get();
    Service::addInput(std::move(sio));

    // Запись в таблицу привязки для совместимости
    return raw;
}

AlgServiceOutput *AlgService::makeOutput(uint16_t id, ParameterElement *bindElement)
{
    auto sio = std::make_unique<AlgServiceOutput>(id, outputs().size(), bindElement, this);
    auto raw = sio.get();
    Service::addOutput(std::move(sio));

    // Запись в таблицу привязки для совместимости
    return raw;
}
