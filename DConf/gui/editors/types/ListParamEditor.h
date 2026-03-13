#pragma once

#include <qvariant.h>

#include <gui/editors/types/ParamEditor.h>
#include <gui/editors/types/ListEditorContainer.h>

class QLayout;

namespace Dpc::Gui 
{
	class ComboBox;
}

class ListParamEditor : public ParamEditor
{
	Q_OBJECT
public:
	using CreateWidgetFuncType = std::function<QWidget*(const QVariant &data)>;

    ListParamEditor(ParameterElement *param, const ListEditorContainer &items, QWidget *parent = nullptr);
	ListParamEditor(const ListEditorContainer &items, QWidget *parent = nullptr);

	void setList(const ListEditorContainer &items);

	void setItemData(int idx, const QVariant &data, int role = Qt::UserRole);
	void setItemData(const QString &value, const QVariant &data, int role = Qt::UserRole);

	void setWidgetCreator(const CreateWidgetFuncType &creator, QLayout *layout);

	virtual void setValue(const QString &value) override;
	QString value() const;

    void setCurrentByText(const QString &text);
    QString currentText() const;

private slots:
	void onCurrentIndexChanged(int index);

private:
	void init(const ListEditorContainer &items);

private:
	Dpc::Gui::ComboBox *m_box;
};
