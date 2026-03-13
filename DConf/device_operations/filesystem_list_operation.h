#ifndef FILESYSTEMLISTOPERATION_H
#define FILESYSTEMLISTOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemListOperation : public AbstractOperation
{
public:
    explicit FilesystemListOperation(const QString &dir, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Read | Flag::Filesystem; }
    QString dir() const { return m_dir; }
    const QStringList& items() const { return m_items; }

protected:
    virtual bool exec() override;

private:
    QString m_dir;
    QStringList m_items;
};

#endif // FILESYSTEMLISTOPERATION_H
