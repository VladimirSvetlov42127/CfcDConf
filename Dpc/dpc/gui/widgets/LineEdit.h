#pragma once

#include <qlineedit.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT LineEdit : public QLineEdit
	{
		Q_OBJECT
	public:
		enum DataType {
			StringType,
			MacAddressType,
			IpAddressType
		};

		LineEdit(QWidget* parent = nullptr);
		LineEdit(DataType type, QWidget* parent = nullptr);

		DataType type() const;
		void setMaxLength(int length);
		void setInputMask(const QString& inputMask);
		void setValidator(const QValidator* v);
		void setText(const QString& text);

		bool isValid() const;

	signals:
		void valueChanged(const QString& value);

	private slots:
		void onTextEdited(const QString& text);

	private:
		void init();
		void checkTextIsValid(const QString& text);

	private:
		DataType m_type;
		bool m_isValid;
		int m_length;
	};
} // namespace