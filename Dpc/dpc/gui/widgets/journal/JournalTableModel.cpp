#include "JournalTableModel.h"

#include <qicon.h>
#include <qapplication.h>
#include <qstyle.h>

#include <dpc/gui/gui.h>

using namespace Dpc;
using namespace Dpc::Gui;

namespace {
	using Dpc::Gui::JournalTableModel;

	const QMap<int, QString> gColumnNames = {
		{JournalTableModel::DateTimeColumn, "Время" },
		{JournalTableModel::DeviceColumn, "Устройство" },
		{JournalTableModel::TypeColumn, "Тип" },
		{JournalTableModel::MsgColumn, "Сообщение" }
	};

	QIcon typeToIcon(int type)
	{
		switch (type)
		{
        case Journal::Record::Information: return JournalTableModel::infoIcon();
        case Journal::Record::Warning: return JournalTableModel::warningIcon();
        case Journal::Record::Error: return JournalTableModel::errorIcon();
        case Journal::Record::Debug: return JournalTableModel::debugIcon();
		default: return QIcon();
		}
	}

	QColor typeToColor(int type)
	{
		switch (type)
		{
        case Journal::Record::Information: return color(Dpc::Gui::InformationColor);
        case Journal::Record::Warning: return color(Dpc::Gui::WarningColor);
        case Journal::Record::Error: return color(Dpc::Gui::ErrorColor);
        case Journal::Record::Debug: return color(Dpc::Gui::DebugColor);
		default: return QColor();
		}
	}

    Journal::ISource::IndexType recordId(const Journal::Record &rec) {
        return rec.source ? rec.source->id() : Journal::ISource::IndexType();
    }
}

namespace Dpc::Gui
{
    QIcon JournalTableModel::infoIcon() { return QIcon(":/icons/info.svg"); }
    QIcon JournalTableModel::warningIcon() { return QIcon(":/icons/warning.svg"); }
    QIcon JournalTableModel::errorIcon() { return QIcon(":/icons/error.svg"); }
    QIcon JournalTableModel::debugIcon() { return QIcon(":/icons/debug.svg"); }

	JournalTableModel::JournalTableModel(QObject* parent) :
		QAbstractTableModel(parent)
	{
	}

	int JournalTableModel::rowCount(const QModelIndex& parent) const
	{
		if (parent.isValid())
			return 0;

		return m_recordList.size();
	}

	int JournalTableModel::columnCount(const QModelIndex& parent) const
	{
		if (parent.isValid())
			return 0;

		return gColumnNames.size();
	}

	QVariant JournalTableModel::data(const QModelIndex& index, int role) const
	{
		if (!index.isValid())
			return QVariant();

		auto& record = m_recordList[index.row()];
		if (Qt::UserRole == role)
			switch (index.column())
			{
            case DeviceColumn: return recordId(record);
			case TypeColumn: return record.type;
			default: return QVariant();
			}

		if (Qt::DisplayRole == role)
			switch (index.column())
			{
			case DateTimeColumn: return record.dateTime.toString("hh:mm:ss:zzz");
			case DeviceColumn: return record.source ? record.source->name() : nullSourceText();
			case MsgColumn: return record.msg;
			default: return QVariant();
			}

		if (Qt::DecorationRole == role)
			switch (index.column())
			{
			case TypeColumn: return typeToIcon(record.type);
			default: return QVariant();
			}

		if (Qt::BackgroundRole == role)
			return typeToColor(record.type);

		if (Qt::TextAlignmentRole == role)
			switch (index.column())
			{
			case DateTimeColumn:
			case DeviceColumn:
			case TypeColumn: return Qt::AlignCenter;
			default: return QVariant();
			}

		if (Qt::ToolTipRole == role)
			switch (index.column())
			{
			case MsgColumn: return record.msg.trimmed();
			default: return QVariant();
			}

		return QVariant();
	}

	QVariant JournalTableModel::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
			return QVariant();

		return gColumnNames.value(section);
	}

    void JournalTableModel::clearSources(const QList<Dpc::Journal::ISource::IndexType> &sourceList)
	{
		if (sourceList.isEmpty())
			return;

        auto sourceSet = QSet<Journal::ISource::IndexType>(sourceList.begin(), sourceList.end());
		beginResetModel();
		auto it = m_recordList.begin();
		while (it != m_recordList.end()) {
            if (sourceSet.contains(recordId(*it)))
				it = m_recordList.erase(it);
			else
				it++;
		}
		endResetModel();
	}

    void JournalTableModel::onNewRecords(const Dpc::Journal::RecordList &list)
	{
		if (list.isEmpty())
			return;

		int size = m_recordList.size();
		beginInsertRows(QModelIndex(), size, size + list.size() - 1);
		m_recordList.append(list);
		endInsertRows();
	}

	//=============================================================================
	JournalFilterModel::JournalFilterModel(QObject* parent) :
		QSortFilterProxyModel(parent)
	{
	}

	void JournalFilterModel::setTypeFilterMask(uint8_t mask)
	{
		m_typeMsk = mask;
		invalidate();
	}

    void JournalFilterModel::setSourceFilterList(const QList<Journal::ISource::IndexType>& list)
	{
        m_sourceSet = QSet<Journal::ISource::IndexType>(list.begin(), list.end());;
		invalidate();
	}

	bool JournalFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
	{
		uint8_t type = sourceModel()->index(sourceRow, JournalTableModel::TypeColumn, sourceParent).data(Qt::UserRole).toUInt();
		if (!(m_typeMsk & type))
			return false;

		QVariant data = sourceModel()->index(sourceRow, JournalTableModel::DeviceColumn, sourceParent).data(Qt::UserRole);
        if (!m_sourceSet.contains(data.value<Journal::ISource::IndexType>()))
			return false;

		return true;
	}
} // namespace
