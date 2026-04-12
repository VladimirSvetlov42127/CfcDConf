#include "grid_item.h"


//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
#include <cmath>
namespace {
    uint8_t item_step = 5;
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
GridItem::GridItem(QGraphicsItem* parent) : QGraphicsItem(parent)
{
    setFlags(QGraphicsItem::ItemIsSelectable |
             QGraphicsItem::ItemIsMovable |
             QGraphicsItem::ItemSendsGeometryChanges);
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
void GridItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _offset = pos() - calcGridPoint(pos());
    QGraphicsItem::mousePressEvent(event);
}

QVariant GridItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF new_point = value.toPointF();
        if(change == ItemPositionChange && scene()){
            QPointF close_point = calcGridPoint(new_point);
            return  close_point+=_offset;
        }
        else
            return new_point;
    }
    else
        return QGraphicsItem::itemChange(change, value);
}

QPointF GridItem::calcGridPoint(const QPointF &point)
{
    qreal x = floor(point.x()/item_step)*item_step;
    qreal y = floor(point.y()/item_step)*item_step;
    return QPointF(x, y);
}
