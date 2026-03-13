#pragma once

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QSet>

#include <dpc/journal/Journal.h>
#include <dpc/journal/Source.h>

namespace Dpc::Gui 
{
	class JournalTableModel : public QAbstractTableModel
	{
	public:
		enum Columns {
			DateTimeColumn,
			DeviceColumn,
			TypeColumn,
			MsgColumn
		};

		JournalTableModel(QObject* parent = nullptr);

		int rowCount(const QModelIndex& parent = QModelIndex()) const;
		int columnCount(const QModelIndex& parent = QModelIndex()) const;
		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

        void clearSources(const QList<Dpc::Journal::ISource::IndexType>& sourceList);

		static QString nullSourceText() { return "Общие"; }
		static QIcon infoIcon();
		static QIcon warningIcon();
		static QIcon errorIcon();
		static QIcon debugIcon();

	public slots:
        void onNewRecords(const Dpc::Journal::RecordList& list);

	private:
        Dpc::Journal::RecordList m_recordList;
	};

	//=============================================================================
	class JournalFilterModel : public QSortFilterProxyModel
	{
	public:
		JournalFilterModel(QObject* parent = nullptr);

		void setTypeFilterMask(uint8_t mask);
		void setSourceFilterList(const QList<Journal::ISource::IndexType>& list);

	protected:
		bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

	private:
		uint8_t m_typeMsk = 0;
		QSet<Journal::ISource::IndexType> m_sourceSet;
	};
} // namespace
