#ifndef TEXTFILTEREDITOR_H
#define TEXTFILTEREDITOR_H

#include <dpc/gui/widgets/qxd_tableview/filters/FilterEditor.h>
#include <dpc/gui/widgets/qxd_tableview/filters/TextFilter.h>

class QLineEdit;
class QCheckBox;

class TextFilterEditor : public FilterEditor
{
public:
	TextFilterEditor(TextFilter *filter, QWidget *parent = nullptr);
	~TextFilterEditor();

	virtual void apply() override;

private:
	TextFilter *m_filter;

	QLineEdit *m_patternEdit;
	QCheckBox *m_exactMatchCheckBox;
	QCheckBox *m_regExpCheckBox;	
};

#endif // TEXTFILTEREDITOR_H
