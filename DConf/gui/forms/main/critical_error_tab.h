#ifndef CRITICAL_ERROR_TAB_H
#define CRITICAL_ERROR_TAB_H

#include "gui/forms/main/abstract_device_tab.h"
#include "gui/forms/main/base_tab_model.h"

class CriticalErrorTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    CriticalErrorTab(QWidget *parent = nullptr);
    ~CriticalErrorTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    BaseTabModel *m_criticalErrorModel;
};

#endif // CRITICAL_ERROR_TAB_H
