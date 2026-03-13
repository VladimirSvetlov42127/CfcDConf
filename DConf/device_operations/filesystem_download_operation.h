#ifndef FILESYSTEMDOWNLOADOPERATION_H
#define FILESYSTEMDOWNLOADOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemDownloadOperation : public AbstractOperation
{
public:
    struct Item {
        QString localPath;
        QString devicePath;
    };

    explicit FilesystemDownloadOperation(const QList<Item>& files, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Read | Flag::Filesystem; }
    const QList<Item>& files() const { return m_files; }

protected slots:
    virtual void onChannelProgress(int state) override;

protected:
    virtual bool exec() override;

private:
    QList<Item> m_files;
    int m_currentFile;
};

#endif // FILESYSTEMDOWNLOADOPERATION_H
