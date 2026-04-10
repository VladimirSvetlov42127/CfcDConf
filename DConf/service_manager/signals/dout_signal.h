#ifndef DOUTSIGNAL_H
#define DOUTSIGNAL_H

#include <memory>

#include "service_manager/signals/signal.h"
#include "service_manager/signals/target_element.h"

// DoutSignal - Дискретный выход. Может иметь только одну привязку источник - Дискретный вход!!!
// При установлении/сбросе привязок редактирует таблицу matrix_signals

class DoutSignal : public Signal, public TargetElement
{
public:
    using UPtr = std::unique_ptr<DoutSignal>;

    DoutSignal(const Config &config);
    virtual ~DoutSignal() = default;

    Signal::Type type() const override { return Signal::Type::Dout; }
    Signal::Subtype subtype() const override { return Signal::Subtype::Undef; }
    QString name() const override;
    bool init(DcController *config) override;

    std::optional<uint32_t> impulse() const;
    void setImpulse(uint32_t value);

protected:
    void onSourceChanged(DinSignal *newSource, DinSignal *prevSource) override;

private:
    ParameterElement* m_impulse = nullptr;
};

#endif // DOUTSIGNAL_H
