#include "LineEdit.h"

#include <qdebug.h>
#include <qvalidator.h>

#include <dpc/gui/gui.h>

namespace {
	namespace StyleSheet {
		const QString Invalid = QString("QLineEdit { background-color: %1; }").arg(Dpc::Gui::color(Dpc::Gui::ErrorColor).name());
		const QString Valid = "";
	}

	namespace ToolTip {
		const QString InvalidValidator = "Введенное значение не соответствует валидатору";
		const QString InvalidInputMask = "Введенное значение не соответствует маске ввода";
		const QString InvalidLength = "Превышено максимально допустимое количество символов";
	}

	const QString ipRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	const QRegularExpression ipRegex("^" + ipRange
		+ "\\." + ipRange
		+ "\\." + ipRange
		+ "\\." + ipRange + "$");
}

namespace Dpc::Gui
{
	LineEdit::LineEdit(QWidget* parent) :
		QLineEdit(parent),
		m_type(StringType)
	{
		init();
	}

	LineEdit::LineEdit(LineEdit::DataType type, QWidget* parent) :
		QLineEdit(parent),
		m_type(type)
	{
		init();
	}

	LineEdit::DataType LineEdit::type() const
	{
		return m_type;
	}

	void LineEdit::setMaxLength(int length)
	{
		auto currentText = text();
		QLineEdit::setMaxLength(length);
		checkTextIsValid(currentText);
	}

	void LineEdit::setInputMask(const QString& inputMask)
	{
		auto currentText = text();
		QLineEdit::setInputMask(inputMask);
		checkTextIsValid(currentText);
	}

	void LineEdit::setValidator(const QValidator* v)
	{
		auto currentText = text();
		QLineEdit::setValidator(v);
		checkTextIsValid(currentText);
	}

	void LineEdit::setText(const QString& text)
	{
		QLineEdit::setText(text);
		checkTextIsValid(text);
	}

	bool LineEdit::isValid() const
	{
		return m_isValid;
	}

	void LineEdit::onTextEdited(const QString& text)
	{
		checkTextIsValid(text);
		if (m_isValid)
			emit valueChanged(text);
	}

	void LineEdit::init()
	{
		m_isValid = true;

		switch (type()) {
		case IpAddressType: setValidator(new QRegularExpressionValidator(ipRegex, this)); break;
		case MacAddressType: setInputMask("HH:HH:HH:HH:HH:HH;_"); break;
		default: break;
		}

		connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(onTextEdited(const QString&)));
	}

	void LineEdit::checkTextIsValid(const QString& text)
	{
		bool isValid = true;
		QString toolTip;
		if (validator()) {
			int p = 0;
			auto t = text;
			if (QValidator::Acceptable != validator()->validate(t, p)) {
				isValid = false;
				toolTip = ToolTip::InvalidValidator;
			}
		}

		if (!inputMask().isEmpty() && !hasAcceptableInput()) {
			isValid = false;
			toolTip = ToolTip::InvalidInputMask;
		}

		if (maxLength() < text.length()) {
			isValid = false;
			toolTip = ToolTip::InvalidLength;
		}

		m_isValid = isValid;
		setStyleSheet(m_isValid ? StyleSheet::Valid : StyleSheet::Invalid);
		setToolTip(toolTip);
	}
} // namespace