#ifndef DINREMOTESIGNAL_H
#define DINREMOTESIGNAL_H

#include "service_manager/signals/din_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class DinRemoteSignal : public DinSignal, public ISignalCustomName
{
public:
    DinRemoteSignal(const Config &config);
    virtual ~DinRemoteSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Remote; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // DINREMOTESIGNAL_H
