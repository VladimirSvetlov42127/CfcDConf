#include "filesystem_remove_operation.h"

using namespace Dpc::Sybus;

FilesystemRemoveOperation::FilesystemRemoveOperation(const QStringList &paths, QObject *parent)
    : AbstractOperation{"Удаление файлов/папок", 1, parent}
    , m_paths{paths}
{
}

bool FilesystemRemoveOperation::exec()
{
    setCurrentStep(0, paths().count());
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    for(int i = 0; i < paths().count(); ++i) {
        auto filePath = paths().at(i);
        if (!channel()->deleteFile(filePath)) {
            addError(QString("Не удалось удалить файл %1: %2")
                     .arg(filePath, channel()->errorMsg()));
            return false;
        }

        emitProgress(i + 1);
    }

    return true;
}
