#pragma once

#include <qwidget.h>

#include <dpc/dpc_global.h>

class QLineEdit;

namespace Dpc::Gui
{
	class DPC_EXPORT IpAddressWidget : public QWidget
	{
		Q_OBJECT

	public:
		IpAddressWidget(QWidget *parent = nullptr);

		QString ipAddreess() const;
		void setIpAddress(const QString &ip);

	signals:
		void changed(const QString &ip);

	protected:
		bool virtual eventFilter(QObject *obj, QEvent *event) override;

	private:
		void init(QLineEdit *edit);

	private:
		QLineEdit *m_p1;
		QLineEdit *m_p2;
		QLineEdit *m_p3;
		QLineEdit *m_p4;

		bool m_fromBackSpace;
	};
} // namespace
