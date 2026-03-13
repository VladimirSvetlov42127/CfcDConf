#include "TableView.h"

#include <qdebug.h>

namespace Dpc::Gui
{
	TableView::TableView(QWidget* parent) :	QTableView(parent)
	{
		init();
	}

	TableView::TableView(QAbstractTableModel* model, QWidget* parent) :	QTableView(parent)
	{
		init();
		setModel(model);
	}

	void TableView::setModel(QAbstractItemModel* model)
	{
		QTableView::setModel(model);
		horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	}

	CheckableHeaderView* TableView::horizontalHeader() const
	{
		return m_headerView;
	}

	void TableView::init()
	{
		verticalHeader()->hide();

		setSelectionBehavior(QAbstractItemView::SelectRows);
		setSelectionMode(QAbstractItemView::SingleSelection);
		setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);
		setWordWrap(false);

		m_headerView = new CheckableHeaderView(Qt::Horizontal, this);
		connect(m_headerView, &CheckableHeaderView::itemsCheckStateAboutToChange, this, &TableView::itemsCheckStateAboutToChange);
		connect(m_headerView, &CheckableHeaderView::itemsCheckStateChanged, this, &TableView::itemsCheckStateChanged);
		setHorizontalHeader(m_headerView);
	}
} // namespace
