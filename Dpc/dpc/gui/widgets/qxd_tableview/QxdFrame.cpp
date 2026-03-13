#include "QxdFrame.h"

#include <QFocusEvent>
#include <QApplication>

QxdFrame::QxdFrame(QWidget *parent) :
     QFrame(parent, Qt::SubWindow)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Plain);
    setAutoFillBackground(true);
    setFocusPolicy(Qt::ClickFocus);
    installEventFilter(this);
}

QxdFrame::~QxdFrame()
{
}

bool QxdFrame::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::FocusOut) {
        QFocusEvent *fe = static_cast<QFocusEvent*>(e);
        if ( fe->lostFocus() &&
                !(qApp->focusWidget() == this || findChildren<QWidget*>().contains(qApp->focusWidget())) )
            emit lostFocus();
    }

    return false;
}

void QxdFrame::showEvent(QShowEvent *event)
{
    QFrame::showEvent(event);

    foreach(QWidget *w, findChildren<QWidget*>())
        w->installEventFilter(this);
}
