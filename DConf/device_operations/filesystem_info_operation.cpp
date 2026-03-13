#include "filesystem_info_operation.h"

#include <QDebug>

#include <dpc/sybus/utils.h>

using namespace Dpc::Sybus;

FilesystemInfoOperation::FilesystemInfoOperation(QObject *parent)
    : AbstractOperation{"Информация о файловой системе", 0, parent}
{
}

bool FilesystemInfoOperation::exec()
{
    m_list.clear();

    uint16_t driveSizeAddr = SP_VOLUME_INFO;
    auto dim = channel()->dimension(driveSizeAddr);
    if (!dim) {
        addError(QString("Не удалось получить размерность параметра %1: %2")
                 .arg(toHex(driveSizeAddr), channel()->errorMsg()));
        return false;
    }

    auto dimSize = dim.subProfileCount();
    auto namesPack = channel()->names(driveSizeAddr);
    if (Channel::NoError != channel()->errorType()) {
        addError(QString("Не удалось получить имена параметра %1: %2")
                 .arg(toHex(driveSizeAddr)).arg(channel()->errorMsg()));
        return false;
    }

    auto valuesPack = channel()->param(driveSizeAddr, 0, dimSize);
    if (Channel::NoError != channel()->errorType()) {
        addError(QString("Не удалось получить занчения параметра %1: %2")
                 .arg(toHex(driveSizeAddr), channel()->errorMsg()));
        return false;
    }

    auto defaultName = namesPack->value<QString>(0);
    for(int i = 0; i < dimSize; ++i) {
        DriveInfo info;
        info.text = namesPack->count() > 1 ? namesPack->value<QString>(i + 1) : defaultName;
        info.value = valuesPack->value<float>(i);
        m_list.append(info);
    }

    return true;
}
