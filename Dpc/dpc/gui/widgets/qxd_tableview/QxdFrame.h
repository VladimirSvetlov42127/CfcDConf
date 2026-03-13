#ifndef QXDFRAME_H
#define QXDFRAME_H

#include <QFrame>

class QxdFrame : public QFrame
{
    Q_OBJECT
public:
    QxdFrame(QWidget *parent = nullptr);
    virtual ~QxdFrame();

signals:
    void lostFocus();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e) override;
    virtual void showEvent(QShowEvent *event) override;
};

#endif // QXDFRAME_H
