#include "cfc_view.h"

#include <QGraphicsScene>

CfcView::CfcView(QGraphicsScene *scene) : QGraphicsView(scene)
{
    //  Свойства класса
    _base_x = 0;
    _base_y = 0;
    _base_width = 1600;
    _base_height = 1200;

    this->setMouseTracking(true);
    viewport()->setMouseTracking(true);

    setDragMode(QGraphicsView::RubberBandDrag);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    //  Параметры сцены
    setContentsMargins(5, 5, 5, 5);
    setSceneRect(_base_x, _base_y, _base_width, _base_height);
}


void CfcView::SceneChanged(const QRectF& rectangle)
{
    QRectF new_bound;
    rectangle.x() < _base_x ? new_bound.setX(rectangle.x()) : new_bound.setX(_base_x);
    rectangle.y() < _base_y ? new_bound.setY(rectangle.y()) : new_bound.setY(_base_y);
    rectangle.width() - rectangle.x() < _base_width ? new_bound.setWidth(_base_width) : new_bound.setWidth(rectangle.width() - rectangle.x());
    rectangle.height() - rectangle.y() < _base_height ? new_bound.setHeight(_base_height) : new_bound.setHeight(rectangle.height() - rectangle.y());

    setSceneRect(new_bound);
    update();
}

