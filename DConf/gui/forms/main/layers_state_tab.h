#ifndef LAYERS_STATE_TAB_H
#define LAYERS_STATE_TAB_H

#include <dpc/gui/widgets/TableView.h>

#include "gui/forms/main/abstract_device_tab.h"
#include "gui/forms/main/base_tab_model.h"

class LayersStateTab : public AbstractDeviceTab
{
    Q_OBJECT
public:
    LayersStateTab(QWidget *parent = nullptr);
    ~LayersStateTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;
    virtual bool hasError() const override;

private:
    BaseTabModel *m_layersStateModel;
    Dpc::Gui::TableView *m_layersStateView;
};

#endif // LAYERS_STATE_TAB_H
