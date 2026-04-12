#include "ain_virtual_signal.h"

#include <dpc/sybus/smparlist.h>

AinVirtualSignal::AinVirtualSignal(const Config &config)
    : AinSignal{config}
    , ISignalCustomName{SP_VIRTAIN_NAME, config.subtypeID}
{
}

QString AinVirtualSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Виртуальный аналог");
}

bool AinVirtualSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return AinSignal::init(config);
}
