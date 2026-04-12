#ifndef CINREMOTEINTERNALSIGNAL_H
#define CINREMOTEINTERNALSIGNAL_H

#include "service_manager/signals/cin_signal.h"

class CinRemoteInternalSignal : public CinSignal
{
public:
    CinRemoteInternalSignal(const Config &config);
    virtual ~CinRemoteInternalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::RemoteInternal; }
};

#endif // CINREMOTEINTERNALSIGNAL_H
