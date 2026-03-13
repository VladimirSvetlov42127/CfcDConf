#include "LineParamEditor.h"

#include <cmath>
#include <qboxlayout.h>
#include <qdebug.h>

#include <dpc/gui/widgets/SpinBox.h>
#include <dpc/gui/widgets/LineEdit.h>

using namespace Dpc::Gui;

namespace {

} // namespcae

LineParamEditor::LineParamEditor(QWidget *parent) :
	ParamEditor(parent)
{
    init(T_STRING);
}

LineParamEditor::LineParamEditor(int type, QWidget *parent) :
	ParamEditor(parent)
{
	init(type);
}

LineParamEditor::LineParamEditor(ParameterElement *param, QWidget *parent) :
	ParamEditor(param, parent)
{
	init(param->type());
}

void LineParamEditor::setReadOnly(bool readOnly)
{
	if (m_lineEdit)
		m_lineEdit->setReadOnly(readOnly);

	if (m_intSpinBox)
		m_intSpinBox->setReadOnly(readOnly);

	if (m_uintSpinBox)
		m_uintSpinBox->setReadOnly(readOnly);

	if (m_doubleSpinBox)
		m_doubleSpinBox->setReadOnly(readOnly);
}

void LineParamEditor::setValue(const QString &value)
{
	if (m_lineEdit) {
		m_lineEdit->setText(value);
		onValueChanged(value);
	}

	if (m_intSpinBox)
		m_intSpinBox->setValue(value.toInt());

	if (m_uintSpinBox)
		m_uintSpinBox->setValue(value.toUInt());

	if (m_doubleSpinBox)
		m_doubleSpinBox->setValue(value.toDouble());
}

void LineParamEditor::setMinimum(double min)
{
	double m_min;

	auto t = param()->type();
	switch (t) {
    case T_BOOL:
    case T_BYTEBOOL:
    case T_BYTE:
    case T_8BIT:
    case T_WORD:
    case T_16BIT:
		m_min = std::max<int>(min, 0);
		break;
    case T_UNIT:
    case T_DWORD:
    case T_32BIT:
		m_min = std::trunc(std::max<double>(min, 0));
		break;		
    case T_CHAR:
    case T_SBYTE:
		m_min = std::max<int>(min, CHAR_MIN);
		break;
    case T_SHORT:
		m_min = std::max<int>(min, SHRT_MIN);
		break;
    case T_INTEGER:
    case T_LONG:
		m_min = std::max<int>(min, INT_MIN);
		break;
    case T_FLOAT:
		m_min = std::max<double>(min, std::numeric_limits<float>::lowest());
		break;
	default:
		break;
	}

	if (m_intSpinBox)
		m_intSpinBox->setMinimum(m_min);

	if (m_uintSpinBox)
		m_uintSpinBox->setMinimum(m_min);

	if (m_doubleSpinBox)
		m_doubleSpinBox->setMinimum(m_min);
}

void LineParamEditor::setMaximum(double max)
{
	double m_max;

	auto t = param()->type();
	switch (t) {
    case T_BOOL:
    case T_BYTEBOOL:
		m_max = std::min<int>(max, 1);
		break;
    case T_BYTE:
    case T_8BIT:
		m_max = std::min<int>(max, UCHAR_MAX);
		break;
    case T_WORD:
    case T_16BIT:
		m_max = std::min<int>(max, USHRT_MAX);
		break;
    case T_UNIT:
    case T_DWORD:
    case T_32BIT:
		m_max = std::trunc(std::min<double>(max, UINT_MAX));
		break;
    case T_CHAR:
    case T_SBYTE:
		m_max = std::min<int>(max, CHAR_MAX);
		break;
    case T_SHORT:
		m_max = std::min<int>(max, SHRT_MAX);
		break;
    case T_INTEGER:
    case T_LONG:
		m_max = std::min<int>(max, INT_MAX);
		break;
    case T_FLOAT:
		m_max = std::min<double>(max, std::numeric_limits<float>::max());
		break;
	default:
		break;
	}

	if (m_intSpinBox)
		m_intSpinBox->setMaximum(m_max);

	if (m_uintSpinBox)
		m_uintSpinBox->setMaximum(m_max);

	if (m_doubleSpinBox)
		m_doubleSpinBox->setMaximum(m_max);
}

void LineParamEditor::setRange(double bottom, double top)
{
	setMinimum(bottom);
	setMaximum(top);
}

void LineParamEditor::setDecimals(int decimals)
{
	if (!m_doubleSpinBox)
		return;

	m_doubleSpinBox->blockSignals(true);
	m_doubleSpinBox->setDecimals(decimals);
	m_doubleSpinBox->setValue(param()->value().toDouble());
	m_doubleSpinBox->blockSignals(false);
}

