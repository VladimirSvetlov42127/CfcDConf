#pragma once

#include <QObject>
#include <QDateTime>

#include <dpc/dpc_global.h>

namespace Dpc
{	
    class DPC_EXPORT Journal : public QObject
	{
		Q_OBJECT
    public:
        class ISource;

        struct Record
        {
            enum Type {
                Unknown = 0,
                Information = 1,
                Warning = 2,
                Error = 4,
                Debug = 8
            };

            const ISource* source = nullptr;
            Type type;
            QDateTime dateTime;
            QString msg;
        };

        using RecordList = QList<Record>;

        Journal(QObject *parent = nullptr);
        ~Journal();

	signals:
        void newRecords(const Dpc::Journal::RecordList&);
        void sourceAdded(const Dpc::Journal::ISource*);
        void sourceRemoved(const Dpc::Journal::ISource*);
        void sourceSelected(const Dpc::Journal::ISource*);

	public slots:
        void addInfoMessage(const QString& msg, const Dpc::Journal::ISource* source = nullptr);
        void addDebugMessage(const QString& msg, uint8_t level = 0, const Dpc::Journal::ISource* source = nullptr);
        void addWarningMessage(const QString& msg, const Dpc::Journal::ISource* source = nullptr);
        void addErrorMessage(const QString& msg, const Dpc::Journal::ISource* source = nullptr);

        void addSource(const Dpc::Journal::ISource* source);
        void removeSource(const Dpc::Journal::ISource* source);
        void select(const Dpc::Journal::ISource* source);

    private:
        RecordList m_recordList;
	};
} // namespace
Q_DECLARE_METATYPE(Dpc::Journal::RecordList)
