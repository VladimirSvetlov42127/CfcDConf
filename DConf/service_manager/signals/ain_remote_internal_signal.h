#ifndef AINREMOTEINTERNALSIGNAL_H
#define AINREMOTEINTERNALSIGNAL_H

#include "service_manager/signals/ain_signal.h"

class AinRemoteInternalSignal : public AinSignal
{
public:
    AinRemoteInternalSignal(const Config &config);
    virtual ~AinRemoteInternalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::RemoteInternal; }
};

#endif // AINREMOTEINTERNALSIGNAL_H
