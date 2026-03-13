#ifndef __SERVICE_MANAGER_H__
#define __SERVICE_MANAGER_H__

#include <map>

#include <QObject>

#include "service_manager/services/alg/alg_manager.h"
#include "service_manager/services/alg_cfc/cfc_alg_manager.h"
#include "service_manager/services/func/func_service.h"

#include "service_manager/signals/input_signal.h"
#include "service_manager/signals/virtual_input_signal.h"
#include "service_manager/signals/output_signal.h"

class DcController;

//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
class ServiceManager : public QObject
{
    Q_OBJECT

public:
    //===============================================================================================================================================
    //	Конструктор класса
    //===============================================================================================================================================
    ServiceManager(DcController *controller);

    // Первичная загрузка сигналов, встроенных алгоритмов/сервисов, гибкой логики, виртуальных функций. Настройка привязок.
    bool init();

    // Настройка привязок.
    bool rebind();

    // Очистка привязок.
    void clearBindings();

    QList<InputSignal*> dins() const;
    QList<OutputSignal*> douts() const;
    QList<VirtualInputSignal*> vdins() const;
    QList<VirtualInputSignal*> freeVdins() const;
    QList<VirtualInputSignal*> busyVdins() const;

    InputSignal* din(uint16_t internalID) const;
    VirtualInputSignal* vdin(uint16_t internalID) const;

    AlgManager& algManager();
    const AlgManager& algManager() const;

    CfcAlgManager& cfcManager();
    const CfcAlgManager& cfcManager() const;

    FuncService& funcService();
    const FuncService& funcService() const;

    //===============================================================================================================================================
    //	Отладочный вывод
    //===============================================================================================================================================
    void printAlgs();
    void printDouts();
    void printDins();

signals:
    //===============================================================================================================================================
    //	Сигналы для логирования
    //===============================================================================================================================================
    void infoToLog(const QString& message);
    void warningToLog(const QString& message);
    void errorToLog(const QString& message);

private:
    //===============================================================================================================================================
    //	Вспомогательные методы класса
    //===============================================================================================================================================
    DcController* controller() { return m_controller; }
    void setBindingParams();
    bool loadSignals();

private:
    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    DcController* m_controller;
    std::map<uint16_t, std::unique_ptr<InputSignal>> m_dins;
    std::map<uint16_t, std::unique_ptr<OutputSignal>> m_douts;

    AlgManager m_algManager;
    CfcAlgManager m_cfcManager;
    FuncService m_funcService;
};

#endif // __SERVICE_MANAGER_H__
