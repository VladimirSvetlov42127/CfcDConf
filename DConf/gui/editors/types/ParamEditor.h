#pragma once

#include <qwidget.h>

#include "data_model/parameters/parameter_element.h"

class ParamEditor : public QWidget
{
	Q_OBJECT
public:
	ParamEditor(QWidget *parent = nullptr);
    ParamEditor(ParameterElement *param, QWidget *parent = nullptr);
	virtual ~ParamEditor();
	
    ParameterElement* param() const;
	virtual void setReadOnly(bool readOnly);
	virtual void setValue(const QString &value) = 0;

signals:
	void valueChanged(const QString &newValue, const QString &oldValue);

protected:
	void setParamValue(const QString &value);

private:
    ParameterElement *m_param;
	QString m_value;
};
