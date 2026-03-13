#include "filesystem_format_operation.h"

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

FilesystemFormatOperation::FilesystemFormatOperation(int drive, QObject * parent) :
    AbstractOperation("Форматирование тома", 0, parent),
	m_drive(drive)
{
}

void FilesystemFormatOperation::onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg)
{
    addError(channel()->errorMsg());
}

bool FilesystemFormatOperation::exec()
{
	if (!channel()->formatDrive(m_drive)) {
		addError(QString("Не удалось отформатировать том %1").arg(m_drive));
		return false;
	}

    return true;
}
