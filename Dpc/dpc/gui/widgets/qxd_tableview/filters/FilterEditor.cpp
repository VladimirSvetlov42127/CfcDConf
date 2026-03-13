#include "FilterEditor.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QSizeGrip>

FilterEditor::FilterEditor(const QString &title, QWidget *parent) :
    QxdFrame(parent),
    m_centralWidget(new QWidget(this)),
    m_titleLabel(new QLabel(title, this)),
    m_okButton(new QPushButton("Ok"))
{
    //setGeometry(0, 0, 150, 200);

    m_titleLabel->setAlignment(Qt::AlignHCenter);
    QFont f = m_titleLabel->font();
	f.setBold(true);
    m_titleLabel->setFont(f);
    m_titleLabel->setMaximumHeight(32);
	QHBoxLayout *titleLayout = new QHBoxLayout;
	titleLayout->setContentsMargins(9, 9, 9, 0);
    titleLayout->addWidget(m_titleLabel);

	QHBoxLayout *centralWidgetLayout = new QHBoxLayout;
	centralWidgetLayout->setContentsMargins(9, 0, 9, 0);
	centralWidgetLayout->addWidget(m_centralWidget);

    connect(m_okButton, &QPushButton::clicked, this, &FilterEditor::accept);
	QHBoxLayout *okButtonLayout = new QHBoxLayout;
	okButtonLayout->setContentsMargins(9, 0, 9, 0);
	okButtonLayout->addStretch();
    okButtonLayout->addWidget(m_okButton);

	QHBoxLayout *sgLayout = new QHBoxLayout;
	sgLayout->addStretch();
	sgLayout->addWidget(new QSizeGrip(this));

	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->addLayout(titleLayout);
	mainLayout->addLayout(centralWidgetLayout);		
	mainLayout->addLayout(okButtonLayout);
    mainLayout->addLayout(sgLayout);
}

FilterEditor::~FilterEditor()
{
}

void FilterEditor::accept()
{
    apply();
    emit accepted();
}

void FilterEditor::setActive(bool active)
{
    m_okButton->setEnabled(active);
}

QWidget * FilterEditor::centralWidget() const
{
    return m_centralWidget;
}

void FilterEditor::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}
