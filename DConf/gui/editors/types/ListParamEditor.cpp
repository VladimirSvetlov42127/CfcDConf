#include "ListParamEditor.h"

#include <qboxlayout.h>
#include <qdebug.h>

#include <dpc/gui/widgets/ComboBox.h>

using namespace Dpc::Gui;

namespace {
	const int g_ValueRole = 0x1000;

	QWidget* makeWidget(const ListParamEditor::CreateWidgetFuncType &creator, const QVariant &data) 
	{
		QWidget *widget = nullptr;
		if (data.isValid())
			widget = creator(data);

		return widget ? widget : new QWidget;
	}
}

ListParamEditor::ListParamEditor(ParameterElement *param, const ListEditorContainer & items, QWidget * parent) :
	ParamEditor(param, parent)
{
	init(items);
}

ListParamEditor::ListParamEditor(const ListEditorContainer & items, QWidget * parent) :
	ParamEditor(parent)
{
	init(items);
}

void ListParamEditor::setList(const ListEditorContainer & items)
{
	m_box->blockSignals(true);
	m_box->clear();

	int currentIdx = -1;
	for (size_t i = 0; i < items.size(); i++) {
		auto item = items.at(i);
		m_box->addItem(item.text);
		m_box->setItemData(i, item.value, g_ValueRole);
		if (param() && param()->value() == item.value.toString())
			currentIdx = i;
	}
	if (param()) {
		m_box->setCurrentIndex(currentIdx);
	}

	m_box->blockSignals(false);
}

void ListParamEditor::setItemData(int idx, const QVariant & data, int role)
{
	m_box->setItemData(idx, data, role);
}

void ListParamEditor::setItemData(const QString & value, const QVariant & data, int role)
{
	for (size_t i = 0; i < m_box->count(); i++)
		if (m_box->itemData(i, g_ValueRole).toString() == value)
			m_box->setItemData(i, data, role);
}

void ListParamEditor::setWidgetCreator(const CreateWidgetFuncType & creator, QLayout * layout)
{
	if (!layout || !creator)
		return;

	QWidget *itemWidget = makeWidget(creator, m_box->currentData(g_ValueRole));
	layout->addWidget(itemWidget);

	auto onCurrentIndexChanged = [=]() mutable {
		QWidget *newWidget = makeWidget(creator, m_box->currentData(g_ValueRole));
		layout->replaceWidget(itemWidget, newWidget);
		itemWidget->deleteLater();
		itemWidget = newWidget;
	};

	connect(m_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, onCurrentIndexChanged);
}

void ListParamEditor::setValue(const QString & value)
{
	int index = -1;
	for (int i = 0; i < m_box->count(); i++)
		if (m_box->itemData(i, g_ValueRole).toString() == value) {
			index = i;
			break;
		}

	m_box->setCurrentIndex(index);
}

QString ListParamEditor::value() const
{
    return m_box->currentData(g_ValueRole).toString();
}

void ListParamEditor::setCurrentByText(const QString &text)
{
    int index = -1;
    for (int i = 0; i < m_box->count(); i++)
        if (m_box->itemText(i) == text) {
            index = i;
            break;
        }

    m_box->setCurrentIndex(index);
}

QString ListParamEditor::currentText() const
{
    return m_box->currentText();
}

void ListParamEditor::onCurrentIndexChanged(int index)
{
	if (index < 0)
		return;

	QString newValue = m_box->currentData(g_ValueRole).toString();
	setParamValue(newValue);
}

void ListParamEditor::init(const ListEditorContainer & items)
{
	m_box = new ComboBox;
	m_box->setEditable(false);
	connect(m_box, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_box);

	setList(items);
}
