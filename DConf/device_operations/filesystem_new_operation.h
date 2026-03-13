#ifndef FILESYSTEMNEWOPERATION_H
#define FILESYSTEMNEWOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemNewOperation : public AbstractOperation
{
public:
    explicit FilesystemNewOperation(const QString &dirPath, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write | Flag::Filesystem; }
    QString dirPath() const { return m_dirPath; }

protected:
    virtual bool exec() override;

private:
    QString m_dirPath;
};

#endif // FILESYSTEMNEWOPERATION_H
