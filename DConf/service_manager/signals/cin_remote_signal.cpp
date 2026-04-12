#include "cin_remote_signal.h"

#include <dpc/sybus/smparlist.h>

CinRemoteSignal::CinRemoteSignal(const Config &config)
    : CinSignal{config}
    , ISignalCustomName{SP_CIN_CHANNAME, config.subtypeID}
{

}

QString CinRemoteSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Внешний счётчик");
}

bool CinRemoteSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return CinSignal::init(config);
}
