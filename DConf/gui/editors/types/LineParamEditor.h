#pragma once

#include "ParamEditor.h"

namespace Dpc::Gui
{
	class LineEdit;
	class IntSpinBox;
	class UIntSpinBox;
	class DoubleSpinBox;
}

class LineParamEditor : public ParamEditor
{
	Q_OBJECT
public:
	LineParamEditor(QWidget *parent = nullptr);
	LineParamEditor(int type, QWidget *parent = nullptr);
    LineParamEditor(ParameterElement *param, QWidget *parent = nullptr);

	virtual void setReadOnly(bool readOnly) override;
	virtual void setValue(const QString &value) override;

	void setMinimum(double min);
	void setMaximum(double max);
	void setRange(double bottom, double top);
	void setDecimals(int decimals);	
	void setLength(int len);
	void setSpecialValueText(const QString &text);
	void addValueText(double value, const QString &text);

private slots:	
	void onValueChanged(int value);
	void onValueChanged(uint value);
	void onValueChanged(double value);
	void onValueChanged(const QString &text);

private:
	void init(int type);

	Dpc::Gui::IntSpinBox* makeIntSpinBox(int type);
	Dpc::Gui::UIntSpinBox* makeUIntSpinBox();
	Dpc::Gui::DoubleSpinBox* makeDoubleSpinBox();
	Dpc::Gui::LineEdit* makeLineEdit(int type);

private:	
	Dpc::Gui::IntSpinBox *m_intSpinBox;
	Dpc::Gui::UIntSpinBox *m_uintSpinBox;
	Dpc::Gui::DoubleSpinBox *m_doubleSpinBox;
	Dpc::Gui::LineEdit *m_lineEdit;
};
