#ifndef PARAMETER_H
#define PARAMETER_H

#include <memory>
#include <vector>

#include "data_model/parameters/parameter_element.h"

#define PROFILE_SIZE 256

// Parameter - параметр устройства depRTU.
// Хранит в себе множество элементов параметра (ParameterElement). Каждый элемент характиризуется уникальным числом - позиция (position).
// Parameter::Profile - список в котором содержатся элементы параметра.
// Количество профилей и количество элементов определяется размерностью параметра.
// у одномерных параметров 1 профиль, в котором может быть до 65536 элементов.
// у двумерных может быть до 256 профилей, в каждом до 256 элементов.
// Значение позиции первого элемента в каждом профиле кратно 256. Т.е равно N * 256, где N - индекс профиля.
// Значения позиций (position) элементов в рамках одного профиля, образуют непрерывную последовательность.

// Виртуальный параметр иммет только тип, адрес и атрибуты, всегда одномерный. Понятие введенно для параметров плат,
// так как для них нет таблицы с описанием параметра в базе данных. Создается с помощью 2го конструктора.

class ParameterRegistry;

class Parameter
{
public:
    using UPtr = std::unique_ptr<Parameter>;

    // Профиль - список элементов параметра.
    using Profile = ParameterElement::UPtrVector;

    Parameter(const QString &name, uint8_t type, uint16_t addr, const Dpc::Sybus::ParamAttribute &attrs, uint16_t profilesCount, uint16_t elementsCount, uint16_t dataSize);

    // Констурктор для создания виртуального параметра, когда известен только тип, адрес и атрибуты параметра. !!!ОБРАТНАЯ СОВМЕСТИМОСТЬ!!!
    Parameter(uint8_t type, uint16_t addr, const Dpc::Sybus::ParamAttribute &attrs);

    // Идентификатор реестра параметров к которому принадлежит параметр
    // Соответсвует uid() реестра или 0, если реестр не установлен
    int32_t uid() const;

    // Общее имя параметра
    QString name() const;
    void setName(const QString &name);

    // Тип параметра
    uint8_t type() const;
    void setType(uint8_t type);

    // Адрес параметра
    uint16_t addr() const;

    // Аттрибуты параметра
    Dpc::Sybus::ParamAttribute attributes() const;
    void setAttributes(const Dpc::Sybus::ParamAttribute &attr);

    // Количество профилей элементов, максимум 256.
    // Если установлен uid, соответсвующие изменения будут и в базе данных.
    uint16_t profilesCount() const;
    void setProfilesCount(uint16_t count);

    // Количество элементов в профиле, максимум 65535 для 1мерных, и 256 для 2мерных.
    // Если установлен uid, соответсвующие изменения будут и в базе данных.
    uint16_t elementsCount() const;
    void setElementsCount(uint16_t count);

    // размер значения одного элемента в байтах
    uint16_t dataSize() const;
    void setDataSize(uint16_t size);

    // Для проверки валидности
    bool isValid() const;

    // Указатель на реестр параметров, к которому относится параметр
    const ParameterRegistry* registry() const;
    void setRegistry(ParameterRegistry* registry);

    // Указаетль на профиль под индексом idx
    const Parameter::Profile* profile(uint8_t idx) const;

    // Возвращает ссылку на список профилей элементов параметра
    const std::vector<Parameter::Profile>& profiles() const;

    // Только для ВИРТУАЛЬНОГО параметра. Добавляет элемент element с соответствующей позицией в параметр.
    // Если параметр виртуальный и получившийся индекс в профиле равен размеру профиля, добавляется в конец профиля и возвращает указатель.
    // Во всех остальных случаях ничего не делает и возвращает nullptr.
    // Если установлен uid, будет вставка элемента в базу данных.
    ParameterElement* append(ParameterElement::UPtr element);

    // Возвращает элемент параметра с позицией position
    ParameterElement* element(uint16_t position);
    const ParameterElement* element(uint16_t position) const;

    // Возвращает элемент параметра с индексом idx в профиле c индексом profileIdx
    ParameterElement* element(uint8_t profileIdx, uint16_t idx);
    const ParameterElement* element(uint8_t profileIdx, uint16_t idx) const;

    // Методы для работы с базой.    
    // Если установлен uid, также добавятся все элементы в базу данных.
    void insert();    
    void update();    
    // Если установлен uid, также удалятся все элементы из базы данных.
    void remove();    

    // Созадние копии данного параметра
    Parameter::UPtr clone() const;

    // Отладка
    QString toString() const;
    bool isEqual(Parameter* other) const;

private:
    // Максимальное количество элементов в профиле, зависит от размерности параметра
    uint16_t maxElementsCount() const;

    // Указаетль на профиль под индексом idx
//    ParameterProfile* profile(uint8_t idx);

    // Вычисляет индекс профиля в котором должен содержатся элемент с позицией position
    uint8_t profileIndex(uint16_t position) const;

    // Вычисляет индекс элемента в рамках профиля для элемента с позицией position
    uint16_t elementIndexPerProfile(uint16_t position) const;

    // Специальный случай, когда отсутсвуют часть данных (name, profileCount, elementCount, dataSize) и нет таблицы с headline. Для совместимости с boards
    bool isVirtual() const;

private:
    QString m_name;
    uint8_t m_type;
    uint16_t m_addr;
    Dpc::Sybus::ParamAttribute m_attrs;
    std::vector<Parameter::Profile> m_profiles;
    uint16_t m_elementsCount;
    uint16_t m_dataSize;

    ParameterRegistry* m_registry = nullptr;
    bool m_isVirtual = false;
};

#endif // PARAMETER_H
