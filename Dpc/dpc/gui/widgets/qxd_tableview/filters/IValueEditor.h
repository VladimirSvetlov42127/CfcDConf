#ifndef IVALUEEDITOR_H
#define IVALUEEDITOR_H

#include <QWidget>

class IValueEditor : public QWidget
{
    Q_OBJECT
public:
    explicit IValueEditor(QWidget *parent = nullptr);
    virtual ~IValueEditor();
signals:
    void valueChanged(const QVariant &);
};

#endif // IVALUEEDITOR_H
