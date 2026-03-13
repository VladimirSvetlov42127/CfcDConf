#pragma once

#include <qstyleditemdelegate.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT LineEditDelegate : public QStyledItemDelegate
	{
	public:
		LineEditDelegate(QObject* parent = 0);
		LineEditDelegate(const QString& inputMask, QObject* parent = 0);

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
		virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

		void setMaxLength(int length);
		void setInputMask(const QString& inputMask);
		void setValidator(QValidator* v);

	protected:
		virtual void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;

	private:
		int m_maxLength;
		QString m_inputMask;
		QValidator* m_validator;
	};
} // namespace