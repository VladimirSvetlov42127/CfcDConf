#include "ComboBox.h"

#include <qdebug.h>
#include <QWheelEvent>

#include <dpc/gui/gui.h>

namespace {
	namespace StyleSheet {
		const QString Invalid = QString("QComboBox { background-color: %1; }").arg(Dpc::Gui::color(Dpc::Gui::ErrorColor).name());
		const QString Valid = "";
	}
}

namespace Dpc::Gui
{
	ComboBox::ComboBox(QWidget* parent) :
		QComboBox(parent)
	{
        connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    }

    void ComboBox::setWheelEventEnabled(bool enabled)
    {
        m_wheelEventEnabled = enabled;
    }

    void ComboBox::wheelEvent(QWheelEvent *event)
    {
        if (!m_wheelEventEnabled) {
            event->accept();
            return;
        }

        QComboBox::wheelEvent(event);
    }

	void ComboBox::onCurrentIndexChanged(int index)
	{
		setStyleSheet(index < 0 ? StyleSheet::Invalid : StyleSheet::Valid);
	}
} // namespace
