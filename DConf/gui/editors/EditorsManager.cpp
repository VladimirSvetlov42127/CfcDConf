#include "EditorsManager.h"

#include <qdebug.h>
#include <qgroupbox.h>



#include <data_model/dc_controller.h>

EditorsManager::EditorsManager(DcController *controller, QGridLayout *layout, QObject *parent) :
	QObject(parent),
	m_controller(controller),
	m_layout(layout),
	m_editorAlignment(Qt::Alignment())
{
}

DcController * EditorsManager::controller() const
{
	return m_controller;
}

void EditorsManager::setLayout(QGridLayout * layout)
{
	m_layout = layout;
}

void EditorsManager::setLabelColumn(int column)
{
	m_labelColumn = column;
}

void EditorsManager::setEditorColumn(int column)
{
	m_editorColumn = column;
}

void EditorsManager::setDescriptionColumn(int column)
{
	m_descriptionColumn = column;
}

void EditorsManager::setLabelStyleSheet(const QString & styleSheet)
{
	m_labelStyleSheet = styleSheet;
}

void EditorsManager::setEditorAlignment(Qt::Alignment alignment)
{
	m_editorAlignment = alignment;
}

QGridLayout * EditorsManager::layout() const
{
	return m_layout;
}

int EditorsManager::labelColumn() const
{
	return m_labelColumn;
}

int EditorsManager::editorColumn() const
{
	return m_editorColumn;
}

int EditorsManager::descriptionColumn() const
{
	return m_descriptionColumn;
}

void EditorsManager::addWidget(QWidget * w, int column, int rowSpan, int columnSpan)
{
	m_layout->addWidget(w, m_layout->rowCount(), column, rowSpan, columnSpan);
}

void EditorsManager::addWidget(QWidget * w, const QString & label, const QString & description)
{
	addToLayout(label, w, description);
}

void EditorsManager::addWidget(QWidget * w, QLabel * label, QWidget * description)
{
	int row = m_layout->rowCount();
	label->setStyleSheet(m_labelStyleSheet);
	m_layout->addWidget(label, row, m_labelColumn);
	m_layout->addWidget(w, row, m_editorColumn, m_editorAlignment);
	if (description)
		m_layout->addWidget(description, row, m_descriptionColumn);
}

void EditorsManager::addLayout(QLayout * layout, const QString & label, const QString &description)
{
	int row = m_layout->rowCount();
	auto labelWidget = new QLabel(label);
	labelWidget->setStyleSheet(m_labelStyleSheet);
	m_layout->addWidget(labelWidget, row, m_labelColumn);
	m_layout->addLayout(layout, row, m_editorColumn, m_editorAlignment);
	if (!description.isEmpty())
		m_layout->addWidget(new QLabel(description), row, m_descriptionColumn);
}

void EditorsManager::addLayout(QLayout * layout, int column, int rowSpan, int columnSpan)
{
	m_layout->addLayout(layout, m_layout->rowCount(), column, rowSpan, columnSpan);
}

EditorsManager * EditorsManager::addGroupBox(const QString & name, int column, int rowSpan, int columnSpan)
{
	QGroupBox *group = new QGroupBox(name);
	group->setStyleSheet("QGroupBox { font-weight: bold; } ");
	QGridLayout *layout = new QGridLayout(group);
	addWidget(group, column, rowSpan, columnSpan);

	return new EditorsManager(controller(), layout, this);
}

QLineEdit * EditorsManager::addLineConst(const QString & label, const QVariant &value, const QString &description)
{	
	auto valueWidget = new QLineEdit(value.toString());
	valueWidget->setReadOnly(true);
	addToLayout(label, valueWidget, description);
	return valueWidget;
}

LineParamEditor * EditorsManager::addLineEditor(ParameterElement * param, const QString & label, const QString & description) const
{
	if (!param)
		return nullptr;

	auto editor = new LineParamEditor(param);
	editor->setDecimals(3);
	addToLayout(label, editor, description);
	return editor;
}

LineParamEditor * EditorsManager::addLineEditor(ParameterElement *param, const QString & label, double min, const QString & description) const
{
	auto editor = addLineEditor(param, label, description);
	if (editor)
		editor->setMinimum(min);
	return editor;
}

LineParamEditor * EditorsManager::addLineEditor(ParameterElement *param, const QString & label, double min, double max, int decimals, const QString & description) const
{
	auto editor = addLineEditor(param, label, description);
	if (editor) {
		if (decimals != 2)
			editor->setDecimals(decimals);
		editor->setRange(min, max);
	}
	return editor;
}

