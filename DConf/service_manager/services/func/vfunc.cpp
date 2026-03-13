#include "vfunc.h"

#include "data_model/parameters/parameter_element.h"
#include "service_manager/services/func/func_service.h"
#include "service_manager/services/func/func_service_output.h"

VFunc::VFunc(VirtualOutputSignal *vdout, ParameterElement *typeElement, ParameterElement *argElement, FuncService *service)
    : m_vdout{ vdout }
    , m_typeElement{ typeElement }
    , m_argElement{ argElement}
    , m_service{ service }
    , m_output{ nullptr }
{
    m_vdout->setFunc(this);
}

VFunc::~VFunc()
{
    m_vdout->setFunc(nullptr);
}

QString VFunc::name() const
{
    return name(type());
}

QString VFunc::text() const
{
    auto funcText = name();
    QString funcArgText = QString("<%1>");
    switch (type()) {
    case VFunc::NOTUSE:
    case VFunc::TEST_CNTRL:
    case VFunc::OSCILL_START:
    case VFunc::QUIT_CMD:
    case VFunc::EXEC_EMBEDED_ALG:
    case VFunc::NETWUSE:
    case VFunc::CHANGE_SIM:
    case VFunc::BLOCK_TU:
        break;
    case VFunc::VDIN_CONTROL:
    case VFunc::VDIN_EVENT:
    case VFunc::FIX_VDIN:
        funcText.append(funcArgText.arg(output() && output()->target() ?
                                                   QString::number(output()->target()->internalID()) : QString()));
        break;
    case VFunc::XCBR_CNTRL:
    case VFunc::ACTIVE_GROUP:
        funcText.append(funcArgText.arg(argValue() + 1));
        break;
    case VFunc::CONTROL_SV:
        funcText.append(funcArgText.arg(argValue() == 0 ? "Откл" : argValue() == 1 ? "Вкл" : QString()));
        break;
    default:
        funcText.append(funcArgText.arg(argValue()));
        break;
    }

    return funcText;
}

VFunc::Type VFunc::type() const
{
    return service()->type(typeId());
}

uint8_t VFunc::typeId() const
{
    return typeElement()->value().toUInt();
}

uint8_t VFunc::argValue() const
{
    return argElement()->value().toUInt();
}

QString VFunc::name(Type type)
{
    const static QHash<VFunc::Type, QString> texts = {
        {VFunc::NOTUSE, "Не используется"},
        {VFunc::TEST_CNTRL, "Пуск теста цепей управления"},
        {VFunc::OSCILL_START, "Пуск осциллографирования"},
        {VFunc::VDIN_CONTROL, "Управление входом"},
        {VFunc::XCBR_CNTRL, "Управление выключателем"},
        {VFunc::XCBR_RZA_CNTRL, "Управление выключателем c РЗА"},
        {VFunc::QUIT_CMD, "Квитация событий"},
        {VFunc::FIX_VDIN, "Фиксация входа"},
        {VFunc::VDOUT_CONFIRM, "Квитация события"},
        {VFunc::VDIN_EVENT, "Событие в входе"},
        {VFunc::EXEC_EMBEDED_ALG, "Запуск встроенного алгоритма"},
        {VFunc::NETWUSE,	"Штатное управление"},
        {VFunc::CHANGE_SIM, "Смена СИМ карты"},
        {VFunc::BLOCK_TU, "Запретить ТУ"},
        {VFunc::CONTROL_SV, "Управление SV потоком"},
        {VFunc::ACTIVE_GROUP, "Установка активной группы уставок"}
    };

    return texts.value(type, QString());
}

void VFunc::setType(uint8_t typeId)
{
    m_typeElement->updateValue(QString::number(typeId));
}

void VFunc::setArgValue(uint8_t value)
{
    m_argElement->updateValue(QString::number(value));
}

FuncService *VFunc::service() const
{
    return m_service;
}

ParameterElement *VFunc::typeElement() const
{
    return m_typeElement;
}

ParameterElement *VFunc::argElement() const
{
    return m_argElement;
}

void VFunc::setOutput(FuncServiceOutput *output)
{
    m_output = output;
}

FuncServiceOutput *VFunc::output() const
{
    return m_output;
}
