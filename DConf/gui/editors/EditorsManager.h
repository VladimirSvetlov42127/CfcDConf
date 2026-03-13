#pragma once

#include <qboxlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>

#include <gui/editors/types/LineParamEditor.h>
#include <gui/editors/types/ListParamEditor.h>
#include <gui/editors/types/CheckParamEditor.h>

class DcController;
class ParameterElement;

class EditorsManager : public QObject
{
	Q_OBJECT
public:
	EditorsManager(DcController *controller, QGridLayout *layout = nullptr, QObject *parent = nullptr);
	
	DcController* controller() const;

	void setLayout(QGridLayout *layout);
	void setLabelColumn(int column);
	void setEditorColumn(int column);
	void setDescriptionColumn(int column);
	void setLabelStyleSheet(const QString &styleSheet);
	void setEditorAlignment(Qt::Alignment alignment = Qt::Alignment());

	QGridLayout *layout() const;
	int labelColumn() const;
	int editorColumn() const;
	int descriptionColumn() const;

	void addWidget(QWidget *w, int column = 0, int rowSpan = 1, int columnSpan = 1);
	void addWidget(QWidget *w, const QString &label, const QString &description = QString());
	void addWidget(QWidget *w, QLabel *label, QWidget *description = nullptr);

	void addLayout(QLayout *layout, const QString &label, const QString &description = QString());
	void addLayout(QLayout *layout, int column = 0, int rowSpan = 1, int columnSpan = 1);

	EditorsManager* addGroupBox(const QString &name, int column = 0, int rowSpan = 1, int columnSpan = 1);
	QLineEdit* addLineConst(const QString &label, const QVariant &value, const QString &description = QString());

    LineParamEditor* addLineEditor(ParameterElement *param, const QString &label, const QString &description = QString()) const;
    LineParamEditor* addLineEditor(ParameterElement *param, const QString &label, double min, const QString &description = QString()) const;
    LineParamEditor* addLineEditor(ParameterElement *param, const QString &label, double min, double max, int decimals = 3, const QString &description = QString()) const;
    LineParamEditor* addLineEditor(int32_t addr, int32_t idx, const QString &label, const QString &description = QString()) const;
	LineParamEditor* addLineEditor(int32_t addr, int32_t idx, const QString &label, double min, const QString &description = QString()) const;
	LineParamEditor* addLineEditor(int32_t addr, int32_t idx, const QString &label, double min, double max, int decimals = 3, const QString &description = QString()) const;

	//ListParamEditor* addListEditor(const QString &label, const ListEditorContainer &items) const;
    //ListParamEditor* addListEditor(ParameterElement *param, const QString &label, const ListEditorContainer &items) const;
	//ListParamEditor* addListEditor(int32_t addr, int32_t idx, const QString &label, const ListEditorContainer &items) const;
	//ListParamEditor* addListEditor(int32_t addr, int32_t idx, int32_t board, const QString &label, const ListEditorContainer &items) const;

	ListParamEditor* addListEditor(const QString& label, const ListEditorContainer& items, bool active = true) const;
    ListParamEditor* addListEditor(ParameterElement* param, const QString& label, const ListEditorContainer& items, bool active = true) const;
    ListParamEditor* addListEditor(int32_t addr, int32_t idx, const QString& label, const ListEditorContainer& items, bool active = true) const;

    CheckParamEditor* addCheckEditor(ParameterElement *param, const QString &label, int bit = -1) const;
    CheckParamEditor* addCheckEditor(int32_t addr, int32_t idx, const QString &label, int bit = -1) const;

	void addWidgetCreatorLayout(ListParamEditor *editor, const ListParamEditor::CreateWidgetFuncType &creator, QLayout *layout = nullptr);
	void addWidgetCreatorLayout(ListParamEditor *editor, const ListParamEditor::CreateWidgetFuncType &creator, QLayout *layout, int column = 0, int rowSpan = 1, int columnSpan = 1);

	void addVerticalStretch(int stretch = 1);
	void addHorizontalStretch(int stretch = 1);
	void addStretch(int stretch = 1);

private:
	void addToLayout(const QString &label, QWidget *editor, const QString &description = QString()) const;

private:
	DcController *m_controller;
	QGridLayout *m_layout;
	int m_labelColumn = 0;
	int m_editorColumn = 1;
	int m_descriptionColumn = 2;
	QString m_labelStyleSheet;
	Qt::Alignment m_editorAlignment;
};
