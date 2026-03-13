#include "TextFilterEditor.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QCheckBox>

namespace {
    const QString titleString = "Текст";
    const QString exactMatchString = "Точное соответствие";
    const QString regExpString = "Регулярное выражение";
}

TextFilterEditor::TextFilterEditor(TextFilter *filter, QWidget *parent) :
    FilterEditor(titleString, parent),
	m_filter(filter),
	m_patternEdit(new QLineEdit(this)),
    m_exactMatchCheckBox(new QCheckBox(exactMatchString, this)),
    m_regExpCheckBox(new QCheckBox(regExpString, this))
{
    //setGeometry(0, 0, 250, 160);

    if (!filter) {
        setActive(false);
        return;
    }

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(m_patternEdit);
	layout->addWidget(m_exactMatchCheckBox);
	layout->addWidget(m_regExpCheckBox);
	layout->addStretch();

	m_patternEdit->setText(filter->pattern());
    m_regExpCheckBox->setChecked(!filter->regExp().isValid());
	m_exactMatchCheckBox->setChecked(filter->exactMatch());

    setGeometry(0, 0, sizeHint().width(), sizeHint().height());
}

TextFilterEditor::~TextFilterEditor()
{
}

void TextFilterEditor::apply()
{
	m_filter->setPattern(m_patternEdit->text());
	m_filter->setExactMatch(m_exactMatchCheckBox->isChecked());
	if (m_regExpCheckBox->isChecked())
        m_filter->setRegExp(QRegularExpression(m_filter->pattern()));
	else
        m_filter->setRegExp(QRegularExpression());

	m_filter->setEnabled( !m_filter->pattern().isEmpty() || m_filter->exactMatch() );
}
