#ifndef AINPHYSICALSIGNAL_H
#define AINPHYSICALSIGNAL_H

#include "service_manager/signals/ain_signal.h"

class AinPhysicalSignal : public AinSignal
{
public:
    AinPhysicalSignal(const Config &config);
    virtual ~AinPhysicalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Physical; }
};

#endif // AINPHYSICALSIGNAL_H
