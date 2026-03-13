#ifndef COMMON_DATA_TAB_H
#define COMMON_DATA_TAB_H

#include <dpc/gui/widgets/TableView.h>

#include "gui/forms/main/abstract_device_tab.h"
#include "gui/forms/main/common_data_model.h"

class CommonDataModel;

class CommonDataTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    CommonDataTab(DcController *controller, QWidget *parent = nullptr);    
    ~CommonDataTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    void updateData(const InformationOperation::ParamsContainer &container);

    CommonDataModel *m_commonDataModel;
    Dpc::Gui::TableView *m_commonDataView;
};

#endif // COMMON_DATA_TAB_H