void LineParamEditor::setLength(int len)
{
	if (m_lineEdit)
		m_lineEdit->setMaxLength(len);
}

void LineParamEditor::setSpecialValueText(const QString &text)
{
	if (m_intSpinBox)
		m_intSpinBox->setSpecialValueText(text);

	if (m_uintSpinBox)
		m_uintSpinBox->setSpecialValueText(text);

	if (m_doubleSpinBox)
		m_doubleSpinBox->setSpecialValueText(text);
}

void LineParamEditor::addValueText(double value, const QString & text)
{
	if (m_intSpinBox)
		m_intSpinBox->addValueText(value, text);

	if (m_uintSpinBox)
		m_uintSpinBox->addValueText(value, text);

	if (m_doubleSpinBox)
		m_doubleSpinBox->addValueText(value, text);
}

void LineParamEditor::onValueChanged(int value)
{
	setParamValue(QString::number(value));
}

void LineParamEditor::onValueChanged(uint value)
{
	setParamValue(QString::number(value));
}

void LineParamEditor::onValueChanged(double value)
{
	setParamValue(QString::number(value, 'f', m_doubleSpinBox->decimals()));
}

void LineParamEditor::onValueChanged(const QString &text)
{
	setParamValue(text);
}

void LineParamEditor::init(int type)
{
	m_lineEdit			= nullptr;
	m_intSpinBox		= nullptr;
	m_uintSpinBox		= nullptr;
	m_doubleSpinBox		= nullptr;
	QWidget *editWidget = nullptr;
	switch (type)
	{
    case T_BYTE:
    case T_BYTEBOOL:
    case T_WORD:
    case T_BOOL:
    case T_SBYTE:
    case T_8BIT:
    case T_16BIT:
    case T_CHAR:
    case T_SHORT:
    case T_INTEGER:
    case T_LONG:
		editWidget = makeIntSpinBox(type);
		break;
    case T_UNIT:
    case T_DWORD:
    case T_32BIT:
		editWidget = makeUIntSpinBox();
		break;
    case T_FLOAT:
		editWidget = makeDoubleSpinBox();
		break;
    case T_DATETIME:
    case T_UTCTIME:
    case T_OUI:
    case T_IP4ADR:
    case T_STRING:
	default:
		editWidget = makeLineEdit(type);
		break;
	}

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(editWidget);
}

IntSpinBox * LineParamEditor::makeIntSpinBox(int type)
{	
	int m_min = INT_MIN;
	int m_max = INT_MAX;
	switch (type) {
    case T_BOOL:
    case T_BYTEBOOL:
		m_min = 0; m_max = 1; break;
    case T_BYTE:
    case T_8BIT:
		m_min = 0; m_max = UCHAR_MAX; break;
    case T_WORD:
    case T_16BIT:
		m_min = 0; m_max = USHRT_MAX; break;
    case T_CHAR:
    case T_SBYTE:
		m_min = CHAR_MIN; m_max = CHAR_MAX; break;
    case T_SHORT:
		m_min = SHRT_MIN; m_max = SHRT_MAX; break;
	default: break;
	}
	
	m_intSpinBox = new IntSpinBox;
	m_intSpinBox->setRange(m_min, m_max);
	if (param())
		m_intSpinBox->setValue(param()->value().toInt());

	connect(m_intSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
	return m_intSpinBox;
}

UIntSpinBox * LineParamEditor::makeUIntSpinBox()
{
	m_uintSpinBox = new UIntSpinBox;
	m_uintSpinBox->setRange(0, UINT_MAX);
	if (param())
		m_uintSpinBox->setValue(param()->value().toUInt());

	connect(m_uintSpinBox, SIGNAL(valueChanged(uint)), this, SLOT(onValueChanged(uint)));
	return m_uintSpinBox;
}

DoubleSpinBox * LineParamEditor::makeDoubleSpinBox()
{
	m_doubleSpinBox = new DoubleSpinBox;
	m_doubleSpinBox->setRange(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
	m_doubleSpinBox->setValue(param()->value().toDouble());

	connect(m_doubleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
	return m_doubleSpinBox;
}

LineEdit * LineParamEditor::makeLineEdit(int type)
{
	auto t = LineEdit::StringType;	
    if (type == T_IP4ADR)
		t = LineEdit::IpAddressType;
    if (type == T_OUI)
		t = LineEdit::MacAddressType;

	m_lineEdit = new LineEdit(t);
	if (param()) {
		m_lineEdit->setText(param()->value());
		if (t == LineEdit::StringType)
            m_lineEdit->setMaxLength(param()->dataSize());
	}

	connect(m_lineEdit, SIGNAL(valueChanged(const QString&)), this, SLOT(onValueChanged(const QString &)));
	return m_lineEdit;
}
