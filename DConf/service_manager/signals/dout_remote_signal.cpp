#include "dout_remote_signal.h"

#include <dpc/sybus/smparlist.h>

DoutRemoteSignal::DoutRemoteSignal(const Config &config)
    : DoutSignal{config}
    , ISignalCustomName{SP_DOUT_NAME, config.subtypeID}
{
}

QString DoutRemoteSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Внешний выход");
}

bool DoutRemoteSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return DoutSignal::init(config);;
}
