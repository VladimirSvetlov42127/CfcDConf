#ifndef SERVICEIO_H
#define SERVICEIO_H

#include <QString>

class ServiceIO
{
public:
    ServiceIO(uint16_t id, uint8_t pin);
    virtual ~ServiceIO() = default;

    // id Входа. Нужен для сохранения обратной совместимости.
    uint16_t id() const;

    // Ножка сервиса на котором расположен вход/выход.
    uint8_t pin() const;

    virtual QString name() const = 0;

private:
    uint16_t m_id;
    uint8_t m_pin;
};

#endif // SERVICEIO_H
