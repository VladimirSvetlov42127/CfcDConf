#ifndef FLX_STAT_TAB_H
#define FLX_STAT_TAB_H

#include "device_operations/abstract_operation.h"
#include "gui/forms/main/abstract_device_tab.h"
#include "device_operations/information_operation.h"

class QTabWidget;

class FlxLogicTab: public AbstractDeviceTab
{
    Q_OBJECT
public:
    FlxLogicTab(QWidget *parent = nullptr);
    ~FlxLogicTab() = default;

    virtual void deviceOperationFinished(AbstractOperation *op) override;
    virtual QList<uint16_t> addrList() const override;

private:
    QWidget *createTab(const InformationOperation::ParamsContainer &container, uint16_t profile);

private:
    QTabWidget *m_tabWidget;
};

#endif // FLX_STAT_TAB_H
