#include "QxdProxyModel.h"

#include <QDebug>
#include <QIcon>
#include <QCollator>
#include <QDateTime>
#include <QApplication>

#include <QElapsedTimer>

#include <dpc/gui/widgets/qxd_tableview/QxdFilter.h>


#ifdef QXD_PROFILE
#define WATCH(text, condition) Watch __W(text, condition);
#else
#define WATCH(text, condition)
#endif

namespace {
    struct Watch
    {
        bool condition;
        QElapsedTimer time;
        QString text;
        Watch(const QString &t, bool c = true) : text(t), condition(c) { time.start(); }
        ~Watch() { if (condition) qDebug() << QString("%1: %2").arg(text).arg(time.elapsed() / 1000.0).toStdString().c_str(); }
    };
}

QxdProxyModel::QxdProxyModel(QObject *parent) :
    QSortFilterProxyModel(parent),
    m_collator(new QCollator()),
    m_filterIcon(":/filter.png")
{
    m_collator->setNumericMode(true);
    m_collator->setCaseSensitivity(this->sortCaseSensitivity());
}

QxdProxyModel::~QxdProxyModel()
{
    delete m_collator;
	for (auto it = m_filters.begin(); it != m_filters.end(); it++)
		qDeleteAll(it.value());
}

void QxdProxyModel::setSourceModel(QAbstractItemModel * sourceModel)
{
	QAbstractItemModel *currentSource = this->sourceModel();
	if (currentSource == sourceModel)
		return;

	if (currentSource)
        disconnect(currentSource, &QAbstractItemModel::modelAboutToBeReset, this, &QxdProxyModel::clearInternalData);
    if (sourceModel)
        connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &QxdProxyModel::clearInternalData);

    clearInternalData();
	QSortFilterProxyModel::setSourceModel(sourceModel);
}

QVariant QxdProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (Qt::Horizontal == orientation && role == Qt::DecorationRole) {
        QxdIFilter *filter = getFilter(section);
		if(filter && filter->isEnabled())
            return m_filterIcon;
	}

	return QSortFilterProxyModel::headerData(section, orientation, role);
}

void QxdProxyModel::sort(int column, Qt::SortOrder order)
{
    WATCH(QString("Sort column %1 time").arg(column), column >= 0);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    if (m_useSourceSort.contains(column))
        sourceModel()->sort(column, order);
    else
        QSortFilterProxyModel::sort(column, order);
    QApplication::restoreOverrideCursor();
    if (column > -1)
        sorted(column);
}

void QxdProxyModel::addFilter(int column, QxdIFilter * filter)
{
	for (auto it = m_filters.begin(); it != m_filters.end(); it++) {
		auto jt = it.value().find(column);
		if (jt != it.value().end()) {
			delete jt.value();
			it.value().erase(jt);
		}
	}

	if (!filter)
		return;
	m_filters[filter->type()].insert(column, filter);
}

QxdIFilter *QxdProxyModel::getFilter(int column) const
{
	for (auto it = m_filters.begin(); it != m_filters.end(); it++) {
		auto jt = it.value().find(column);
		if (jt != it.value().end())
			return jt.value();
	}

    return nullptr;
}

void QxdProxyModel::setSortCaseSensitivityX(Qt::CaseSensitivity cs)
{
    m_collator->setCaseSensitivity(cs);
}

void QxdProxyModel::resetFilter(int column)
{
    for (auto it = m_filters.begin(); it != m_filters.end(); it++)
        for (auto jt = it.value().begin(); jt != it.value().end(); jt++)
            if (column < 0 || jt.key() == column)
                jt.value()->setEnabled(false);
}

void QxdProxyModel::setUseSourceSort(const QSet<int> &columns)
{
    m_useSourceSort = columns;
}

void QxdProxyModel::setFilterIcon(const QIcon &icon)
{
    m_filterIcon = icon;
}

QIcon QxdProxyModel::filterIcon() const
{
    return m_filterIcon;
}

bool QxdProxyModel::acceptedByFilters(int source_row, const QModelIndex &source_parent, int ignoreColumn) const
{
    for(auto it = m_filters.begin(); it != m_filters.end(); it++)
        for (auto jt = it.value().begin(); jt != it.value().end(); jt++) {
            if (jt.key() == ignoreColumn)
                continue;

            QxdIFilter *filter = jt.value();
            if (filter->isEnabled()) {
                QModelIndex index = sourceModel()->index(source_row, jt.key(), source_parent);
                if (!filter->accept(index, filterRole()))
                    return false;
            }
        }

    return true;
}

void QxdProxyModel::invalidateFilter()
{
    int count = rowCount();
//    sort(-1);
    emit filtersAboutToBeInvalidate();
    {
        WATCH("Filtering time", true);
        QApplication::setOverrideCursor(Qt::WaitCursor);
        invalidate();
        QApplication::restoreOverrideCursor();
    }
    emit filtersInvalidated();
    emit headerDataChanged(Qt::Horizontal, 0, columnCount());
    int newCount = rowCount();
    if (newCount != count)
        emit filteredCountChanged(newCount);
}

void QxdProxyModel::clearInternalData()
{
    sort(-1);
    resetFilter();
}

bool QxdProxyModel::lessThan(const QModelIndex & source_left, const QModelIndex & source_right) const
{
    QVariant l = source_left.data(sortRole());
    QVariant r = source_right.data(sortRole());

	switch (l.userType()) {
	case QVariant::Invalid:
		return (r.type() != QVariant::Invalid);
	case QVariant::Int:
		return l.toInt() < r.toInt();
	case QVariant::UInt:
		return l.toUInt() < r.toUInt();
	case QVariant::LongLong:
		return l.toLongLong() < r.toLongLong();
	case QVariant::ULongLong:
		return l.toULongLong() < r.toULongLong();
	case QMetaType::Float:
		return l.toFloat() < r.toFloat();
	case QVariant::Double:
		return l.toDouble() < r.toDouble();
	case QVariant::Char:
		return l.toChar() < r.toChar();
	case QVariant::Date:
		return l.toDate() < r.toDate();
	case QVariant::Time:
		return l.toTime() < r.toTime();
	case QVariant::DateTime:
        return l.toDateTime() < r.toDateTime();
    default:
        break;
    }

    // any other type compare like strings;
    return m_collator->compare(l.toString(), r.toString()) < 0;
}

bool QxdProxyModel::filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
{
    return acceptedByFilters(source_row, source_parent);
}
