#ifndef VIRTUALOUTPUTSIGNAL_H
#define VIRTUALOUTPUTSIGNAL_H

#include "service_manager/signals/output_signal.h"

class VFunc;

class VirtualOutputSignal : public OutputSignal
{
public:
    VirtualOutputSignal(int32_t id, int32_t internalid, uint8_t direction, uint8_t type, uint8_t subtype,const QString &name, ParameterElement* bindElement, uint16_t subtypeID);
    virtual ~VirtualOutputSignal() = default;

    VFunc* func() const;
    void setFunc(VFunc *func);

    QString name() const override;

private:
    VFunc* m_func;
};

#endif // VIRTUALOUTPUTSIGNAL_H
