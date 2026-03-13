#include "IpAddressWidget.h"

#include <qlabel.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qboxlayout.h>
#include <qevent.h>

namespace {
	const QRegularExpression ipRangeRegex("^(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])$");
}

namespace Dpc::Gui
{
	IpAddressWidget::IpAddressWidget(QWidget* parent) :
		QWidget(parent),
		m_p1(new QLineEdit(this)),
		m_p2(new QLineEdit(this)),
		m_p3(new QLineEdit(this)),
		m_p4(new QLineEdit(this)),
		m_fromBackSpace(false)
	{
		init(m_p1);
		init(m_p2);
		init(m_p3);
		init(m_p4);

		QHBoxLayout* layout = new QHBoxLayout(this);
		layout->setContentsMargins(0, 0, 0, 0);
		layout->addWidget(m_p1);
		layout->addWidget(new QLabel("."));
		layout->addWidget(m_p2);
		layout->addWidget(new QLabel("."));
		layout->addWidget(m_p3);
		layout->addWidget(new QLabel("."));
		layout->addWidget(m_p4);
		layout->addStretch();
	}

	QString IpAddressWidget::ipAddreess() const
	{
		return QString("%1.%2.%3.%4").arg(m_p1->text()).arg(m_p2->text()).arg(m_p3->text()).arg(m_p4->text());
	}

	void IpAddressWidget::setIpAddress(const QString& ip)
	{
		QStringList parts = ip.split('.');
		m_p1->setText(parts.value(0));
		m_p2->setText(parts.value(1));
		m_p3->setText(parts.value(2));
		m_p4->setText(parts.value(3));
	}

	bool IpAddressWidget::eventFilter(QObject* obj, QEvent* event)
	{
		if (obj != m_p1 && obj != m_p2 && obj != m_p3 && obj != m_p4)
			return false;

		if (event->type() == QEvent::KeyPress) {
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if (keyEvent->key() == Qt::Key_Period && obj != m_p4) {
				focusNextChild();
				return true;
			}

			if (keyEvent->key() == Qt::Key_Backspace && obj != m_p1 && static_cast<QLineEdit*>(obj)->text().isEmpty()) {
				focusPreviousChild();
				m_fromBackSpace = true;
				return true;
			}
		}

		if (event->type() == QEvent::FocusIn && m_fromBackSpace) {
			auto edit = static_cast<QLineEdit*>(obj);
			m_fromBackSpace = false;
		}

		return false;
	}

	void IpAddressWidget::init(QLineEdit* edit)
	{
		edit->setValidator(new QRegularExpressionValidator(ipRangeRegex, this));
		edit->installEventFilter(this);
		edit->setMaxLength(3);
		edit->setMaximumWidth(30);
		edit->setAlignment(Qt::AlignCenter);
		connect(edit, &QLineEdit::textEdited, this, [=](const QString& text) {
			if (text.toUInt() > 25)
			focusNextChild();
		emit changed(ipAddreess());
			});
	}
} // namespace