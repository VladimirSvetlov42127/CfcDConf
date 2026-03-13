#pragma once

#include <qheaderview.h>
#include <qhash.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui 
{
	class DPC_EXPORT CheckableHeaderView : public QHeaderView
	{
		Q_OBJECT
	public:
		CheckableHeaderView(Qt::Orientation orientation, QWidget* parent = 0);
		~CheckableHeaderView() {}

		void setEnabledChecking(bool enabled);

		static QString defaultStyleSheet();

	signals:
		void sectionToogled(int section, bool checked);
		void itemsCheckStateAboutToChange(int column);
		void itemsCheckStateChanged(int column);

	private slots:
		void onDataChanged(const QModelIndex& topLeft, const QModelIndex& bottomRight);
		void onModelReset();
		void onRowsInserted(const QModelIndex& parent, int first, int last);
		void onRowsRemoved(const QModelIndex& parent, int first, int last);

		void onColumnsInserted(const QModelIndex& parent, int first, int last);
		void onColumnsAboutToBeRemoved(const QModelIndex& parent, int first, int last);
		void onColumnsRemoved(const QModelIndex& parent, int first, int last);

	protected:
		virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const override;
		virtual void mousePressEvent(QMouseEvent* event) override;
		virtual void setModel(QAbstractItemModel* model) override;

	private:
		// Первое значение - Есть ли в столбце column флажки(помечаемые элементы), начиная со строки firstRow до lastRow включительно.
		// Второе значение - Установленны ли они все в true.
		// Если lastRow равен -1, то проверяются строки с firstRow до концы модели.
		std::pair<bool , bool> isCheckable(int column, int firstRow = 0, int lastRow = -1) const;

	private:
		bool m_enabledChecking;
		int m_beforeRemoveColumnsCount;

		// Контейнер столбцов, у которых должна быть галка. Ключ - номер столбца, Значение - отмечена ли галка.
		QHash<int, bool> m_sections;
	};
} // namespace
