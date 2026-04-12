#ifndef DINVIRTUALSIGNAL_H
#define DINVIRTUALSIGNAL_H

#include "service_manager/signals/din_signal.h"
#include "service_manager/signals/signal_icustom_name.h"

// DinVirtualSignal - Виртуальный дискретный вход, расширение DinSignal(Дискретного входа), который может иметь привязку источник - Выход сервиса!!!

class ServiceOutput;

class DinVirtualSignal : public DinSignal, public ISignalCustomName
{
public:
    DinVirtualSignal(const Config &config);
    virtual ~DinVirtualSignal() = default;

    Signal::Subtype subtype() const override { return Signal::Subtype::Virtual; }    
    QString name() const override;
    QString text() const override;
    QString fullText() const override;
    bool init(DcController* config) override;

    QString baseText() const;

    void setSource(ServiceOutput* newSource);
    ServiceOutput* source() const;

    auto saveFlag() const { return m_saveBit.value(); }
    void setSaveFlag(bool flag) { m_saveBit.setValue(flag); }

    auto fix1Flag() const { return m_fix1Bit.value(); }
    void setFix1Flag(bool flag) { m_fix1Bit.setValue(flag); }

private:
    void onSourceChanged(const ServiceOutput *newSource, const ServiceOutput *prevSource) const;

private:
    ServiceOutput* m_source = nullptr;
    ElementBit m_saveBit;
    ElementBit m_fix1Bit;
};

#endif // DINVIRTUALSIGNAL_H
