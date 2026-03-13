#pragma once

#include <QWidget>

#include "device_operations/abstract_operation.h"

class AbstractDeviceTab : public QWidget
{
	Q_OBJECT

public:
    AbstractDeviceTab( QWidget *parent = nullptr);
    virtual ~AbstractDeviceTab();

    virtual void deviceOperationFinished(AbstractOperation *op) = 0;
    virtual QList<uint16_t> addrList() const = 0;
    virtual bool hasError() const { return false; };

signals:
    void operationRequest(AbstractOperation::SPtr operation);

};
