#ifndef DOUTPHYSICALSIGNAL_H
#define DOUTPHYSICALSIGNAL_H

#include "service_manager/signals/dout_signal.h"

class DoutPhysicalSignal : public DoutSignal
{
public:
    DoutPhysicalSignal(const Config &config);
    virtual ~DoutPhysicalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Physical; }

    bool isCloningEnabled() const;
};

#endif // DOUTPHYSICALSIGNAL_H
