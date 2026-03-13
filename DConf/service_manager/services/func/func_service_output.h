#ifndef __FUNC_SERVICE_OUTPUT_H__
#define __FUNC_SERVICE_OUTPUT_H__

#include "service_manager/services/service_output.h"

class VFunc;

class FuncServiceOutput : public ServiceOutput
{
public:
    FuncServiceOutput(VFunc* vfunc);
    virtual ~FuncServiceOutput();

    QString name() const override;

protected:
    void updateBinding(VirtualInputSignal* new_target) override;

private:
    VFunc* m_vfunc;
};

#endif // __FUNC_SERVICE_OUTPUT_H__
