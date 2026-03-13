#include "filesystem_upload_operation.h"

using namespace Dpc::Sybus;

FilesystemUploadOperation::FilesystemUploadOperation(const QList<Item> &files, QObject *parent)
    : AbstractOperation{"Запись файлов", 1, parent}
    , m_files{files}
{
}

void FilesystemUploadOperation::onChannelProgress(int state)
{
    double totalProgressPerFile = 100.0 / files().count();
    emitProgress(totalProgressPerFile * m_currentFile + totalProgressPerFile * state / 100.0);
}

bool FilesystemUploadOperation::exec()
{
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    m_currentFile = 0;
    while(m_currentFile < files().count()) {
        auto fileItem = files().at(m_currentFile);
        if (!channel()->uploadFile(fileItem.localPath, fileItem.devicePath)) {
            addError(QString("Не удалось записать файл %1: %2")
                     .arg(fileItem.localPath, channel()->errorMsg()));
            return false;
        }

        ++m_currentFile;
    }

    return true;
}
