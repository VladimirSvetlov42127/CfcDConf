#ifndef CINREMOTESIGNAL_H
#define CINREMOTESIGNAL_H

#include "service_manager/signals/cin_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class CinRemoteSignal : public CinSignal, public ISignalCustomName
{
public:
    CinRemoteSignal(const Config &config);
    virtual ~CinRemoteSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Remote; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // CINREMOTESIGNAL_H
