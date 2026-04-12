#ifndef CINVIRTUALSIGNAL_H
#define CINVIRTUALSIGNAL_H

#include "service_manager/signals/cin_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class CinVirtualSignal : public CinSignal, public ISignalCustomName
{
public:
    CinVirtualSignal(const Config &config);
    virtual ~CinVirtualSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Virtual; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // CINVIRTUALSIGNAL_H
