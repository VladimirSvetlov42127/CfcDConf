#include "editor_socket.h"


//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QPainter>
#include <QUuid>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorSocket::EditorSocket(FlexLogic::SocketType type, bool inversion, QGraphicsItem* parent) : QGraphicsObject(parent), EditorSocketData(type, inversion)
{
    _high_light = false;
    setAcceptHoverEvents(true);
    setAcceptDrops(true);
}

EditorSocket::~EditorSocket()
{
}


//===================================================================================================================================================
//  Вспомогательные методы класса
//===================================================================================================================================================
void EditorSocket::SetAssets()
{
    if (HighLight()) {
        _pen = QPen(FlexLogic::socket_select_color, 0);
        _brush = QBrush(FlexLogic::socket_select_bkcolor);
        return; }

    if (SocketType() == FlexLogic::SocketType::INPUT_SOCKET) {
        _pen = QPen(FlexLogic::shape_color, FlexLogic::shape_width);
        _brush = QBrush(FlexLogic::input_bkcolor);
        return; }

    if (SocketType() == FlexLogic::SocketType::OUTPUT_SOCKET) {
        _pen = QPen(FlexLogic::shape_color, FlexLogic::shape_width);
        _brush = QBrush(FlexLogic::output_bkcolor);
        return; }

    return;
}


//===================================================================================================================================================
//  Перегружаемые методы класса
//===================================================================================================================================================
QRectF EditorSocket::boundingRect() const
{
    return QRectF(-FlexLogic::select_shape, -FlexLogic::select_shape,
        2 * FlexLogic::select_shape, 2 * FlexLogic::select_shape);
}

void EditorSocket::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    SetAssets();
    painter->setPen(_pen);
    painter->setBrush(_brush);
    QRectF rectangle = boundingRect();
    int radius = 0;
    HighLight() ? radius = 2 * FlexLogic::socket_radius : radius = FlexLogic::socket_radius + FlexLogic::shape_width / 2;
    if (Inversion() && SocketType() == FlexLogic::SocketType::OUTPUT_SOCKET) painter->drawEllipse(QPointF(0, 0), radius, radius);
    if (!Inversion() || SocketType() == FlexLogic::SocketType::INPUT_SOCKET) painter->drawRect(rectangle.center().x() - radius,
        rectangle.center().y() - radius, 2 * radius, 2 * radius);
    return;
}


//===================================================================================================================================================
//  Обработка событий мышки
//===================================================================================================================================================
void EditorSocket::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    SetHighLight(true);
    update();
    QGraphicsItem::hoverEnterEvent(event);
}

void EditorSocket::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    SetHighLight(false);
    update();
    QGraphicsItem::hoverLeaveEvent(event);
}

