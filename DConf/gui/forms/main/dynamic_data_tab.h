#ifndef DYNAMIC_DATA_TAB_H
#define DYNAMIC_DATA_TAB_H

#include "gui/forms/main/abstract_device_tab.h"
#include "gui/forms/main/base_tab_model.h"

class DynamicDataTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    DynamicDataTab(QWidget *parent = nullptr);
    ~DynamicDataTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    BaseTabModel *m_dynamicDataModel;
};

#endif // DYNAMIC_DATA_TAB_H
