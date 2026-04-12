#ifndef CINPHYSICALSIGNAL_H
#define CINPHYSICALSIGNAL_H

#include "service_manager/signals/cin_signal.h"

class CinPhysicalSignal : public CinSignal
{
public:
    CinPhysicalSignal(const Config &config);
    virtual ~CinPhysicalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Physical; }
};

#endif // CINPHYSICALSIGNAL_H
