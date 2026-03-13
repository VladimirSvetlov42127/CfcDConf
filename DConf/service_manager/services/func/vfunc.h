#ifndef VFUNC_H
#define VFUNC_H

#include <QString>

#include "service_manager/signals/virtual_output_signal.h"

class ParameterElement;
class FuncService;
class FuncServiceOutput;

// VFunc - Функция вирутального выхода
// Каждая вирутальная функция обладает типом type, и аргуметном функции arg.
// Список типов функций один единственный для всех устройств depRTU.
// Но в зависимости от устройства, у каждого типа функции есть свой идентификатор - typeId
// Установка и вычитывание типа функции делается по typeId.
// Изменения типа функции и аргумента функции делаются через класс FuncService, для контроля целостности сервиса.

class VFunc
{
    friend class FuncService;
    friend class FuncServiceOutput;
public:

    // Общий список всех типов функций виртуальных выходов.
    enum Type {
        NOTUSE = 0,				// "Не используется";
        TEST_CNTRL,				// "Пуск теста цепей управления";
        OSCILL_START,			// "Пуск осциллографирования";
        VDIN_CONTROL,			// "Управление виртуальным входом"; ARG = [VDIN]
        XCBR_CNTRL,				// "Управление выключателем"; ARG = [N]
        XCBR_RZA_CNTRL,			// "Управление выключателем c РЗА"; ARG = [N]
        QUIT_CMD,				// "Квитация событий";
        FIX_VDIN,				// "Фиксация входа"; ARG = [VDIN]
        VDOUT_CONFIRM,			// "Квитация события"; ARG = [VDIN]
        VDIN_EVENT,				// "Событие в виртуальном входе"; ARG = [VDIN]
        EXEC_EMBEDED_ALG,		// "Запуск встроенного алгоритма";
        NETWUSE,				// "Штатное управление";
        CHANGE_SIM,				// "Смена СИМ карты";
        BLOCK_TU,				// "Запретить ТУ";
        CONTROL_SV = 15,		// "Управление SV потоком", ARG = { Вкл, Откл }
        ACTIVE_GROUP,			// "Активная группа уставок" ARG = [N]

        UNKNOWN
    };

    using UPtr = std::unique_ptr<VFunc>;
    using UPtrVector = std::vector<VFunc::UPtr>;

    // vdout - Виртуальный дискретный выход, на который установлена функция.
    VFunc(VirtualOutputSignal *vdout, ParameterElement* typeElement, ParameterElement* argElement, FuncService *service);
    virtual ~VFunc();

    // Имя функции
    QString name() const;

    // Текстовое представление, включающее имя и информацию об аргументе функции
    QString text() const;

    // Тип установленой функции виртуального выхода
    VFunc::Type type() const;

    // Идентификатор типа установленой функции виртуального выхода
    uint8_t typeId() const;

    // Значение аргумента функции
    uint8_t argValue() const;

    // Имя функции типа type
    static QString name(VFunc::Type type);

private:
    // Установка типа функции вирутального выхода по идентификатору. Доступ через FuncService
    void setType(uint8_t typeId);

    // Установка значения аргумента функции виртуального выхода. Доступ через FuncService
    void setArgValue(uint8_t value);

    // Сервис к которому принадлежит функция
    FuncService* service() const;

    // Элемента параметра типа функции
    ParameterElement* typeElement() const;

    // Элемент аргумента функции
    ParameterElement* argElement() const;

    // Выход сервиса. Акутально только для функций у которых есть выход (аргумент vdin). Для остальных nullptr.
    void setOutput(FuncServiceOutput *output);
    FuncServiceOutput* output() const;

private:
    VirtualOutputSignal* m_vdout;
    ParameterElement* m_typeElement;
    ParameterElement* m_argElement;
    FuncService* m_service;
    FuncServiceOutput* m_output;
};

#endif // VFUNC_H
