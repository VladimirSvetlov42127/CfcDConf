#pragma once

#include "ParamEditor.h"

#include <qcheckbox.h>

class CheckParamEditor : public ParamEditor
{
	Q_OBJECT
public:
    CheckParamEditor(ParameterElement *param, int bit = -1, QWidget *parent = nullptr);

	virtual void setValue(const QString &value) override;

	void setText(const QString &text);

	bool checked() const;

public slots:
	void onStateChanged(int state);

private:
	QCheckBox *m_box;
	int m_bit;
};
