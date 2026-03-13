#include "PushButtonDelegate.h"

#include <qdebug.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <qevent.h>

namespace Dpc::Gui
{
	PushButtonDelegate::PushButtonDelegate(const QString& text, QObject* parent) :
		QStyledItemDelegate(parent),
		m_text(text)
	{
	}

	void PushButtonDelegate::setText(const QString& text)
	{
		m_text = text;
	}

	void PushButtonDelegate::setIcon(const QIcon& icon)
	{
		m_icon = icon;
	}

	void PushButtonDelegate::setIconSize(const QSize& size)
	{
		m_iconSize = size;
	}

	void PushButtonDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		if (!index.data(Qt::EditRole).toBool()) {
			QStyledItemDelegate::paint(painter, option, index);
			return;
		}

		QStyleOptionButton buttonOption;
		buttonOption.rect = option.rect.adjusted(+3, +3, -3, -3);		
		buttonOption.state |= index == m_pressedIndex ? QStyle::State_Sunken : QStyle::State_Raised;
		buttonOption.text = m_text;
		buttonOption.icon = m_icon;
		buttonOption.iconSize = m_iconSize;			
		QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
	}

	bool PushButtonDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
	{
		if (event->type() == QEvent::MouseButtonPress)
			m_pressedIndex = index;

		if (event->type() == QEvent::MouseButtonRelease && index == m_pressedIndex) {
			if (index.data(Qt::EditRole).isValid())
				emit clicked(index);
			m_pressedIndex = QModelIndex();
		}

		if (event->type() == QEvent::MouseMove) {
			if (m_pressedIndex.isValid() && index != m_pressedIndex) {
				m_pressedIndex = QModelIndex();
				emit sizeHintChanged(m_pressedIndex);	// force to repaint pressed item				
			}
		}

		return true;
	}
} // namespace
