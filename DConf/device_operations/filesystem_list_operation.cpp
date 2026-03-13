#include "filesystem_list_operation.h"

using namespace Dpc::Sybus;

FilesystemListOperation::FilesystemListOperation(const QString &dir, QObject *parent)
    : AbstractOperation{"Список файлов/папок", 0, parent}
    , m_dir{dir}
{
}

bool FilesystemListOperation::exec()
{
    FileSystemLoginScope fsls(channel());
    if (!fsls.isValid) {
        addError(QString("Не удалось авторизоваться в файловую систему на устройстве: %1").arg(channel()->errorMsg()));
        return false;
    }

    m_items = channel()->directoryFileList(dir());
    if (Dpc::Sybus::Channel::NoError != channel()->errorCode()) {
        addError(QString("Не удалось получить содержимое папки: %1").arg(dir()));
        return false;
    }

    return true;
}
