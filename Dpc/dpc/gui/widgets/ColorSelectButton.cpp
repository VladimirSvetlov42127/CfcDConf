#include "ColorSelectButton.h"

#include <qdebug.h>
#include <qcolordialog.h>

#include <dpc/gui/gui.h>

namespace Dpc::Gui
{
	ColorSelectButton::ColorSelectButton(const QColor& color, QWidget* parent) :
		QPushButton(parent)
	{
		setColor(color);
		connect(this, &QPushButton::clicked, this, &ColorSelectButton::onClicked);
	}

	void ColorSelectButton::setColor(const QColor& color)
	{
		if (color == m_currentColor)
			return;

		m_currentColor = color;
		QString styleSheet;
		if (color.isValid())
			styleSheet = QString("QPushButton { background-color: %1; }").arg(color.name());
		setStyleSheet(styleSheet);
		emit colorChanged(color);
	}

	void ColorSelectButton::onClicked()
	{
		QColorDialog dlg;
		if (dlg.exec() == QDialog::Rejected)
			return;

		setColor(dlg.selectedColor());
	}
} // namespace