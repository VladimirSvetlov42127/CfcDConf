#ifndef PARAMETERREGISTRY_H
#define PARAMETERREGISTRY_H

#include <vector>
#include <unordered_map>

#include "data_model/parameters/parameter.h"

// ParameterRegistry - Реестр параметров с уникальными адресами.
// По сути контейнер с доступом к параметрам по адресу, а также обходом контейнера через итераторы.

class DcController;

class ParameterRegistry
{
public:
    // uid - идентификатор реестера, используется в параметрах реестера, и их элементах при запросах к базе данных
    ParameterRegistry();

    // Методы для получения стандартных итераторов. Нужны для цикла по диапазону.
    auto begin() { return m_params.begin(); }
    auto end() { return m_params.end(); }
    auto begin() const { return m_params.begin(); }
    auto end() const { return m_params.end(); }
    auto cbegin() const noexcept { return m_params.cbegin(); }
    auto cend() const noexcept { return m_params.cend(); }

    // Идентификатор базы данных. Используется параметрами и их элементами, для обновления своих данных в базе данных.
    // Когда установлен в 0, никаких взаимодйетсвий с базой данных не производтся.
    int32_t uid() const;

    void setDevice(DcController *device);

    // Количество параметров в реестере
    uint32_t size() const;

    // Добавляет параметр param в реестр. Возвращает сырой указаетль на вставленый параметр.
    // Если установлен uid, будет вставка параметра и всех его элементов в базу данных.
    Parameter* add(Parameter::UPtr param);

    // Удаление параметра с адресом addr из реестера.
    // Если установлен uid, удалится параметр и все его элементы из базы данных
    bool remove(uint32_t addr);

    // Возвращает параметр с адресом addr, или nullptr если такого параметра нет в реестере.
    Parameter* parameter(uint16_t addr) const;

    // Возвращает элемент параметра с адресом addr в позиции position, или nullptr если такого элемента нет в реестере.
    ParameterElement* element(uint16_t addr, uint16_t position) const;

    // Отладка
    bool isEqual(ParameterRegistry *other) const;

private:
    DcController *m_device;
    std::unordered_map<uint16_t, Parameter::UPtr> m_params;
};

#endif // PARAMETERREGISTRY_H
