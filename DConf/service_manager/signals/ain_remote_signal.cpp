#include "ain_remote_signal.h"

#include <dpc/sybus/smparlist.h>

AinRemoteSignal::AinRemoteSignal(const Config &config)
    : AinSignal{config}
    , ISignalCustomName{SP_AIN_CHANNAME, config.subtypeID}
{
}

QString AinRemoteSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Внешний аналог");
}

bool AinRemoteSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return AinSignal::init(config);
}
