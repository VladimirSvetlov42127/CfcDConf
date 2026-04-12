#ifndef AINREMOTESIGNAL_H
#define AINREMOTESIGNAL_H

#include "service_manager/signals/ain_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class AinRemoteSignal : public AinSignal, public ISignalCustomName
{
public:
    AinRemoteSignal(const Config &config);
    virtual ~AinRemoteSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Remote; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // AINREMOTESIGNAL_H
