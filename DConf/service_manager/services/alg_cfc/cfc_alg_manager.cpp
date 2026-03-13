#include "cfc_alg_manager.h"

#include <QFile>

#include "data_model/dc_controller.h"
#include "db/dc_db_manager.h"

CfcAlgManager::CfcAlgManager(DcController *config)
    : m_config{config}
{
}

bool CfcAlgManager::reload()
{
    clear();

    auto cfcBindParam = config()->paramsRegistry().parameter(SP_FLEXLGCROSSTABLE);
    if (!cfcBindParam)
        return true;

    m_cfcAlgList.resize(cfcBindParam->profilesCount());
    m_ioIDList.resize(cfcBindParam->profilesCount() * cfcBindParam->elementsCount(), false);
    for (uint8_t position = 0; position < cfcBindParam->profilesCount(); ++position) {
        insert(position, CfcAlgService::load(position, this));
    }

    return true;
}

void CfcAlgManager::clear()
{
    for(auto &cfcAlg: m_cfcAlgList)
        remove(cfcAlg.get());

    for(size_t i = 0; i < m_ioIDList.size(); ++i)
        m_ioIDList[i] = false;

    auto uid = config()->uid();
    if (uid) {
        gDbManager.execute(uid, "DELETE FROM matrix_alg_cfc");
        gDbManager.execute(uid, "DELETE FROM alg_cfc_io;");
        gDbManager.execute(uid, "DELETE FROM algs_cfc");
    }
}

QList<CfcAlgService *> CfcAlgManager::cfcAlgList() const
{
    QList<CfcAlgService*> result;
    for(auto &service: m_cfcAlgList)
        if (service)
            result.append(service.get());

    return result;
}

bool CfcAlgManager::canCreate() const
{
    for(auto &service: m_cfcAlgList)
        if (!service)
            return true;

    return false;
}

CfcAlgService *CfcAlgManager::create()
{
    std::optional<uint8_t> freePos;
    for(size_t i = 0; i < m_cfcAlgList.size(); ++i)
        if (!m_cfcAlgList.at(i)) {
            freePos = i;
            break;
        }

    if (!freePos)
        return nullptr;

    uint8_t position = freePos.value();
    return insert(position, CfcAlgService::create(position, this));
}

bool CfcAlgManager::remove(CfcAlgService *cfcAlg)
{
    if (!cfcAlg)
        return true;

    std::optional<uint8_t> position;
    for(uint8_t i = 0; i < m_cfcAlgList.size(); ++i) {
        if (m_cfcAlgList[i].get() == cfcAlg) {
            position = i;
            break;
        }
    }

    if (!position)
        return false;

    auto uid = config()->uid();
    if (uid) {
        auto query = QString("DELETE FROM algs_cfc WHERE cfc_alg_id = %1;").arg(cfcAlg->id());
        gDbManager.execute(uid, query);
    }

    cfcAlg->clear();
    m_cfcAlgList[position.value()].reset();
    return true;
}

bool CfcAlgManager::takeIOID(uint8_t ioID)
{
    uint8_t index = ioID - 1;
    if (index < m_ioIDList.size() && !m_ioIDList[index]) {
        m_ioIDList[index] = true;
        return true;
    }

    return false;
}

std::optional<uint8_t> CfcAlgManager::takeFreeIOID()
{
    for(size_t i = 0; i < m_ioIDList.size(); ++i)
        if (!m_ioIDList[i]) {
            m_ioIDList[i] = true;
            return i + 1;
        }

    return {};
}

void CfcAlgManager::releaseIOID(uint8_t ioID)
{
    uint8_t index = ioID - 1;
    if (index < m_ioIDList.size())
        m_ioIDList[index] = false;
}

DcController *CfcAlgManager::config() const
{
    return m_config;
}

CfcAlgService *CfcAlgManager::insert(uint8_t position, CfcAlgService::UPtr cfcAlg)
{
    if (!cfcAlg || position >= m_cfcAlgList.size())
        return nullptr;

    auto uid = config()->uid();
    if (uid) {
        auto query = QString("INSERT INTO algs_cfc(cfc_alg_id, cfc_position, name, properties) VALUES(%1, %2, '%3', '%4');")
                .arg(cfcAlg->id())
                .arg(position)
                .arg(cfcAlg->name(), cfcAlg->properties());
        gDbManager.execute(uid, query);
    }

    m_cfcAlgList[position] = std::move(cfcAlg);
    return m_cfcAlgList[position].get();
}
