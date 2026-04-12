#include "dout_virtual_signal.h"

#include <dpc/sybus/smparlist.h>

#include "service_manager/services/func/vfunc.h"

DoutVirtualSignal::DoutVirtualSignal(const Config &config)
    : DoutSignal{config}
    , ISignalCustomName{SP_VIRTDOUT_NAME, config.subtypeID}
    , m_func{ nullptr }
{

}

VFunc *DoutVirtualSignal::func() const
{
    return m_func;
}

void DoutVirtualSignal::setFunc(VFunc *func)
{
    m_func = func;
}

QString DoutVirtualSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    if (func())
        return func()->text();

    return QString("Виртуальный выход");
}

bool DoutVirtualSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return DoutSignal::init(config);
}
