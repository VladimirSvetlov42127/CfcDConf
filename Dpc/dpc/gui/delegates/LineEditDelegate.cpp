#include "LineEditDelegate.h"

#include <qdebug.h>

#include <dpc/gui/widgets/LineEdit.h>

namespace Dpc::Gui
{
	LineEditDelegate::LineEditDelegate(QObject* parent) :
		QStyledItemDelegate(parent),
		m_maxLength(-1),
		m_validator(nullptr)
	{
	}

	LineEditDelegate::LineEditDelegate(const QString& inputMask, QObject* parent) :
		QStyledItemDelegate(parent),
		m_inputMask(inputMask),
		m_maxLength(-1),
		m_validator(nullptr)
	{
	}

	QWidget* LineEditDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		auto editor = new LineEdit(parent);
		if (-1 < m_maxLength)
			editor->setMaxLength(m_maxLength);
		editor->setInputMask(m_inputMask);
		editor->setValidator(m_validator);
		return editor;
	}

	void LineEditDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		auto value = index.model()->data(index, Qt::EditRole).toString();
		auto ed = static_cast<LineEdit*>(editor);
		ed->setText(value);
	}

	void LineEditDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		auto ed = static_cast<LineEdit*>(editor);
		model->setData(index, ed->text(), Qt::EditRole);
	}

	void LineEditDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		editor->setGeometry(option.rect);
	}

	void LineEditDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyledItemDelegate::paint(painter, option, index);
	}

	void LineEditDelegate::setMaxLength(int length)
	{
		m_maxLength = length;
	}

	void LineEditDelegate::setInputMask(const QString& inputMask)
	{
		m_inputMask = inputMask;
	}

	void LineEditDelegate::setValidator(QValidator* v)
	{
		m_validator = v;
	}

	void LineEditDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
	{
		QStyledItemDelegate::initStyleOption(option, index);
	}
} // namespace
