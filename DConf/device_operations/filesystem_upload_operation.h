#ifndef FILESYSTEMUPLOADOPERATION_H
#define FILESYSTEMUPLOADOPERATION_H

#include "device_operations/abstract_operation.h"

class FilesystemUploadOperation : public AbstractOperation
{
public:
    struct Item {
        QString localPath;
        QString devicePath;
    };

    explicit FilesystemUploadOperation(const QList<Item> &files, QObject *parent = nullptr);

    Flags flags() const override { return Flag::Write | Flag::Filesystem; }
    const QList<Item>& files() const { return m_files; }

protected slots:
    virtual void onChannelProgress(int state) override;

protected:
    virtual bool exec() override;

private:
    QList<Item> m_files;
    int m_currentFile;
};

#endif // FILESYSTEMUPLOADOPERATION_H
