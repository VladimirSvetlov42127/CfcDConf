#ifndef __CFC_VIEW_H__
#define __CFC_VIEW_H__

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QResizeEvent>
#include <QMouseEvent>


class CfcView: public QGraphicsView
{
    Q_OBJECT

public:
    CfcView(QGraphicsScene *scene = nullptr);

public slots:
    void SceneChanged(const QRectF& rectangle);

private:
    int _base_x;
    int _base_y;
    int _base_width;
    int _base_height;
};

#endif // __CFC_VIEW_H__
