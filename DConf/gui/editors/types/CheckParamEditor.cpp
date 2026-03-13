#include "CheckParamEditor.h"

#include <qboxlayout.h>

CheckParamEditor::CheckParamEditor(ParameterElement *param, int bit, QWidget *parent) :
	ParamEditor(param, parent),
	m_box(new QCheckBox),
	m_bit(bit)
{
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_box = new QCheckBox;
	m_box->setTristate(false);

	uint value = param->value().toUInt();
	if (m_bit > -1)
		value &= (1 << m_bit);
	m_box->setChecked(value);

	connect(m_box, SIGNAL(stateChanged(int)), this, SLOT(onStateChanged(int)));

	layout->addWidget(m_box);
}

void CheckParamEditor::setValue(const QString & value)
{
	m_box->setChecked(value.toInt());
}

void CheckParamEditor::setText(const QString & text)
{
	m_box->setText(text);
}

bool CheckParamEditor::checked() const
{
	uint value = param()->value().toUInt();
	if (m_bit > -1)
		value &= (1 << m_bit);

	return value;
}

void CheckParamEditor::onStateChanged(int state)
{
	uint newValue = state ? 1 : 0;	
	if (m_bit > -1) {
		uint currentValue = param()->value().toUInt();
		if (state)
			newValue = currentValue | (1 << m_bit);
		else
			newValue = currentValue & ~(1 << m_bit);
	}

	setParamValue(QString::number(newValue));
}
