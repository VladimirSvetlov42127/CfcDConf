#ifndef ABSTRACTCONFIGOPERATION_H
#define ABSTRACTCONFIGOPERATION_H

#include <device_operations/abstract_operation.h>

#include <data_model/dc_controller.h>

class AbstractConfigOperation : public AbstractOperation
{
    Q_OBJECT
public:
    AbstractConfigOperation(const QString &name, DcController *device, int stepsCount = 1, QObject *parent = nullptr);

    static QString configArchiveDevicePath();

protected:
    DcController* config() const;

    void setCheckSoftIdEnabled(bool enabled);
    bool checkSoftId();

private:
    DcController* m_config;
    bool m_isCheckSoftIdEnabled;
};

#endif // ABSTRACTCONFIGOPERATION_H
