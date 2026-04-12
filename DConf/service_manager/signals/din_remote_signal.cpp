#include "din_remote_signal.h"

DinRemoteSignal::DinRemoteSignal(const Config &config)
    : DinSignal{config}
    , ISignalCustomName{SP_DIN_TYPE_NAME, config.subtypeID}
{

}

QString DinRemoteSignal::name() const
{
    if (auto userCustomName = customName(); !userCustomName.isEmpty())
        return userCustomName;

    return QString("Внешний вход");
}

bool DinRemoteSignal::init(DcController *config)
{
    ISignalCustomName::init(config);
    return DinSignal::init(config);
}
