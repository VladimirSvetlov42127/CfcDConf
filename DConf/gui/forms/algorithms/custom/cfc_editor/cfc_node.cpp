#include "cfc_node.h"

#include <QDebug>
//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "gui/forms/algorithms/custom/cfc_editor/cfc_link.h"

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    uint8_t select_shape = 10;
    uint8_t select_box = 4;
    QColor select_color = QColor(Qt::blue);
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcNode::CfcNode(QString id, QGraphicsItem* parent) : CfcBasicNode(id, parent)
{
}

CfcNode::CfcNode(QDomNode xml, QGraphicsItem* parent) : CfcBasicNode(xml, parent)
{
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void CfcNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    paintElement(painter);
    if (isSelected()) paintSelected(painter);
}

QRectF CfcNode::boundingRect() const
{
    QRectF rectangle = availableRect();
    int x_margin = select_box + select_shape;
    int y_margin = select_box + select_shape / 2;

    return rectangle.adjusted(-x_margin, -y_margin, x_margin, y_margin);
}

void CfcNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //  Проверка области срабатывания мышки
    if (!shape().contains(event->pos())) {
        event->setAccepted(false);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        setCursor(QCursor(Qt::ClosedHandCursor));
        event->setAccepted(true);
    }

    return;
}

void CfcNode::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    QGraphicsItem::mouseReleaseEvent(event);
    if (!shape().contains(event->pos())) event->setAccepted(false);

    return;
}

QVariant CfcNode::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        for (int i = 0; i < sockets().count(); i++) {
            CfcSocket* socket = sockets().at(i);
            for (int ii = 0; ii < socket->links().count(); ii++)
                if (socket->links().at(ii))
                    socket->links().at(ii)->move();
            update();
        }
    }
    return QGraphicsItem::itemChange(change, value);
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcNode::paintSelected(QPainter* painter)
{
    //	Сохранение параметров рисования
    QRectF rectangle = boundingRect();
    painter->save();

    //	Вывод прямоугольника
    painter->setPen(QPen(select_color, 0, Qt::DotLine));
    painter->setBrush(QBrush());
    painter->drawRect(rectangle.x() + select_box / 2, rectangle.y() + select_box / 2,
                      rectangle.width() - select_box, rectangle.height() - select_box);

    //	Вывод узловых точек
    painter->setPen(QPen(select_color, 0, Qt::SolidLine));
    painter->setBrush(QBrush(Qt::white));
    painter->drawRect(rectangle.x(), rectangle.y(), select_box, select_box);
    painter->drawRect(rectangle.x() + rectangle.width() / 2 - select_box / 2, rectangle.y(), select_box, select_box);
    painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y(), select_box, select_box);
    painter->drawRect(rectangle.x(), rectangle.y() + rectangle.height() / 2 - select_box / 2, select_box, select_box);
    painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y() + rectangle.height() / 2 - select_box / 2, select_box, select_box);
    painter->drawRect(rectangle.x(), rectangle.y() + rectangle.height() - select_box, select_box, select_box);
    painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y() + rectangle.height() - select_box, select_box, select_box);
    painter->drawRect(rectangle.x() + rectangle.width() / 2 - select_box / 2, rectangle.y() + rectangle.height() - select_box, select_box, select_box);

    //	Восстановление параметров рисования
    painter->restore();

    return;
}
