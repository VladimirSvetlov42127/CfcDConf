#ifndef DINSIGNAL_H
#define DINSIGNAL_H

#include <memory>
#include <optional>

#include <QList>

#include "service_manager/signals/signal.h"
#include "data_model/parameters/parameter_element.h"

// DinSignal - Дискретный вход. Может быть привязан ко многим элементам цель - Дискретным выходам!!! и Входам сервисов!!!.

class TargetElement;

class DinSignal : public Signal
{
public:
    using UPtr = std::unique_ptr<DinSignal>;

    DinSignal(const Config &config);
    virtual ~DinSignal() = default;

    Signal::Type type() const override { return Signal::Type::Din; }
    Signal::Subtype subtype() const override { return Signal::Subtype::Undef; }
    bool init(DcController* config) override;

    const QList<TargetElement*>& targets() const;
    void addTarget(TargetElement* target);
    void removeTarget(TargetElement* target);

    auto oscillFlag() const { return m_oscillBit.value(); }
    void setOscillFlag(bool flag) { m_oscillBit.setValue(flag); }

    auto trendFlag() const { return m_trendBit.value(); }
    void setTrendFlag(bool flag) { m_trendBit.setValue(flag); }

    auto archiveFlag() const { return m_archiveBit.value(); }
    void setArchiveFlag(bool flag)  { m_archiveBit.setValue(flag); }

    auto dpFlag() const { return m_dpBit.value(); }
    void setDPFlag(bool flag)  { m_dpBit.setValue(flag); }

    auto iec101SelectFlag() const { return m_iec101SelectFlag.value(); }
    void setIec101SelectFlag(bool flag)  { m_iec101SelectFlag.setValue(flag); }

    auto iec101Group1Flag() const { return m_iec101Group1Flag.value(); }
    void setIec101Group1Flag(bool flag)  { m_iec101Group1Flag.setValue(flag); }

    auto iec101Group2Flag() const { return m_iec101Group2Flag.value(); }
    void setIec101Group2Flag(bool flag)  { m_iec101Group2Flag.setValue(flag); }

    auto iec101BackgroundFlag() const { return m_iec101BackgroundFlag.value(); }
    void setIec101BackgroundFlag(bool flag)  { m_iec101BackgroundFlag.setValue(flag); }

    auto iec103SelectFlag() const { return m_iec103SelectFlag.value(); }
    void setIec103SelectFlag(bool flag)  { m_iec103SelectFlag.setValue(flag); }

    auto spodesSelectFlag() const { return m_spodesSelectFlag.value(); }
    void setSpodes101SelectFlag(bool flag)  { m_spodesSelectFlag.setValue(flag); }

private:
    QList<TargetElement*> m_targets;

    ElementBit m_oscillBit;
    ElementBit m_trendBit;
    ElementBit m_archiveBit;
    ElementBit m_dpBit;

    ElementBit m_iec101SelectFlag;
    ElementBit m_iec101Group1Flag;
    ElementBit m_iec101Group2Flag;
    ElementBit m_iec101BackgroundFlag;
    ElementBit m_iec103SelectFlag;
    ElementBit m_spodesSelectFlag;

};

#endif // DINSIGNAL_H
