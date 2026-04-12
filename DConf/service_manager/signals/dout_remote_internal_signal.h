#ifndef DOUTREMOTEINTERNALSIGNAL_H
#define DOUTREMOTEINTERNALSIGNAL_H

#include "service_manager/signals/dout_signal.h"

class DoutRemoteInternalSignal : public DoutSignal
{
public:
    DoutRemoteInternalSignal(const Config &config);
    virtual ~DoutRemoteInternalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::RemoteInternal; }
};

#endif // DOUTREMOTEINTERNALSIGNAL_H
