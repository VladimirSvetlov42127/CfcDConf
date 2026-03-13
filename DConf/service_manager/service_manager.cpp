#include "service_manager.h"

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>
#include <QFile>
#include <QMap>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "data_model/dc_controller.h"
#include "utils/bindings_update.h"
#include "db/dc_db_manager.h"

namespace {

}

//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
ServiceManager::ServiceManager(DcController *controller)
    : m_controller{controller}
    , m_algManager{controller}
    , m_cfcManager{controller}
    , m_funcService{controller}
{
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
bool ServiceManager::init()
{
    auto service_version_setting = "service_version";
    auto service_version = m_controller->settingValue(service_version_setting, "0").toUInt();
//    if (!service_version)
        setBindingParams();

    //  Загрузка сигналов
    if (!loadSignals()) {
        emit errorToLog("Ошибка загрузки сигналов в менеджер сигналов.");
        return false;
    }

    //  Загрузка привязок встроенных алгоритмов
    if (!algManager().init())
        emit warningToLog("Проблемы с загрузкой привязок встроенных алгоритмов.");

    //  Загрузка привязок алгоритмов гибкой логики
    if (!cfcManager().reload())
        emit warningToLog("Проблемы с загрузкой привязок алгоритмов гибкой логики.");

    //  Загрузка привязок функций
    if (!funcService().init())
        emit warningToLog("Проблемы с загрузкой привязок виртуальных функций.");

    m_controller->setSetting(service_version_setting, QString::number(++service_version));
    return true;
}

bool ServiceManager::rebind()
{
    for(auto &[id, dout]: m_douts) {
        auto bindValue = static_cast<const OutputSignal*>(dout.get())->bindElement()->value().toUInt();
        dout->setSource(din(bindValue));
    }

    algManager().rebind();
    cfcManager().reload();
    funcService().rebind();
    return true;
}

void ServiceManager::clearBindings()
{
    funcService().clearBindings();
    cfcManager().clear();
    algManager().clearBindings();
    for(auto &[id, dout]: m_douts) {
        dout->setSource(nullptr);
    }
}

QList<InputSignal *> ServiceManager::dins() const
{
    QList<InputSignal*> result;
    for(auto it = m_dins.begin(); it != m_dins.end(); ++it)
        result.append(it->second.get());
    return result;
}

QList<VirtualInputSignal *> ServiceManager::vdins() const
{
    QList<VirtualInputSignal*> result;
    for(auto it = m_dins.begin(); it != m_dins.end(); ++it)
        if (auto s = dynamic_cast<VirtualInputSignal*>(it->second.get()); s)
            result.append(s);
    return result;
}

QList<OutputSignal *> ServiceManager::douts() const
{
    QList<OutputSignal*> result;
    for(auto it = m_douts.begin(); it != m_douts.end(); ++it)
        result.append(it->second.get());
    return result;
}

QList<VirtualInputSignal *> ServiceManager::freeVdins() const
{
    QList<VirtualInputSignal*> result;
    for(auto it = m_dins.begin(); it != m_dins.end(); ++it)
        if (auto s = dynamic_cast<VirtualInputSignal*>(it->second.get()); s && !s->source())
            result.append(s);
    return result;
}

QList<VirtualInputSignal *> ServiceManager::busyVdins() const
{
    QList<VirtualInputSignal*> result;
    for(auto it = m_dins.begin(); it != m_dins.end(); ++it)
        if (auto s = dynamic_cast<VirtualInputSignal*>(it->second.get()); s && s->source())
            result.append(s);
    return result;
}

InputSignal *ServiceManager::din(uint16_t internalID) const
{
    if (auto it = m_dins.find(internalID); it != m_dins.end())
        return it->second.get();

    return nullptr;
}

VirtualInputSignal *ServiceManager::vdin(uint16_t internalID) const
{
    if (auto it = m_dins.find(internalID); it != m_dins.end())
        return dynamic_cast<VirtualInputSignal*>(it->second.get());

    return nullptr;
}

AlgManager &ServiceManager::algManager()
{
    return m_algManager;
}

const AlgManager &ServiceManager::algManager() const
{
    return m_algManager;
}

CfcAlgManager &ServiceManager::cfcManager()
{
    return m_cfcManager;
}

const CfcAlgManager &ServiceManager::cfcManager() const
{
    return m_cfcManager;
}

FuncService &ServiceManager::funcService()
{
    return m_funcService;
}

const FuncService &ServiceManager::funcService() const
{
    return m_funcService;
}

//===================================================================================================================================================
//	Отладочный вывод
//===================================================================================================================================================
void ServiceManager::printAlgs()
{
    qDebug() << "======================================";
    qDebug() << "======================== SERVICEs";
    qDebug() << "======================================";

    for(auto &service: algManager().algList()) {
        bool isInputBinded = false;
        for(auto &input: service->inputs())
            if (input->source()) {
                isInputBinded = true;
                break;
            }

        bool isOutputBinded = false;
        for(auto &output: service->outputs())
            if (output->target()) {
                isOutputBinded = true;
                break;
            }

        if (!isInputBinded && !isOutputBinded)
            continue;

        service->print();
    }
}

void ServiceManager::printDouts()
{
    qDebug() << "======================================";
    qDebug() << "========================== DOUTs";
    qDebug() << "======================================";

    //
    // auto dinToString = [](InputSignal *in) {
    //     if (!in)
    //         return QString();
    //
    //     return QString("<--------- (%1) %2").arg(in->internalID()).arg(in->name());
    // };
    //
    // QString outTemp = QString("(%1) %2: %3");
    // for(auto &[id, dout]: m_douts) {
    //     // if (!dout->source())
    //     //     continue;
    //
    //     qDebug().noquote() << outTemp.arg(dout->internalID()).arg(dout->name(), dinToString(dout->source()));
    // }
    //

    for (size_t i = 0; i < m_douts.size(); i++) {
        auto output = m_douts.find(i)->second.get();
        qDebug() << i << output->name() << " ----> " << output->text() << "  =====> " << output->fullText();
    }
}

void ServiceManager::printDins()
{
    qDebug() << "======================================";
    qDebug() << "========================== DINs";
    qDebug() << "======================================";

    //
    // auto targetsToString = [](InputSignal *in) {
    //     if (!in)
    //         return QString();
    //
    //     QStringList list;
    //     for(auto target: in->targets()) {
    //         list.append(QString("%1").arg(target->name()));
    //     }
    //     return QString("---------> %1").arg(list.join(" | "));
    // };
    //
    // QString inTemp = QString("(%1) %2: %3");
    // for(auto &[id, din]: m_dins) {
    //     // if (din->targets().empty())
    //     //     continue;
    //
    //     qDebug().noquote() << inTemp.arg(din->internalID()).arg(din->name(), targetsToString(din.get()));
    // }
    //

    for (size_t i = 0; i < m_dins.size(); i++) {
        auto input = m_dins.find(i)->second.get();
        qDebug() << i << input->name() << " ----> " << input->text() << "=====>" << input->fullText();
    }
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void ServiceManager::setBindingParams()
{
    //  Синхронизация таблиц привязки
    BindingsUpdate updater(controller());
    updater.tablesToParams();

    return;
}

bool ServiceManager::loadSignals()
{
    auto uid = m_controller->uid();
    if (uid) {
        auto query = QString("DELETE FROM matrix_signals");
        gDbManager.execute(uid, query);
    }

    // Загрузка дискретных входов
    uint16_t in_subtype = 0;
    uint16_t in_ad = 0;
    uint16_t in_physical = 0;
    uint16_t in_logic = 0;
    uint16_t in_virtual = 0;
    uint16_t in_remote = 0;

    for(auto signal: m_controller->getSignalList(DEF_SIG_TYPE_DISCRETE)) {
        if (signal->subtype() == DEF_SIG_SUBTYPE_LED_AD)
            in_subtype = in_ad++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_PHIS)
            in_subtype = in_physical++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_LOGIC)
            in_subtype = in_logic++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_VIRTUAL)
            in_subtype = in_virtual++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_REMOTE)
            in_subtype = in_remote++;

        if (DEF_SIG_SUBTYPE_VIRTUAL == signal->subtype()) {
            auto vdin = std::make_unique<VirtualInputSignal>(signal->index(),
                                                             signal->internalId(),
                                                             signal->direction(),
                                                             signal->type(),
                                                             signal->subtype(),
                                                             "Виртуальный вход " + QString::number(in_subtype),
                                                             in_subtype);
            m_dins.emplace(signal->internalId(), std::move(vdin));
        }
        else {
            auto din = std::make_unique<InputSignal>(signal->index(),
                                                     signal->internalId(),
                                                     signal->direction(),
                                                     signal->type(),
                                                     signal->subtype(),
                                                     signal->name(), in_subtype);
            m_dins.emplace(signal->internalId(), std::move(din));
        }
    }

    // Загрузка дискретных выходов
    uint16_t out_subtype = 0;
    uint16_t out_physical = 0;
    uint16_t out_logic = 0;
    uint16_t out_virtual = 0;
    uint16_t out_remote = 0;

    for(auto signal: m_controller->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_UNDEF, DEF_SIG_DIRECTION_OUTPUT)) {
        auto bindElement = m_controller->paramsRegistry().element(0x02F2, signal->internalId());
        if (!bindElement)
            continue;

        if (signal->subtype() == DEF_SIG_SUBTYPE_PHIS || signal->subtype() == DEF_SIG_SUBTYPE_LED_AD)
            out_subtype = out_physical++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_LOGIC)
            out_subtype = out_logic++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_VIRTUAL)
            out_subtype = out_virtual++;
        if (signal->subtype() == DEF_SIG_SUBTYPE_REMOTE)
            out_subtype = out_remote++;

        std::unique_ptr<OutputSignal> dout;
        if (DEF_SIG_SUBTYPE_VIRTUAL == signal->subtype()) {
            dout = std::make_unique<VirtualOutputSignal>(signal->index(),
                                                   signal->internalId(),
                                                   signal->direction(),
                                                   signal->type(),
                                                   signal->subtype(),
                                                   signal->name(),
                                                   bindElement, out_subtype);
        }
        else {
            dout = std::make_unique<OutputSignal>(signal->index(),
                                                   signal->internalId(),
                                                   signal->direction(),
                                                   signal->type(),
                                                   signal->subtype(),
                                                   signal->name(),
                                                   bindElement, out_subtype);
        }

        if (auto it = m_dins.find(bindElement->value().toUInt()); it != m_dins.end())
            dout->setSource(it->second.get());        

        m_douts.emplace(signal->internalId(), std::move(dout));
    }

    return true;
}
