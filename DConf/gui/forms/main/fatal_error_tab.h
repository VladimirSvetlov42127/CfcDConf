#ifndef FATAL_ERROR_TAB_H
#define FATAL_ERROR_TAB_H

#include "device_operations/information_operation.h"
#include "gui/forms/main/abstract_device_tab.h"

class QTabWidget;

class FatalErrorTab : public AbstractDeviceTab
{
    Q_OBJECT
public:
    FatalErrorTab(QWidget *parent = nullptr);
    ~FatalErrorTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;
    virtual bool hasError() const override;

private:
    QWidget *createTab(const InformationOperation::ParamsContainer &container, uint16_t profile);

private:
    QTabWidget *m_tabWidget;
    QList<bool> m_hasError;
};

#endif // FATAL_ERROR_TAB_H
