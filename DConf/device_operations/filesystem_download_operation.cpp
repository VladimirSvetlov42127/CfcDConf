#include "filesystem_download_operation.h"

using namespace Dpc::Sybus;

FilesystemDownloadOperation::FilesystemDownloadOperation(const QList<Item> &files, QObject *parent)
    : AbstractOperation{"Загрузить файлы", 1, parent}
    , m_files{files}
{
}

void FilesystemDownloadOperation::onChannelProgress(int state)
{
    double totalProgressPerFile = 100.0 / files().count();
    emitProgress(totalProgressPerFile * m_currentFile + totalProgressPerFile * state / 100.0);
}

bool FilesystemDownloadOperation::exec()
{
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    m_currentFile = 0;
    while(m_currentFile < files().count()) {
        auto fileItem = files().at(m_currentFile);
        if (!channel()->downloadFile(fileItem.devicePath, fileItem.localPath)) {
            addError(QString("Не удалось скачать файл %1: %2")
                     .arg(fileItem.localPath, channel()->errorMsg()));
            return false;
        }

        ++m_currentFile;
    }

    return true;
}
