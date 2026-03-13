#ifndef ALGMANAGER_H
#define ALGMANAGER_H

#include <unordered_set>

#include "service_manager/services/alg/alg_service.h"

class DcAlgInternal;
class DcController;

class AlgManager
{
public:
    AlgManager(DcController* config);

    // Инициализация списка встроенных алгоритмов/сервисов. Настройка привязок
    bool init();

    // Сброс всех привязок встроенных алгоритмов/сервисов
    void clearBindings();

    // Настройка привязок всех встроенных алгоритмов/сервисов
    void rebind();

    // Количество встроенных алгоритмов/сервисов
    int count() const;

    // Список встроенных алгоритмов/сервисов
    QList<AlgService*> algList() const;

    // Возвращает встроенный алгоритм/сервис с адресом addr
    AlgService* alg(uint16_t addr) const;    

private:
    DcController* config() const;

    // Загрузка встроенного сервиса internalAlg из таблиц.
    // Индексы сервиса и индексы входов/выходов сервиса берутся из таблиц.
    void load(DcAlgInternal* internalAlg);

    // Обновление встроенного сервиса с адресом inAddr. Добавляются все отсутсвующие входы/выходы с записью в таблицу.
    // Если не сервиса с адресом inAddr. Сервис создается во всеми входами/выходами с записью в соответсвующие таблицы.
    void update(uint16_t inAddr, const QString& name);

    void setUsedAlgID(uint16_t id);
    uint16_t takeFreeAlgID();

    void setUsedIOID(uint16_t ioID);
    uint16_t takeFreeAlgIOID();

private:
    DcController* m_config;
    std::unordered_map<uint16_t, AlgService::UPtr> m_algs;
    std::unordered_set<uint16_t> m_usedAlgIds;
    std::unordered_set<uint16_t> m_usedIOIds;
};

#endif // ALGMANAGER_H
