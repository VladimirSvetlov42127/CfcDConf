#ifndef DOUTREMOTESIGNAL_H
#define DOUTREMOTESIGNAL_H

#include "service_manager/signals/dout_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class DoutRemoteSignal : public DoutSignal, public ISignalCustomName
{
public:
    DoutRemoteSignal(const Config &config);
    virtual ~DoutRemoteSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Remote; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // DOUTREMOTESIGNAL_H
