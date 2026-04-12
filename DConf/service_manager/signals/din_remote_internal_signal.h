#ifndef DINREMOTEINTERNALSIGNAL_H
#define DINREMOTEINTERNALSIGNAL_H

#include "service_manager/signals/din_signal.h"

class DinRemoteInternalSignal : public DinSignal
{
public:
    DinRemoteInternalSignal(const Config &config);
    virtual ~DinRemoteInternalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::RemoteInternal; }
};

#endif // DINREMOTEINTERNALSIGNAL_H
