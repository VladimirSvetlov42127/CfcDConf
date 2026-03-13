#include "parameter_registry.h"

#include <QDebug>

#include "data_model/dc_controller.h"

ParameterRegistry::ParameterRegistry()
    : m_device{nullptr}
{    
}

int32_t ParameterRegistry::uid() const
{
    return m_device ? m_device->uid() : 0;
}

void ParameterRegistry::setDevice(DcController *device)
{
    m_device = device;
}

uint32_t ParameterRegistry::size() const
{
    return m_params.size();
}

Parameter *ParameterRegistry::add(Parameter::UPtr param)
{
    if (!param)
        return nullptr;

    auto addedParam = m_params.insert_or_assign(param->addr(), std::move(param)).first->second.get();
    addedParam->setRegistry(this);
    addedParam->insert();

    return addedParam;
}

bool ParameterRegistry::remove(uint32_t addr)
{
    auto findIt = m_params.find(addr);
    if (findIt == m_params.end())
        return false;

    findIt->second->remove();
    m_params.erase(findIt);
    return true;
}

Parameter *ParameterRegistry::parameter(uint16_t addr) const
{
    auto findIt = m_params.find(addr);
    if (findIt == m_params.end())
        return nullptr;

    return findIt->second.get();
}

ParameterElement *ParameterRegistry::element(uint16_t addr, uint16_t position) const
{
    auto parameter = this->parameter(addr);
    if (!parameter)
        return nullptr;

    return parameter->element(position);
}

bool ParameterRegistry::isEqual(ParameterRegistry *other) const
{
    if (size() != other->size()) {
        qDebug() << QString("Failed Size: %1 != %2").arg(size()).arg(other->size());
        return false;
    }

    for(auto &[addr, parameter]: m_params) {
        auto o = other->parameter(addr);
        if (!o) {
            qDebug() << "Failed find" << QString("0x%1").arg(QString::number(addr, 16));
            return false;
        }

        if (!parameter->isEqual(o)) {
            qDebug() << "Failed compare:";
            qDebug() << parameter->toString();
            qDebug() << "-----------------";
            qDebug() << o->toString();
            return false;
        }
    }

    return true;
}
