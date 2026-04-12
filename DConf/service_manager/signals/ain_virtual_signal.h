#ifndef AINVIRTUALSIGNAL_H
#define AINVIRTUALSIGNAL_H

#include "service_manager/signals/ain_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class AinVirtualSignal : public AinSignal, public ISignalCustomName
{
public:
    AinVirtualSignal(const Config &config);
    virtual ~AinVirtualSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Virtual; }
    QString name() const override;
    bool init(DcController* config) override;
};

#endif // AINVIRTUALSIGNAL_H
