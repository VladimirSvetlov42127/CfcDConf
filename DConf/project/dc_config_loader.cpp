#include "dc_config_loader.h"

#include <QFile>
#include <QDebug>

#include "db/dc_db_manager.h"
#include "device_templates/dc_templates_manager.h"
#include "utils/bindings_update.h"

namespace {

constexpr const char* SETTINGS_BOARDS_VERSION = "boards_version";
constexpr const unsigned int BOARDS_UPDATE_STORAGE_VERSION = 2;

}

ConfigLoader::ConfigLoader(QObject *parent)
    : QObject{parent}
{
//    connect(&gDbManager, &DcDbManager::error, this, &ConfigLoader::onDbError);
}

ConfigLoader::~ConfigLoader()
{
}

DcController::UPtr ConfigLoader::load(const QString &filePath)
{
    return load(filePath, QString(), false);
}

DcController::UPtr ConfigLoader::load(const QString &filePath, const QString &name, bool updateFromTemplate)
{
//    qDebug() << "======================================================";

    auto config = gDbManager.load(filePath, name);
    if (!config) {
        emit error(QString("Ошибка загрузки конфигурации устройства из файла: %1").arg(filePath));
        return nullptr;
    }

    if (updateFromTemplate)
        updateController(config.get());

    config->loadServiceManager();

//    qDebug() << "------------------------------------------------------";
    return config;
}

void ConfigLoader::freeCache()
{
    m_cache.clear();
}

void ConfigLoader::onDbError(const QString &msg, const QString &filePath, DcController *device)
{
    emit error(QString("%1: %2").arg(msg, filePath));
}

void ConfigLoader::updateController(DcController *config)
{
    QString model = config->settingValue("model");
    QString modifier = config->settingValue("prodkey");
    QString templateFilePath = gTemplatesManager.deviceTemplateFilePath(model, modifier);

    DcController* templateConfig = nullptr;
    if (auto findIt = m_cache.find(templateFilePath); findIt != m_cache.end()) {
        templateConfig = findIt->second.get();
    }
    else {
        if (!QFile::exists(templateFilePath)) {
            emit error(QString("Не найден шаблон конфигурации устройства %1 (%2), обновление недоступно.")
                          .arg(model, modifier));
            return;
        }

        auto tempCfg = gDbManager.load(templateFilePath);
        if (!tempCfg) {
            emit error(QString("Ошибка чтения шаблона конфигурации устройства %1, обновление недоступно.")
                          .arg(templateFilePath));
            return;
        }

        templateConfig = m_cache.emplace(templateFilePath, std::move(tempCfg)).first->second.get();
    }

    config->beginTransaction();
    updateParams(config, templateConfig);
    updateBoards(config, templateConfig);
    updateSignals(config, templateConfig);
    updateAlgs(config, templateConfig);
    updateSettings(config, templateConfig);
    config->endTransaction();

    // Отладка
//    for(size_t i = 0; i < config->boards().size(); ++i) {
//        auto board = config->boards().at(i).get();
//        auto tBoard = templateConfig->boards().at(i).get();
//        if (!board->paramsRegistry().isEqual(&tBoard->paramsRegistry()))
//            qDebug() << "Board" << i + 1 << board->type() << "!!!NotEqual!!!";
//        else
//            qDebug() << "Board" << i + 1 << board->type() << "IsEqual, size:" << board->paramsRegistry().size() << tBoard->paramsRegistry().size();
//    }

//    if (config->paramsRegistry().isEqual(&templateConfig->paramsRegistry()))
//        qDebug() << "Parmas IsEqual, size: " << config->paramsRegistry().size() << templateConfig->paramsRegistry().size();
//    else
//        qDebug() << "Params !!!NotEqual!!!";
}

