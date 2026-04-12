#include "cin_virtual_signal.h"

#include <dpc/sybus/smparlist.h>

CinVirtualSignal::CinVirtualSignal(const Config &config)
    : CinSignal{config}
    , ISignalCustomName{SP_VIRTCIN_NAME, config.subtypeID}
{

}

QString CinVirtualSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Виртуальный счётчик");
}

bool CinVirtualSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return CinSignal::init(config);
}
