#include "ParamEditor.h"

ParamEditor::ParamEditor(QWidget *parent) :
	QWidget(parent),
	m_param(nullptr)
{
}

ParamEditor::ParamEditor(ParameterElement *param, QWidget *parent) :
	QWidget(parent),
	m_param(param)
{
}

ParamEditor::~ParamEditor()
{
}

ParameterElement *ParamEditor::param() const
{ 
	return m_param; 
}

void ParamEditor::setReadOnly(bool readOnly)
{
	setEnabled(!readOnly);
}

void ParamEditor::setParamValue(const QString & value)
{
	if (!m_param) {
		if (m_value != value) {
			auto prevValue = m_value;
			m_value = value;
			emit valueChanged(value, prevValue);			
		}
		return;
	}

	auto t = param()->type();
    if (t != T_STRING && value.trimmed().isEmpty())
		return;

	QString oldValue = param()->value();
	bool ok = false;

	switch (t)
	{
    case T_BYTE:
    case T_BYTEBOOL:
    case T_WORD:
    case T_BOOL:
    case T_CHAR:
    case T_8BIT:
    case T_16BIT:
    case T_SBYTE:
    case T_SHORT:
    case T_INTEGER:
    case T_LONG:
		if (oldValue.toInt(&ok) == value.toInt() && ok)
			return;
		break;
    case T_UNIT:
    case T_32BIT:
    case T_DWORD:
    case T_DATETIME:
		if (oldValue.toUInt(&ok) == value.toUInt() && ok)
			return;
		break;
    case T_FLOAT:
		if (oldValue.toFloat(&ok) == value.toFloat() && ok)
			return;
		break;
    case T_UTCTIME:
		if (oldValue.toLongLong(&ok) == value.toLongLong() && ok) return;
		break;
    case T_OUI:			// mac address
    case T_IP4ADR:
    case T_STRING:
	default:
		if (oldValue == value) return;
		break;
	}

	param()->updateValue(value);
	emit valueChanged(param()->value(), oldValue);
}
