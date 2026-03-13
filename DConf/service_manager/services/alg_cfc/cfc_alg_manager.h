#ifndef CFCALGMANAGER_H
#define CFCALGMANAGER_H

#include "service_manager/services/alg_cfc/cfc_alg_service.h"

class DcController;

class CfcAlgManager
{
    friend class CfcAlgService;
public:
    CfcAlgManager(DcController* config);

    // Перезагрузить алгоритмы гибкой логики
    bool reload();

    // Удаление всех алгоритмов гибкой логики
    void clear();

    // Список алгоритмов гибкой логики
    QList<CfcAlgService*> cfcAlgList() const;

    // Можно ли создать ещё алгоритм гибкой логики
    bool canCreate() const;

    // Создать алгоритм гибкой логики, при отсутсвии свободных позиций, возвращает nullptr
    CfcAlgService* create();

    // Удаление алгоритма гибкой логики
    bool remove(CfcAlgService* cfcAlg);

private:
    // Забрать ioID, возвращает false, если ioID уже занят.
    bool takeIOID(uint8_t ioID);

    // Получить свободный ioID.
    std::optional<uint8_t> takeFreeIOID();

    // Освободить ioID.
    void releaseIOID(uint8_t ioID);

    DcController* config() const;

    CfcAlgService* insert(uint8_t position, CfcAlgService::UPtr cfcAlg);

private:
    DcController* m_config;
    std::vector<bool> m_ioIDList;
    std::vector<CfcAlgService::UPtr> m_cfcAlgList;
};

#endif // CFCALGMANAGER_H
