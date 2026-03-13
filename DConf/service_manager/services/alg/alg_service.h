#ifndef ALGSERVICE_H
#define ALGSERVICE_H

#include "service_manager/services/service.h"
#include "service_manager/services/alg/alg_service_input.h"
#include "service_manager/services/alg/alg_service_output.h"

class AlgService : public Service
{
public:
    using UPtr = std::unique_ptr<AlgService>;

    AlgService(uint16_t id, const QString &name, uint16_t addr);
    virtual ~AlgService() = default;

    Service::Type type() const override;

    // Адрес встроенного алгоритма. Для идентификации сервиса.
    uint16_t addr() const;

    // ID алгоритма для табилц привязки. Для обратной совместимости
    uint16_t id() const;

    // Создает и добавляет вход/выход алгоритма
    // Запись в таблицу привязки для совместимости
    AlgServiceInput* makeInput(uint16_t id, ParameterElement *bindElement);
    AlgServiceOutput* makeOutput(uint16_t id, ParameterElement *bindElement);

private:
    uint16_t m_addr;
    uint16_t m_id;
};

#endif // ALGSERVICE_H
