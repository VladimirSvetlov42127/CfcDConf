#ifndef DINLOGICALSIGNAL_H
#define DINLOGICALSIGNAL_H

#include "service_manager/signals/din_signal.h"

class DinLogicalSignal : public DinSignal
{
public:
    DinLogicalSignal(const Config &config);
    virtual ~DinLogicalSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Logical; }
};

#endif // DINLOGICALSIGNAL_H
