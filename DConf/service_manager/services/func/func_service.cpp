#include "func_service.h"

#include "service_manager/services/func/func_service_output.h"
#include "data_model/dc_controller.h"

namespace {

} // namespace

FuncService::FuncService(DcController *config)
    : Service{"Сервис функций виртуальных выходов"}
    , m_config{config}
{
}

Service::Type FuncService::type() const
{
    return Service::FuncVOutType;
}

bool FuncService::init()
{
    for(auto vdin: config()->serviceManager()->vdins())
        m_vdins.emplace(vdin->subTypeID(), vdin);

    auto vfListParam = config()->paramsRegistry().parameter(SP_DOUT_FUNCLIST);
    if (!vfListParam)
        return true;

    auto softIdParam = config()->paramsRegistry().element(SP_SOFTWARE_ID, 0);
    if (!softIdParam)
        return false;

    // Заполнение сервиса типами поддерживаемых функций
    auto softId = softIdParam->value().toUInt();
    auto isInvalidLtOscill = softId == DEPRTU_LT_T || softId == DEPRTU_LTA_T || softId == DEPRTU_LT_R || softId == DEPRTU_LTA_R ||
            softId == DEPRTU_LT_F || softId == DEPRTU_LTA_F;

    for (uint8_t i = 0; i < vfListParam->elementsCount(); i++) {
        auto p = vfListParam->element(i);
        auto type = static_cast<VFunc::Type>(p->value().toUInt());
        if (type == VFunc::OSCILL_START && isInvalidLtOscill)
            continue;

        m_vfuncsInfo.emplace(i, VFuncInfo{i, type, VFunc::name(type)} );
    }

    //  Проверка наличия виртуальных функций, создание и привязка в виртуальным выходам.
    auto funcParam = config()->paramsRegistry().parameter(SP_DOUT_TYPE);
    if (!funcParam)
        return true;

    // Список виртуальных выходов.
    std::unordered_map<uint8_t, VirtualOutputSignal*> vdouts;
    for(auto dout: config()->serviceManager()->douts())
        if (auto vout = dynamic_cast<VirtualOutputSignal*>(dout); vout)
            vdouts[vout->subTypeID()] = vout;

    // Создание вирутальных функций.
    for (uint8_t i = 0; i < funcParam->profilesCount(); ++i) {
        auto  typeIdElement = funcParam->element(i, 0);
        auto  argElement = funcParam->element(i, 1);

        auto vdoutIt = vdouts.find(i);
        if (vdoutIt == vdouts.end())
            return false;

        auto vfunc = std::make_unique<VFunc>(vdoutIt->second, typeIdElement, argElement, this);
        m_vfuncList.emplace_back(std::move(vfunc));
    }

    rebind();
    return true;
}

void FuncService::clearBindings()
{
    std::optional<uint8_t> noUseTypeID;
    for(auto &[typeID, info]: supportedTypes())
        if (VFunc::NOTUSE == info.type) {
            noUseTypeID = typeID;
            break;
        }

    if (!noUseTypeID)
        return;

    for(size_t i = 0; i < size(); ++i)
        setType(i, noUseTypeID.value());
}

void FuncService::rebind()
{
    for(size_t i = 0; i < m_vfuncList.size(); ++i) {
        auto vfunc = m_vfuncList.at(i).get();
        setType(i, vfunc->typeId());
    }
}

size_t FuncService::size() const
{
    return funcList().size();
}

const std::map<uint8_t, FuncService::VFuncInfo> &FuncService::supportedTypes() const
{
    return m_vfuncsInfo;
}

const VFunc::UPtrVector &FuncService::funcList() const
{
    return m_vfuncList;
}

QString FuncService::name(uint8_t id) const
{
    return supportedTypes().at(id).name;
}

VFunc::Type FuncService::type(uint8_t id) const
{
    return supportedTypes().at(id).type;
}

void FuncService::setType(uint8_t idx, uint8_t typeId)
{
    if (idx >= m_vfuncList.size())
        return;

    auto vfunc = m_vfuncList.at(idx).get();
    auto oldHasVDinArg = hasVDinArg(vfunc->typeId());
    auto newHasVDinArg = hasVDinArg(typeId);

    // меняем тип функции
    vfunc->setType(typeId);

    // удаляем выход сервиса, если у старой функции агрумент VDin.
    if (oldHasVDinArg)
        removeOutput(vfunc);

    // добавляем выход сервиса, если у новой функции агрумент VDin.
    if (newHasVDinArg)
        makeOutput(vfunc);
}

void FuncService::setArgValue(uint8_t idx, uint8_t value)
{
    if (idx >= m_vfuncList.size())
        return;

    auto vfunc = m_vfuncList.at(idx).get();

    // Если для функции не создан выход сервиса, устанавливаем значение аргумента функции в value.
    auto func_output = vfunc->output();
    if (!func_output) {
        vfunc->setArgValue(value);
        return;
    }

    // Если для функции создан выход сервиса, устанавливаем этому выходу target в VDin с subTypeId = value.
    func_output->setTarget(vdin(value));
}

DcController *FuncService::config() const
{
    return m_config;
}

VirtualInputSignal *FuncService::vdin(uint16_t vdinSubTypeID) const
{
    if (auto vdinIt = m_vdins.find(vdinSubTypeID); vdinIt != m_vdins.end())
        return vdinIt->second;

    return nullptr;
}

void FuncService::makeOutput(VFunc *func)
{
    auto sio = std::make_unique<FuncServiceOutput>(func);

    // Ищем vdin у которого subTypeId равен значению аргумента функции и привязываем если он свободен (не установлен source).    
    if (auto vdin = this->vdin(func->argValue()); vdin && !vdin->source())
        sio->setTarget(vdin);

    Service::addOutput(std::move(sio));
}

void FuncService::removeOutput(VFunc *func)
{
    auto func_output = func->output();
    if (!func_output)
        return;

    // Освободить виртуальный вход не меняя аргумента функции
    auto currentArg = func->argValue();
    func_output->setTarget(nullptr);    // сбрасывает значение аргумента функции в 0xFF.
    func->setArgValue(currentArg);

    Service::deleteOutput(func_output);
}

bool FuncService::hasVDinArg(uint8_t typeId) const
{
    switch (type(typeId)) {
    case VFunc::VDIN_CONTROL:
    case VFunc::VDIN_EVENT:
    case VFunc::FIX_VDIN:
        return true;
        break;
    default:
        break;
    }

    return false;
}

