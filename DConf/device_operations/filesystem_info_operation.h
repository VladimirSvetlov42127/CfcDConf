#ifndef FILESYSTEMINFOOPERATION_H
#define FILESYSTEMINFOOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemInfoOperation : public AbstractOperation
{
public:
    struct DriveInfo {
        QString text;
        float value;
    };

    explicit FilesystemInfoOperation(QObject *parent = nullptr);

    Flags flags() const override { return Flag::Read | Flag::Filesystem; }
    const QList<DriveInfo>& drives() const { return m_list; }

protected:
    virtual bool exec() override;

private:
    QList<DriveInfo> m_list;
};

#endif // FILESYSTEMINFOOPERATION_H
