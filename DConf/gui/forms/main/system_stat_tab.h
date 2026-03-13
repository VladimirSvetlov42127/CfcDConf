#ifndef SYSTEM_STAT_TAB_H
#define SYSTEM_STAT_TAB_H

#include <QTabWidget>

#include "device_operations/information_operation.h"
#include "gui/forms/main/abstract_device_tab.h"

class SystemStatTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    SystemStatTab(QWidget *parent = nullptr);
    ~SystemStatTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    QWidget *createTab(const InformationOperation::ParamsContainer &container, uint16_t profile);

private:
    QTabWidget *m_tabWidget;
};

#endif // SYSTEM_STAT_TAB_H
