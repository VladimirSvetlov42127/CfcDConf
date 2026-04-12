#ifndef AINACPSIGNAL_H
#define AINACPSIGNAL_H

#include "service_manager/signals/ain_signal.h"

class AinAcpSignal : public AinSignal
{
public:
    AinAcpSignal(const Config &config);
    virtual ~AinAcpSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Acp; }
};

#endif // AINACPSIGNAL_H