LineParamEditor * EditorsManager::addLineEditor(int32_t addr, int32_t idx, const QString & label, const QString &description) const
{
    return addLineEditor(controller()->paramsRegistry().element(addr, idx), label, description);
}

LineParamEditor * EditorsManager::addLineEditor(int32_t addr, int32_t idx, const QString & label, double min, const QString & description) const
{
    return addLineEditor(controller()->paramsRegistry().element(addr, idx), label, min, description);
}

LineParamEditor * EditorsManager::addLineEditor(int32_t addr, int32_t idx, const QString & label, double min, double max, int decimals, const QString & description) const
{
    return addLineEditor(controller()->paramsRegistry().element(addr, idx), label, min, max, decimals, description);
}

ListParamEditor * EditorsManager::addListEditor(const QString & label, const ListEditorContainer& items, bool active) const
{
	auto editor = new ListParamEditor(items);
	editor->setEnabled(active);
	addToLayout(label, editor);
	return editor;
}

ListParamEditor * EditorsManager::addListEditor(ParameterElement *param, const QString & label, const ListEditorContainer& items, bool active) const
{
	if (!param)
		return nullptr;

	auto editor = new ListParamEditor(param, items);
	editor->setEnabled(active);
	addToLayout(label, editor);
	return editor;
}

ListParamEditor * EditorsManager::addListEditor(int32_t addr, int32_t idx, const QString & label, const ListEditorContainer &items, bool active) const
{
    return addListEditor(controller()->paramsRegistry().element(addr, idx), label, items, active);
}

//
//ListParamEditor* EditorsManager::addListEditor(const QString& label, const ListEditorContainer& items) const
//{
//	auto editor = new ListParamEditor(items);
//	addToLayout(label, editor);
//	return editor;
//}
//
//ListParamEditor* EditorsManager::addListEditor(ParameterElement* param, const QString& label, const ListEditorContainer& items) const
//{
//	if (!param)
//		return nullptr;
//
//	auto editor = new ListParamEditor(param, items);
//	addToLayout(label, editor);
//	return editor;
//}
//
//ListParamEditor* EditorsManager::addListEditor(int32_t addr, int32_t idx, const QString& label, const ListEditorContainer& items) const
//{
//	return addListEditor(controller()->paramsRegistry().element(addr, idx), label, items);
//}
//
//ListParamEditor* EditorsManager::addListEditor(int32_t addr, int32_t idx, int32_t board, const QString& label, const ListEditorContainer& items) const
//{
//	return addListEditor(controller()->paramsRegistry().element(addr, idx, board), label, items);
//}

CheckParamEditor * EditorsManager::addCheckEditor(ParameterElement *param, const QString & label, int bit) const
{
	if (!param)
		return nullptr;

	auto editor = new CheckParamEditor(param, bit);
	addToLayout(label, editor);
	return editor;
}

CheckParamEditor * EditorsManager::addCheckEditor(int32_t addr, int32_t idx, const QString & label, int bit) const
{
    return addCheckEditor(controller()->paramsRegistry().element(addr, idx), label, bit);
}

void EditorsManager::addWidgetCreatorLayout(ListParamEditor * editor, const ListParamEditor::CreateWidgetFuncType & creator, QLayout * layout)
{
	if (!editor)
		return;

	if (!layout)
		layout = new QGridLayout;

	editor->setWidgetCreator(creator, layout);
	addLayout(layout, m_labelColumn, 1, m_layout->columnCount() + 1);
}

void EditorsManager::addWidgetCreatorLayout(ListParamEditor * editor, const ListParamEditor::CreateWidgetFuncType & creator, QLayout * layout, int column, int rowSpan, int columnSpan)
{
	if (!editor)
		return;

	editor->setWidgetCreator(creator, layout);
	addLayout(layout, column, rowSpan, columnSpan);
}

void EditorsManager::addVerticalStretch(int stretch)
{
	m_layout->setRowStretch(m_layout->rowCount(), stretch);
}

void EditorsManager::addHorizontalStretch(int stretch)
{
	m_layout->setColumnStretch(m_layout->columnCount(), stretch);
}

void EditorsManager::addStretch(int stretch)
{
	addVerticalStretch(stretch);
	addHorizontalStretch(stretch);
}

void EditorsManager::addToLayout(const QString & label, QWidget * editor, const QString &description) const
{
	int row = m_layout->rowCount();
	auto labelWidget = new QLabel(label);
	labelWidget->setStyleSheet(m_labelStyleSheet);
	m_layout->addWidget(labelWidget, row, m_labelColumn);
	m_layout->addWidget(editor, row, m_editorColumn, m_editorAlignment);
	if (!description.isEmpty())
		m_layout->addWidget(new QLabel(description), row, m_descriptionColumn);
}
