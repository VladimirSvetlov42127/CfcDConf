#pragma once

#include <qstyleditemdelegate.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT PushButtonDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		PushButtonDelegate(const QString &text = QString(), QObject* parent = 0);

		void setText(const QString& text);
		void setIcon(const QIcon& icon);
		void setIconSize(const QSize& size);

		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

	signals:
		void clicked(const QModelIndex& index);

	private:
		QString m_text;
		QIcon m_icon;
		QSize m_iconSize;

		QModelIndex m_pressedIndex;
	};
} // namespace