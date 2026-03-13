#include "CheckableHeaderView.h"

#include <qstyle.h>
#include <qpainter.h>
#include <qevent.h>
#include <qdebug.h>

namespace {
	//const QString gDefaultStyleSheet = "QHeaderView::section {\
	//		background-color: #7AB1C9;\
	//		padding-left: 4px;\
	//		border: 1px solid #999999;\
	//		height: 25px}";
	const QString gDefaultStyleSheet = "QHeaderView::section {\
			background-color: #547586;\
			color: #EDF1F2;\
			padding-left: 4px;\
			border: 1px solid #999999;\
			height: 25px}";
}

namespace Dpc::Gui
{
	CheckableHeaderView::CheckableHeaderView(Qt::Orientation orientation, QWidget* parent) :
		QHeaderView(orientation, parent),
		m_enabledChecking(true)
	{
		setStyleSheet(defaultStyleSheet());
	}

	void CheckableHeaderView::setEnabledChecking(bool enabled)
	{
		m_enabledChecking = enabled;
	}

	QString CheckableHeaderView::defaultStyleSheet()
	{
		return gDefaultStyleSheet;
	}

	void CheckableHeaderView::onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight)
	{
		auto column = topLeft.column();
		auto it = m_sections.find(column);
		if (it == m_sections.end())
			return;

		bool checked = topLeft.data(Qt::CheckStateRole).toBool();
		if (it.value() && !checked) {
			it.value() = false;
			this->viewport()->update();
		}
		else if (!it.value() && checked) {
			auto res = isCheckable(column);

			if (res.second) {
				it.value() = true;
				this->viewport()->update();	}
		}
	}

	void CheckableHeaderView::onModelReset()
	{
		m_sections.clear();
		for (int column = 0; column < model()->columnCount(); column++)
			if (auto res = isCheckable(column); res.first)
				m_sections[column] = res.second;

		this->viewport()->update();
	}

	void CheckableHeaderView::onRowsInserted(const QModelIndex& parent, int first, int last)
	{
		for (int column = 0; column < model()->columnCount(); column++) {
			auto res = isCheckable(column, first, last);

			if (!res.first)
				continue;

			auto findIt = m_sections.find(column);
			if (findIt == m_sections.end()) {
				m_sections[column] = res.second;
				continue;
			}

			findIt.value() = findIt.value() && res.second;
		}	

		this->viewport()->update();
	}

	void CheckableHeaderView::onRowsRemoved(const QModelIndex& parent, int first, int last)
	{
		auto it = m_sections.begin();
		while (it != m_sections.end()) {
			auto res = isCheckable(it.key());
			if (res.first) {
				it.value() = res.second;
				it++;
			} 
			else {
				it = m_sections.erase(it);
			}
		}

		this->viewport()->update();
	}

	void CheckableHeaderView::onColumnsInserted(const QModelIndex& parent, int first, int last)
	{
		int offset = last - first + 1;
		for (int column = model()->columnCount() - 1; column >= first; column--) {
			auto findIt = m_sections.find(column);
			if (findIt == m_sections.end())
				continue;

			m_sections[findIt.key() + offset] = findIt.value();
			m_sections.erase(findIt);
		}

		for (int column = first; column <= last; column++)
			if (auto res = isCheckable(column); res.first)
				m_sections[column] = res.second;

		this->viewport()->update();
	}

	void CheckableHeaderView::onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last)
	{
		m_beforeRemoveColumnsCount = model()->columnCount();

		for (int i = first; i <= last; i++) {
			m_sections.remove(i);
		}
	}

	void CheckableHeaderView::onColumnsRemoved(const QModelIndex& parent, int first, int last)
	{		
		int offset = last - first + 1;
		for (int i = last + 1; i < m_beforeRemoveColumnsCount; i++) {
			auto findIt = m_sections.find(i);
			if (findIt == m_sections.end())
				continue;

			m_sections[findIt.key() - offset] = findIt.value();
			m_sections.erase(findIt);
		}

		this->viewport()->update();
	}	

	void CheckableHeaderView::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
	{
		painter->save();
		QHeaderView::paintSection(painter, rect, logicalIndex);
		painter->restore();

		if (!m_enabledChecking)
			return;

		if (orientation() != Qt::Horizontal)
			return;

		auto it = m_sections.find(logicalIndex);
		if (it == m_sections.end())
			return;

		int offset = 3; // (height() - style()->pixelMetric(QStyle::PM_IndicatorHeight)) / 2; 
		int pos = sectionViewportPosition(logicalIndex);

		QStyleOptionButton option;
		option.rect = QRect(offset + pos, offset, style()->pixelMetric(QStyle::PM_IndicatorWidth), style()->pixelMetric(QStyle::PM_IndicatorHeight));

		if (it.value())
			option.state = QStyle::State_On;
		else
			option.state = QStyle::State_Off;

		option.state |= QStyle::State_Enabled;
		style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
	}

	void CheckableHeaderView::mousePressEvent(QMouseEvent* event)
	{
		QHeaderView::mousePressEvent(event);
		if (!m_enabledChecking)
			return;

		if (orientation() != Qt::Horizontal)
			return;

		int offset = 3; //(height() - style()->pixelMetric(QStyle::PM_IndicatorHeight)) / 2;		
		for (int column_index = 0; column_index < model()->columnCount(); column_index++) {
			if (isSectionHidden(column_index)) continue;
			int pos = sectionViewportPosition(column_index);
			QRect rect(offset + pos, offset, style()->pixelMetric(QStyle::PM_IndicatorWidth), style()->pixelMetric(QStyle::PM_IndicatorHeight));

			if (!rect.contains(event->pos()))
				continue;

			auto it = m_sections.find(column_index);
			if (it == m_sections.end())
				return;

			emit itemsCheckStateAboutToChange(column_index);
			it.value() = !it.value();
			disconnect(this->model(), &QAbstractItemModel::dataChanged, this, &CheckableHeaderView::onDataChanged);
			for (size_t row = 0; row < model()->rowCount(); row++) {
				QModelIndex index = model()->index(row, column_index);
				if (model()->flags(index) & Qt::ItemIsUserCheckable)
					model()->setData(index, it.value() ? Qt::Checked : Qt::Unchecked);
			}
			connect(this->model(), &QAbstractItemModel::dataChanged, this, &CheckableHeaderView::onDataChanged);
			emit itemsCheckStateChanged(column_index);

			this->viewport()->update();
			emit sectionToogled(column_index, it.value());
			break;
		}
	}

	void CheckableHeaderView::setModel(QAbstractItemModel* model)
	{
		if (this->model()) {
			disconnect(this->model(), &QAbstractItemModel::modelReset, this, &CheckableHeaderView::onModelReset);
			disconnect(this->model(), &QAbstractItemModel::rowsInserted, this, &CheckableHeaderView::onRowsInserted);
			disconnect(this->model(), &QAbstractItemModel::rowsRemoved, this, &CheckableHeaderView::onRowsRemoved);
			disconnect(this->model(), &QAbstractItemModel::columnsInserted, this, &CheckableHeaderView::onColumnsInserted);
			disconnect(this->model(), &QAbstractItemModel::columnsAboutToBeRemoved, this, &CheckableHeaderView::onColumnsAboutToBeRemoved);
			disconnect(this->model(), &QAbstractItemModel::columnsRemoved, this, &CheckableHeaderView::onColumnsRemoved);
			disconnect(this->model(), &QAbstractItemModel::dataChanged, this, &CheckableHeaderView::onDataChanged);
		}

		QHeaderView::setModel(model);
		if (!model)
			return;

		m_sections.clear();
		for (size_t column = 0; column < model->columnCount(); column++)			
			if (auto res = isCheckable(column); res.first)
				m_sections[column] = res.second;

		connect(model, &QAbstractItemModel::modelReset, this, &CheckableHeaderView::onModelReset);
		connect(model, &QAbstractItemModel::rowsInserted, this, &CheckableHeaderView::onRowsInserted);
		connect(model, &QAbstractItemModel::rowsRemoved, this, &CheckableHeaderView::onRowsRemoved);
		connect(model, &QAbstractItemModel::columnsInserted, this, &CheckableHeaderView::onColumnsInserted);
		connect(model, &QAbstractItemModel::columnsAboutToBeRemoved, this, &CheckableHeaderView::onColumnsAboutToBeRemoved);
		connect(model, &QAbstractItemModel::columnsRemoved, this, &CheckableHeaderView::onColumnsRemoved);
		connect(model, &QAbstractItemModel::dataChanged, this, &CheckableHeaderView::onDataChanged);
	}

	std::pair<bool, bool> CheckableHeaderView::isCheckable(int column, int firstRow, int lastRow) const
	{
		if (lastRow < 0)
			lastRow = model()->rowCount() - 1;

		bool isCheckable = false;
		bool isAllChecked = true;
		for (int row = firstRow; row <= lastRow; row++) {
			auto index = model()->index(row, column);
			if (index.flags() & Qt::ItemIsUserCheckable) {
				isCheckable = true;
				isAllChecked = index.data(Qt::CheckStateRole).toBool();
				if (!isAllChecked)
					break;
			}
		}

		return std::pair(isCheckable, isAllChecked);
	}
} // namespace