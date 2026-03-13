#include "alg_manager.h"

#include "data_model/dc_controller.h"
#include "db/dc_db_manager.h"

AlgManager::AlgManager(DcController* config)
    : m_config{config}
{
}

bool AlgManager::init()
{
    m_usedAlgIds.clear();
    m_usedIOIds.clear();

    auto uid = config()->uid();
    if (uid) {
        gDbManager.execute(uid, "DELETE FROM matrix_alg");
    }

    // Загрузка всех сервисов из таблиц.
    for (size_t i = 0; i < config()->algs_internal()->size(); ++i) {
        auto internalAlg = config()->algs_internal()->get(i);
        load(internalAlg);
    }

    // Обновление всех загруженных сервисов.
    for(auto &[inAddr, alg]: m_algs) {
        update(alg->addr(), alg->name());
    }

    // Добавление дополнительных встроенных сервисов.
    update(SP_AINCMP_WORDIN_PARAM, "Сравнение Аналогов");
    update(SP_APPL_MANAGE_WORDDIN_PARAM, "Отображение группы уставок");
    update(SP_INDCON_WORDIN_PARAM, "Дискрет связи");

    rebind();
    return true;
}

void AlgManager::clearBindings()
{
    for(auto &[addr, alg]: m_algs) {
        for(auto& input: alg->inputs())
            input->setSource(nullptr);

        for(auto& output: alg->outputs())
            output->setTarget(nullptr);
    }
}

void AlgManager::rebind()
{
    for(auto &[addr, alg]: m_algs) {
        for(auto& input: alg->inputs()) {
            auto bindValue = static_cast<const ServiceInput*>(input.get())->bindElement()->value().toUInt();
            input->setSource(config()->serviceManager()->din(bindValue));
        }

        for(auto& output: alg->outputs()) {
            auto bindValue = static_cast<const ServiceOutput*>(output.get())->bindElement()->value().toUInt();
            output->setTarget(config()->serviceManager()->vdin(bindValue));
        }
    }
}

int AlgManager::count() const
{
    return m_algs.size();
}

QList<AlgService *> AlgManager::algList() const
{
    QList<AlgService*> result;
    for(auto &[addr, alg]: m_algs)
        result.append(alg.get());

    return result;
}

AlgService *AlgManager::alg(uint16_t addr) const
{
    if (auto algIt = m_algs.find(addr); algIt != m_algs.end())
        return algIt->second.get();

    return nullptr;
}

DcController *AlgManager::config() const
{
    return m_config;
}

void AlgManager::load(DcAlgInternal *internalAlg)
{
    if (!internalAlg || internalAlg->index() <= 0)
        return;

    auto id = internalAlg->index();
    uint16_t inAddr = internalAlg->property("addr").toUInt(nullptr, 16);
    uint16_t outAddr = inAddr + 1;
    if (!inAddr || this->alg(inAddr))
        return;

    auto alg = m_algs.emplace(inAddr, std::make_unique<AlgService>(id, internalAlg->name(), inAddr) ).first->second.get();
    setUsedAlgID(id);

    auto inParameter = config()->paramsRegistry().parameter(inAddr);
    auto outParameter = config()->paramsRegistry().parameter(outAddr);
    for(size_t i = 0; i < internalAlg->ios()->size(); ++i) {
        auto io = internalAlg->ios()->get(i);
        if (!io)
            continue;

        // Добавление входов
        if (inParameter && io->direction() == 1) {
            auto bindElement = inParameter->profiles().at(0).at(io->pin()).get();
            if (!bindElement)
                continue;

            alg->makeInput(io->index(), bindElement);
            setUsedIOID(io->index());
        }

        // Добавление выходов
        if (outParameter && io->direction() == 2) {
            auto bindElement = outParameter->profiles().at(0).at(io->pin()).get();
            if (!bindElement)
                continue;

            alg->makeOutput(io->index(), bindElement);
            setUsedIOID(io->index());
        }
    }
}

void AlgManager::update(uint16_t inAddr, const QString &name)
{
    if (!inAddr)
        return;

    uint16_t outAddr = inAddr + 1;
    auto inParameter = config()->paramsRegistry().parameter(inAddr);
    auto outParameter = config()->paramsRegistry().parameter(outAddr);
    auto uid = config()->uid();
    auto alg = this->alg(inAddr);
    if (!alg) {
        // Если нет сервиса, пробуем создать его и делаем запись в таблицу.
        if (!inParameter && !outParameter)
            return;

        alg = m_algs.emplace(inAddr, std::make_unique<AlgService>(takeFreeAlgID(), name, inAddr) ).first->second.get();
        if (uid) {
            DcProperties prop;
            prop.set("addr", QString("0x%1").arg(QString::number(inAddr, 16).toUpper()));
            auto query = QString("INSERT INTO algs(alg_id, position, name, properties) VALUES(%1, %2, '%3', '%4');")
                    .arg(alg->id())
                    .arg(-1)
                    .arg(alg->name(), prop.toJson());
            gDbManager.execute(uid, query);

            // Чистим хвосты в таблице входов/выходов
            query = QString("DELETE FROM alg_io WHERE alg_id = %1;").arg(alg->id());
            gDbManager.execute(uid, query);
        }
    }

    auto ioQuery = QString("INSERT INTO alg_io(io_id, alg_id, alg_pin, io_direction, name) VALUES(%1, %2, %3, %4, '%5');");

    // Добавление отсутствующих входов, с записью в таблицу
    if (inParameter && alg->inputs().size() < inParameter->elementsCount()) {
        auto &inProfile = inParameter->profiles().at(0);
        for(size_t i = alg->inputs().size(); i < inProfile.size(); ++i) {
            auto bindElement = inProfile.at(i).get();
            auto sin = alg->makeInput(takeFreeAlgIOID(), bindElement);
            if (uid)
                gDbManager.execute(uid, ioQuery.arg(sin->id()).arg(alg->id()).arg(sin->pin()).arg(1).arg(sin->name()));
        }
    }

    // Добавление отсутствующих выходов, с записью в таблицу
    if (outParameter && alg->outputs().size() < outParameter->elementsCount()) {
        auto &outProfile = outParameter->profiles().at(0);
        for(size_t i = alg->outputs().size(); i < outProfile.size(); ++i) {
            auto bindElement = outProfile.at(i).get();
            auto sout = alg->makeOutput(takeFreeAlgIOID(), bindElement);
            if (uid)
                gDbManager.execute(uid, ioQuery.arg(sout->id()).arg(alg->id()).arg(sout->pin()).arg(2).arg(sout->name()));
        }
    }
}

void AlgManager::setUsedAlgID(uint16_t id)
{
    m_usedAlgIds.emplace(id);
}

uint16_t AlgManager::takeFreeAlgID()
{
    for(size_t i = 1; i < std::numeric_limits<uint16_t>::max(); ++i)
        if (auto findIdIt = m_usedAlgIds.find(i); findIdIt == m_usedAlgIds.end()) {
            setUsedAlgID(i);
            return i;
        }

    return 0;
}

void AlgManager::setUsedIOID(uint16_t ioID)
{
    m_usedIOIds.emplace(ioID);
}

uint16_t AlgManager::takeFreeAlgIOID()
{
    for(size_t i = 1; i < std::numeric_limits<uint16_t>::max(); ++i)
        if (auto findIdIt = m_usedIOIds.find(i); findIdIt == m_usedIOIds.end()) {
            setUsedIOID(i);
            return i;
        }

    return 0;
}
