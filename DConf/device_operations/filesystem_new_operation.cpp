#include "filesystem_new_operation.h"

using namespace Dpc::Sybus;

FilesystemNewOperation::FilesystemNewOperation(const QString &dirPath, QObject *parent)
    : AbstractOperation{"Создание папки", 0, parent}
    , m_dirPath{dirPath}
{
}

bool FilesystemNewOperation::exec()
{
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    if (!channel()->mkdirectory(dirPath())) {
        addError(QString("Не удалось создать директорию %1 на устройстве: %2")
                 .arg(dirPath(), channel()->errorMsg()));
        return false;
    }

    return true;
}
