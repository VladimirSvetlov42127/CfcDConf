#ifndef SM_FLAGS_TAB_H
#define SM_FLAGS_TAB_H

#include <dpc/gui/widgets/TableView.h>

#include "gui/forms/main/abstract_device_tab.h"
#include "gui/forms/main/base_tab_model.h"

class SmFlagsTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    SmFlagsTab(QWidget *parent = nullptr);
    ~SmFlagsTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;
    virtual bool hasError() const override;

private:
    BaseTabModel *m_smFlagsModel;
    Dpc::Gui::TableView *m_smFlagsView;
};

#endif // SM_FLAGS_TAB_H
