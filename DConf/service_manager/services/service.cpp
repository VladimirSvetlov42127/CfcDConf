#include "service.h"

Service::Service(const QString &name)
    : m_name{ name }
{
}

Service::Type Service::type() const
{
    return Service::NoType;
}

QString Service::name() const
{
    return m_name;
}

void Service::setName(const QString &name)
{
    m_name = name;
}

const ServiceInputList &Service::inputs() const
{
    return m_inputs;
}

const ServiceOutputList &Service::outputs() const
{
    return m_outputs;
}

void Service::addInput(ServiceInputPtr input)
{
    m_inputs.emplace_back(std::move(input));
}

void Service::addOutput(ServiceOutputPtr output)
{
    m_outputs.emplace_back(std::move(output));
}

void Service::deleteInput(ServiceInput *input)
{
    auto newEnd = std::remove_if(m_inputs.begin(), m_inputs.end(), [=](const ServiceInputPtr &item) {
        return item.get() == input;
    });

    m_inputs.erase(newEnd, m_inputs.end());
}

void Service::deleteOutput(ServiceOutput *output)
{
    auto newEnd = std::remove_if(m_outputs.begin(), m_outputs.end(), [=](const ServiceOutputPtr &item) {
        return item.get() == output;
    });

    m_outputs.erase(newEnd, m_outputs.end());
}

void Service::print() const
{
    qDebug() << "=======================" << name();
        QString inTemp = QString("IN:  %1: %2");
        QString outTemp = QString("OUT: %1: %2");
        for(auto &in: inputs()) {
            auto inInfo = QString("%1 %2").arg(in->id()).arg(in->name());
            auto inSourceInfo = in->source() ? QString("<--------- (%1) %2").arg(in->source()->internalID()).arg(in->source()->name()) : QString();
            qDebug().noquote() << inTemp.arg(inInfo, inSourceInfo);
        }
        for(auto &out: outputs()) {
            auto outInfo = QString("%1 %2").arg(out->id()).arg(out->name());
            auto outTargetInfo = out->target() ? QString("---------> (%1) %2").arg(out->target()->internalID()).arg(out->target()->name()) : QString();
            qDebug().noquote() << outTemp.arg(outInfo, outTargetInfo);
        }
}
