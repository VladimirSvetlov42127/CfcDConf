#include "DcForm.h"

#include <qlabel.h>
#include <qscrollarea.h>
#include <qevent.h>

DcForm::DcForm(DcController *controller, const QString &title, bool hasShape)
    : m_controller(controller)
    , m_title(title)
    , m_centralWidget(new QWidget)
    , m_journal(nullptr)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 0);
    layout->setSpacing(2);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    if (!hasShape)
        scrollArea->setFrameShape(QFrame::NoFrame);

    scrollArea->setWidget(m_centralWidget);
    layout->addWidget(scrollArea);
}

DcForm::~DcForm()
{
    //    qDebug() << Q_FUNC_INFO << objectName();
}

void DcForm::setJournal(Dpc::Journal *journal)
{
    m_journal = journal;
}

Dpc::Journal *DcForm::journal() const
{
    return m_journal;
}

DcController* DcForm::controller() const
{
    return m_controller;
}

QString DcForm::title() const
{
    return m_title;
}

QWidget * DcForm::centralWidget() const
{
	return m_centralWidget;
}

void DcForm::setTitle(const QString & title)
{
    m_title = title;
}

bool DcForm::hasAny(DcController * controller, const QList<Param>& params)
{
    for (auto &it : params)
        if (controller->paramsRegistry().element(it.addr, it.index))
            return true;

    return false;
}

bool DcForm::hasAll(DcController * controller, const QList<Param>& params)
{
    for (auto &it : params)
        if (!controller->paramsRegistry().element(it.addr, it.index))
            return false;

    return true;
}
