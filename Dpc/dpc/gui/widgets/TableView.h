#pragma once

#include <qtableview.h>
#include <qabstractitemmodel.h>

#include <dpc/dpc_global.h>
#include <dpc/gui/widgets/CheckableHeaderView.h>

namespace Dpc::Gui 
{
	class DPC_EXPORT TableView : public QTableView
	{
		Q_OBJECT

	public:
		TableView(QWidget* parent = nullptr);
		TableView(QAbstractTableModel* model, QWidget* parent = nullptr);

		virtual void setModel(QAbstractItemModel* model) override;

		CheckableHeaderView* horizontalHeader() const;

	signals:
		void itemsCheckStateAboutToChange(int column);
		void itemsCheckStateChanged(int column);

	private:
		void init();

	private:
		CheckableHeaderView* m_headerView;
	};
} // namespace
