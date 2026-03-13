#ifndef SIGNALS_TAB_H
#define SIGNALS_TAB_H

#include <QTabWidget>

#include "gui/forms/main/abstract_device_tab.h"
#include "device_operations/information_operation.h"

class SignalsTab : public AbstractDeviceTab
{
    Q_OBJECT

public:
    SignalsTab(QWidget *parent = nullptr);
    ~SignalsTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    QWidget *createSignalsTabs(const InformationOperation::ParamsContainer &container, uint16_t addr);

private:
    QTabWidget *m_tabWidget;
};

#endif // SIGNALS_TAB_H
