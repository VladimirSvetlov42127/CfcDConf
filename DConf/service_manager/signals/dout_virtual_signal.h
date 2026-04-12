#ifndef DOUTVIRTUALSIGNAL_H
#define DOUTVIRTUALSIGNAL_H

#include "service_manager/signals/dout_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

class VFunc;

class DoutVirtualSignal : public DoutSignal, public ISignalCustomName
{
public:
    DoutVirtualSignal(const Config &config);
    virtual ~DoutVirtualSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Virtual; }

    VFunc* func() const;
    void setFunc(VFunc *func);

    QString name() const override;
    bool init(DcController* config) override;

private:
    VFunc* m_func = nullptr;
};

#endif // DOUTVIRTUALSIGNAL_H
