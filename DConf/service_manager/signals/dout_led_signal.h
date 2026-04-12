#ifndef DOUTLEDSIGNAL_H
#define DOUTLEDSIGNAL_H

#include "service_manager/signals/dout_signal.h"

class DoutLedSignal : public DoutSignal
{
public:
    DoutLedSignal(const Config &config);
    virtual ~DoutLedSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Led; }
};

#endif // DOUTLEDSIGNAL_H
