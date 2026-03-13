#include "Journal.h"

#include <QDebug>
#include <QTimer>

namespace Dpc
{
    Journal::Journal(QObject *parent)
        : QObject(parent)
    {
        qRegisterMetaType<Dpc::Journal::RecordList>();

        auto timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, [=]() {
            if (m_recordList.isEmpty())
                return;

            emit newRecords(m_recordList);
            m_recordList.clear();
        });

        timer->start(75);
    }

    Journal::~Journal()
    {
    }

    void Journal::addInfoMessage(const QString& msg, const ISource* source)
	{
		m_recordList << Record{ source, Record::Information, QDateTime::currentDateTime(), msg };
	}

    void Journal::addDebugMessage(const QString& msg, uint8_t level, const ISource* source)
	{
        m_recordList << Record{ source, Record::Debug, QDateTime::currentDateTime(),
                        QString("%1%2").arg(QString(level, '\t'), msg) };
	}

    void Journal::addWarningMessage(const QString& msg, const ISource* source)
	{
		m_recordList << Record{ source, Record::Warning, QDateTime::currentDateTime(), msg };
	}

    void Journal::addErrorMessage(const QString& msg, const ISource* source)
	{
		m_recordList << Record{ source, Record::Error, QDateTime::currentDateTime(), msg };
	}

    void Journal::addSource(const ISource* source)
	{
		emit sourceAdded(source);
	}

    void Journal::removeSource(const ISource* source)
	{
		emit sourceRemoved(source);
	}

    void Journal::select(const ISource* source)
	{
		emit sourceSelected(source);
	}
} // namespace
