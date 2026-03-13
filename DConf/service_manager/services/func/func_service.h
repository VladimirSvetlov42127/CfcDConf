#ifndef __FUNC_SERVICE_H__
#define __FUNC_SERVICE_H__

#include <unordered_map>

#include "service_manager/services/service.h"
#include "service_manager/services/func/vfunc.h"

// Сервис функций виртуальных выходов.
// Позволяет назначать на вирутальные выходы функции, и задавать значения их аргументов.
// Регулирует механизм привязки/отвязки вирутальных входов при использовании функций, аргументом которых является виртуальный вход.
// Для корректной работы сервису необходим список всех виртуальных входов.

class DcController;

class FuncService : public Service
{
public:
    // Структура с информацией о типе функции виртуального выхода
    struct VFuncInfo
    {
        uint8_t id;         // идентификатор типа для данного устройства.
        VFunc::Type type;   // тип функции из общего списка типов функций виртуальных выходов.
        QString name;       // имя функции
    };

    FuncService(DcController* config);
    virtual ~FuncService() = default;

    // Тип сервиса
    Service::Type type() const override;

    // Инициализация списка функций виртуальных выходов. Настройка типов всех функций и привязок.
    bool init();

    // Сброс всех типов функций и их привязок. Тип всех функций устанавливается в "Не используется".
    // Если в списке поддерживаемых функций нет "Не используется", ничего не далает.
    void clearBindings();

    // Настройка типов всех функций и привязок.
    void rebind();

    // Количество функций.
    size_t size() const;

    // Список поддерживаемых типов функций, в виде ассоциативного контейнера.
    // ключ - идентификатор типа, значение - информация о типе.
    const std::map<uint8_t, VFuncInfo>& supportedTypes() const;

    // Список всех функций виртуальных выходов
    const VFunc::UPtrVector& funcList() const;

    // Имя функции по идентификатору типа id
    QString name(uint8_t id) const;

    // Тип функции по идентификатору типа id
    VFunc::Type type(uint8_t id) const;

    // Установка типа функции вирутального выхода под индексом idx в тип с идентификатором typeId.
    void setType(uint8_t idx, uint8_t typeId);

    // Установка значения аргумента функции вирутального выхода под индексом idx, в значение value.
    void setArgValue(uint8_t idx, uint8_t value);

private:
    DcController* config() const;

    // Возвращает vdin с subTypeId = vdinSubTypeID, или nullptr если нет такого.
    VirtualInputSignal* vdin(uint16_t vdinSubTypeID) const;

    // Создание выходa сервиса для функции func.
    // Если будет найден vdin c subTypeId равным значению аргумента функции, и этот vdin будет свободен,
    // Выход сервиса будет привязан к этому vdin.
    void makeOutput(VFunc* func);

    // Удаление выходa сервиса для функции func.
    // Если есть vdin привязанный к этому выход сервиса, он будет освобождён.
    void removeOutput(VFunc* func);

    // Возвращает true, если аргуметном функции с идентификатором типа typeId, является виртуальный вход
    // в противном случае fasle.
    bool hasVDinArg(uint8_t typeId) const;

private:
    DcController *m_config;
    std::unordered_map<uint8_t, VirtualInputSignal*> m_vdins;
    std::map<uint8_t, VFuncInfo> m_vfuncsInfo;
    VFunc::UPtrVector m_vfuncList;
};

#endif // __FUNC_SERVICE_H__
