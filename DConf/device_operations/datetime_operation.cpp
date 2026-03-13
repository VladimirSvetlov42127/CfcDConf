#include "datetime_operation.h"

#include <dpc/sybus/channel/Channel.h>

using namespace Dpc::Sybus;

DatetimeOperation::DatetimeOperation(const QDateTime& dateTime, const QVariant& offset, QObject * parent) :
    AbstractOperation("Установка даты и времени", 0, parent),
	m_dateTime(dateTime), 
	m_offset(offset)
{
}

void DatetimeOperation::onChannelError(Dpc::Sybus::Channel::ErrorType errorType, int errorCode, const QString &errorMsg)
{
    addError(channel()->errorMsg());
}

bool DatetimeOperation::exec()
{
	auto currentOffset = channel()->dateTimeOffset();
	if (Channel::NoError != channel()->errorCode())
		return false;

	if (currentOffset.isValid()) {
		m_dateTime = m_dateTime.addSecs(-1 * (m_offset.toInt() * 3600));

		if (m_offset.isValid())
			m_dateTime = m_dateTime.addSecs(currentOffset.toInt() * 3600);
	}

	if (!channel()->setDateTime(m_dateTime))
		return false;

	if (m_offset.isValid() && !channel()->setDateTimeOffset(m_offset.value<int8_t>()))
		return false;

	if (!saveConfig())
		return false;

	if (!restart())
		return false;

    return true;
}
