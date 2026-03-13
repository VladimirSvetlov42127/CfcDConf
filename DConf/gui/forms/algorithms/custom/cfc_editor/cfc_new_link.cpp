#include "cfc_new_link.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QGraphicsScene>
#include <QGraphicsView>


//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    uint8_t select_shape = 10;
    QColor select_color = QColor(Qt::blue);
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcNewLink::CfcNewLink(CfcSocket* socket, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    _released = false;
    _current_socket = socket;
    _source_socket = nullptr;
    _target_socket = nullptr;
    _mouse_point = socket->scenePos();
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
QList<QPointF> CfcNewLink::points() const
{
    QList<QPointF> points;
    QPointF _mid_point = (_mouse_point + firstPoint()) / 2;

    points.append(firstPoint());
    points.append(QPointF(_mid_point.x(), firstPoint().y()));
    points.append(QPointF(_mid_point.x(), _mouse_point.y()));
    points.append(QPointF(_mouse_point));

    return points;
}

QRectF CfcNewLink::boundingRect() const
{
    QPolygonF cfc_polygon = polygon();
    if (cfc_polygon.isEmpty())
        return QRectF();
    return cfc_polygon.boundingRect().adjusted(-select_shape, -select_shape, select_shape, select_shape);
}

QPainterPath CfcNewLink::shape() const
{
    QPainterPathStroker ps;
    ps.setWidth(select_shape);

    return ps.createStroke(path());
}


//===================================================================================================================================================
//	Виртуальные методы класса
//===================================================================================================================================================
void CfcNewLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    //  Настройка вывода
    painter->save();
    painter->setPen(QPen(select_color, 1, Qt::DotLine));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(QBrush());

    //  Вывод ломаной линии
    painter->drawPath(path());

    //	Вывод точек
    painter->setPen(QPen(select_color, 0));
    painter->setBrush(QBrush(Qt::white));

    QList<QPointF> cfc_points = points();
    for (int i = 0; i < cfc_points.count(); i++)
        painter->drawEllipse(cfc_points.at(i), select_shape / 2, select_shape / 2);
    painter->restore();

    return;
}

void CfcNewLink::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    scene()->views().at(0)->setCursor(QCursor(Qt::ArrowCursor));
    _released = true;
    _mouse_point = event->scenePos();
    CfcSocket* socket = dynamic_cast<CfcSocket*>(scene()->itemAt(_mouse_point, QTransform()));
    if (!socket)
        return;
    if (socket->socketType() == _current_socket->socketType())
        return;

    //  Определение источника и приемника сигнала
    socket->socketType() == CfcSocket::INPUT_SOCKET ? _target_socket = socket : _source_socket = socket;
    _current_socket->socketType() == CfcSocket::INPUT_SOCKET ? _target_socket = _current_socket : _source_socket = _current_socket;

    QGraphicsItem::mouseReleaseEvent(event);
}

void CfcNewLink::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if (_released)
        return;

    _mouse_point = event->scenePos();
    CfcSocket* socket = dynamic_cast<CfcSocket*>(scene()->itemAt(_mouse_point, QTransform()));
    if (!socket && scene()->views().at(0)->cursor() != Qt::CrossCursor)
        scene()->views().at(0)->setCursor(Qt::CrossCursor);

    // if (socket && (socket->socketType() == _current_socket->socketType())) scene()->views().at(0)->setCursor(Qt::ForbiddenCursor);
    if (socket)
        socket->socketType() == _current_socket->socketType() ? scene()->views().at(0)->setCursor(Qt::ForbiddenCursor) :
        scene()->views().at(0)->setCursor(Qt::DragCopyCursor);

    update();
    QGraphicsItem::mouseMoveEvent(event);

}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QPolygonF CfcNewLink::polygon() const
{
    QPolygonF cfc_polygon = QPolygonF();
    QList<QPointF> cfc_points = points();
    if (cfc_points.isEmpty()) return cfc_polygon;

    for (int i = 0; i < cfc_points.count(); i++)
        cfc_polygon.append(cfc_points.at(i));

    return cfc_polygon;
}

QPainterPath CfcNewLink::path() const
{
    //  Заполнение списка графических элементов
    QList<QPointF> cfc_points = points();
    QPainterPath cfc_path = QPainterPath();
    cfc_path.moveTo(cfc_points.at(0));
    for (int i = 1; i < cfc_points.count(); i++)
        cfc_path.lineTo(cfc_points.at(i));

    return cfc_path;
}
