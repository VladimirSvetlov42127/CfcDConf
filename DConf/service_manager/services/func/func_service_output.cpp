#include "func_service_output.h"

#include "service_manager/services/func/func_service.h"
#include "data_model/parameters/parameter_element.h"

namespace {
    const uint16_t g_NoSignal = 0xFF;
} // namespace

FuncServiceOutput::FuncServiceOutput(VFunc *vfunc)
    : ServiceOutput{ 0, 0, vfunc->argElement(), vfunc->service() }
    , m_vfunc{vfunc}
{
    m_vfunc->setOutput(this);
}

FuncServiceOutput::~FuncServiceOutput()
{
    m_vfunc->setOutput(nullptr);
}

QString FuncServiceOutput::name() const
{
    return m_vfunc->text();
}

void FuncServiceOutput::updateBinding(VirtualInputSignal *new_target)
{
    auto newValue = new_target ? new_target->subTypeID() : g_NoSignal;
    bindElement()->updateValue(QString::number(newValue));
}
