#ifndef FILESYSTEMREMOVEOPERATION_H
#define FILESYSTEMREMOVEOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemRemoveOperation : public AbstractOperation
{
public:
    explicit FilesystemRemoveOperation(const QStringList &paths, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write | Flag::Filesystem; }
    const QStringList& paths() const { return m_paths; }

protected:
    virtual bool exec() override;

private:
    QStringList m_paths;
};

#endif // FILESYSTEMREMOVEOPERATION_H
