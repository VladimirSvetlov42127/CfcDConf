#pragma once

#include <qpushbutton.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT ColorSelectButton : public QPushButton
	{
		Q_OBJECT
	public:
		ColorSelectButton(const QColor &color = QColor(), QWidget* parent = nullptr);

		QColor color() const { return m_currentColor; }		

	signals:
		void colorChanged(const QColor& color);

	public slots:
		void setColor(const QColor& color);

	private slots:
		void onClicked();

	private:
		QColor m_currentColor;
	};
} // namespace