void ConfigLoader::updateParams(DcController *contr, DcController *temp)
{
    // Принудительно удаляем параметры записи паролей. ВРЕМЕНОЕ РЕШЕНИЕ для отключения перезаписи паролей, на устройстве.
    contr->paramsRegistry().remove(SP_PASSWORDMAN);
    contr->paramsRegistry().remove(SP_PASSWORDNET);

    // Список особых праметров, значения элементов которых надо обновлять всегда.
    auto specialParams = DcController::specialParams(DcController::UpdatableParam);

    // Отмечаем для удаления параметры которых больше нет в шаблоне и обновляем те которые есть, по шаблону.
    std::vector<uint16_t> paramsToRemove;
    for(auto &[addr, parameter]: contr->paramsRegistry()) {
        // Получаем параметр с таким же адресом из шаблона
        auto tempParameter = temp->paramsRegistry().parameter(addr);
        if (!tempParameter) {
            paramsToRemove.emplace_back(addr);
            continue;
        }

        // Обновление свойств параметра, по шаблонному параметру. обновляются только отличающиеся свойства.
        parameter->setName(tempParameter->name());
        parameter->setType(tempParameter->type());
        parameter->setDataSize(tempParameter->dataSize());
        parameter->setAttributes(tempParameter->attributes());

        // Запоминаем количество элементов в параметре.
        auto oldProfilesCount = parameter->profilesCount();
        auto oldElementsCount = parameter->elementsCount();

        // Меняем размерность параметра, по шаблоному параметру. Если размерность увеличилась, в параметре будут созданы новые элементы.
        parameter->setProfilesCount(tempParameter->profilesCount());
        parameter->setElementsCount(tempParameter->elementsCount());

        // Проходимся по элементам параметра
        for(size_t i = 0; i < parameter->profilesCount(); ++i) {
            auto profile = parameter->profile(i);
            for(size_t j = 0; j < profile->size(); ++j) {
                auto element = profile->at(j).get();
                auto tempElement = tempParameter->profile(i)->at(j).get();

                // Для старых элементов, которые не относятся к особым параметрам обновляем только имя, по шаблоному элементу.
                if ((i < oldProfilesCount) && (j < oldElementsCount) && !specialParams.contains(parameter->addr())) {
                    element->setName(tempElement->name());
                    continue;
                }

                // Для новых элементов и тех которые относятся к особым параметрам обновляем имя и значение, по шаблоному элементу.
                element->update(tempElement->name(), tempElement->value());
            }
        }
    }

    // Удаление параметров которых нет в шаблоне
    for(auto addr: paramsToRemove)
        contr->paramsRegistry().remove(addr);

    // Добавлнеие параметров которые есть в шаблоне, но нету в конфигурации
    for(auto &[addr, tempParameter]: temp->paramsRegistry()) {
        auto parameter = contr->paramsRegistry().parameter(addr);
        if (parameter)
            continue;

        contr->paramsRegistry().add(tempParameter->clone());
    }
}

void ConfigLoader::updateBoards(DcController *contr, DcController *temp)
{
    // !!! При чтении параметров плат из файла базы данных конфигурации не обрабатывается ситуация с битыми записями.
    // !!! Потенциально возможна ситуация при которой не получится вставить новую запись из-за наличия записи с таким же 'primary key'

    // Cверка версий плат шаблона и конфигурации
    uint32_t tempBoardsVersion = temp->settingValue(SETTINGS_BOARDS_VERSION, "0").toUInt();
    uint32_t contrBoardsVersion = contr->settingValue(SETTINGS_BOARDS_VERSION, "0").toUInt();

    if (contrBoardsVersion >= tempBoardsVersion)
        return;

    if (contrBoardsVersion < BOARDS_UPDATE_STORAGE_VERSION)
        DcBoard::updateStorage(contr->uid());

    // У устройства из конфигурации
    // Запоминаем занчения параметров у интелектуальных(slot > -1) плат(ориентируемся на слоты) и затем удаляем все платы и параметры на них.
    QHash<QString, QString> boardsParamValues;
    QString keyTemplate = "%1-%2-%3";
    while (contr->boards().size()) {
        auto board = contr->boards().back().get();
        while ( board->paramsRegistry().size() ) {
            auto parameter = board->paramsRegistry().begin()->second.get();
            if (board->slot() > -1)
                for(auto &profile: parameter->profiles()) {
                    for(auto &element: profile) {
                        auto key = keyTemplate.arg(board->slot()).arg(element->addr()).arg(element->position());
                        boardsParamValues[key] = element->value();
                    }
                }

            board->paramsRegistry().remove(parameter->addr());
        }

        contr->remove(board);
    }

    // Заполняем платы и их параметры из шаблона.
    // Если встречается параметр, который ранее был на какой то плате(ориентируемся на слоты), востанавливаем его значение
    for(auto &tempBoard: temp->boards()) {
        auto newBoardUPtr = std::make_unique<DcBoard>(tempBoard->id(), tempBoard->slot(), tempBoard->type(), tempBoard->inst(), tempBoard->properties(), contr);
        auto newBoard = newBoardUPtr.get();
        contr->append(std::move(newBoardUPtr));

        for(auto& [tempAddr, tempParameter]: tempBoard->paramsRegistry()) {
            auto parameter = newBoard->paramsRegistry().add(tempParameter->clone());

            for(auto& profile: parameter->profiles())
                for(auto& element: profile) {
                    auto key = keyTemplate.arg(newBoard->slot()).arg(element->addr()).arg(element->position());
                    if (auto findIt = boardsParamValues.find(key); findIt != boardsParamValues.end())
                        element->updateValue(findIt.value());
                }
        }
    }

    // Запись о версии плат из шаблона в конфигурацию
    contr->setSetting(SETTINGS_BOARDS_VERSION, QString::number(tempBoardsVersion));
}

void ConfigLoader::updateSignals(DcController *contr, DcController *temp)
{
    // генератор ключа сигнала(внутрений номер, тип, направление, подтип)
    auto makeKey = [](DcSignal* s) { return QString("%1-%2-%3-%4").arg(s->type()).arg(s->direction()).arg(s->subtype()).arg(s->internalId()); };

    // создаем контейнер сигналов из конфигурации шаблона
    std::unordered_map<QString, DcSignal*> tempConfigSignlas;
    for (auto&& t : temp->getSignalList()) {
        tempConfigSignlas.insert({ makeKey(t), t});
    }

    // проходимся по списку сигналов из конфигурации устройства, и ищем соответсвующий сигнал в контейнере из конфигурации шаблона
    // если не находим, то удаляем сигнал из конфигурации устройства.
    // если нашли, то коректируем свойства и имя в сигнале из конфигурации устройства, на соответствующие из сигнала кофнигурации шаблона и
    // удаляем соответсвующий сигнал из контейнера конфигурации шаблона.

    int32_t maxIndex = 0;
    for(auto &&s: contr->getSignalList()) {
        auto findIt = tempConfigSignlas.find(makeKey(s));
        if (findIt == tempConfigSignlas.end()) {
            contr->removeSignal(s->internalId(), s->type(), s->direction());
            continue;
        }

        auto t = findIt->second;
        if (s->properties() != t->properties()) {
            for (auto& [tpName, tpValue]: t->properties())
                s->updateProperty(tpName, tpValue);
        }

        if (s->subtype() != DEF_SIG_SUBTYPE_VIRTUAL)
            s->updateName(t->name());

        tempConfigSignlas.erase(findIt);
        maxIndex = std::max(maxIndex, s->index());
    }
    maxIndex++;

    // проходимся по всем оставшемся сигналам из контейнера конфигурации шаблона и добавляем их в конфигурацию устройства.
    for (auto &it: tempConfigSignlas) {
        auto t = it.second;
        auto signal = new DcSignal(maxIndex++, t->internalId(), t->direction(), t->type(), t->subtype(), t->name(), t->properties().toJson(), contr);
        contr->addSignal(signal, false);
    }
}

void ConfigLoader::updateAlgs(DcController *contr, DcController *temp)
{
    // удаление
    int32_t maxIndex = 0;
    for (int i = contr->algs_internal()->size() - 1; i >= 0; i--) {
        DcAlgInternal *c = contr->algs_internal()->get(i);
        DcAlgInternal *same = nullptr;
        for (size_t l = 0; l < temp->algs_internal()->size(); l++) {
            DcAlgInternal *t = temp->algs_internal()->get(l);
            if (*t == *c) {
                same = t;
                break;
            }
        }
        if (!same)
            contr->algs_internal()->remove(i);
        else {
            maxIndex = std::max(maxIndex, c->index());
            for (int j = c->ios()->size() - 1; j >= 0; j--) {
                DcAlgIOInternal *cc = c->ios()->get(j);
                DcAlgIOInternal *tt = same->ios()->getById(cc->index());
                if (!tt)
                    c->ios()->remove(j);
            }
        }
    }

    // добавление
    maxIndex++;
    for (size_t i = 0; i < temp->algs_internal()->size(); i++) {
        DcAlgInternal *t = temp->algs_internal()->get(i);
        DcAlgInternal *same = nullptr;
        for (size_t l = 0; l < contr->algs_internal()->size(); l++) {
            DcAlgInternal *c = contr->algs_internal()->get(l);
            if (*t == *c) {
                same = c;
                break;
            }
        }
        if (!same) {
            same = new DcAlgInternal(maxIndex++, t->position(), t->name(), t->properties().toJson(), contr);
            contr->algs_internal()->add(same, false);
        }

        for (size_t j = 0; j < t->ios()->size(); j++) {
            DcAlgIOInternal *tt = t->ios()->get(j);
            if (!same->ios()->getById(tt->index())) {
                DcAlgIOInternal *cc = new DcAlgIOInternal(tt->index(), same->index(), tt->pin(), tt->direction(), tt->name(), contr);
                same->ios()->add(cc, false);
            }
        }
    }
}

void ConfigLoader::updateSettings(DcController *contr, DcController *temp)
{
    const QStringList settingsToUpdateList { "MASK_BYTES_COUNT" };

    for (auto &[settingName, tempSetting]: temp->settings()) {
        if (tempSetting->name() == "cfg_version") // временно отключаем обработку версии конфигурации.
            continue;

        // Добавляем только новые и обновялем которые в списке settingsToUpdateList
        if (!contr->setting(tempSetting->name()) || settingsToUpdateList.contains(tempSetting->name()))
            contr->setSetting(tempSetting->name(), tempSetting->value());
    }
}
