#include "cfc_output_socket.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPointF>

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    int shape_width = 2;
    int socket_radius = 3;
    QColor shape_color = QColor(70, 100, 120);
    QColor output_bkcolor = QColor(Qt::yellow);
    QColor socket_select_color = QColor(255, 0, 0);
    QColor socket_select_bkcolor = QColor(255, 150, 150);
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcOutputSocket::CfcOutputSocket(uint8_t index, bool inversion, CfcBasicNode* node, QGraphicsItem* parent)
    : CfcSocket(index, node, parent)
{
    _inversion = inversion;
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
QDomNode CfcOutputSocket::toXML() const
{
    QDomDocument xml;
    QDomElement xml_socket = xml.createElement("connector");
    xml_socket.setAttribute("id", id());
    xml_socket.setAttribute("type", "Output");

    return xml_socket;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void CfcOutputSocket::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    int radius = socket_radius + shape_width / 2;
    QPen pen = QPen(shape_color, shape_width);
    QBrush brush = QBrush(output_bkcolor);
    if (highLight()) {
        radius = 2 * socket_radius;
        pen = QPen(socket_select_color, 0);
        brush = QBrush(socket_select_bkcolor);
    }
    painter->setPen(pen);
    painter->setBrush(brush);

    inversion() ? painter->drawEllipse(QPointF(0, 0), radius, radius) :
        painter->drawRect(boundingRect().center().x() - radius, boundingRect().center().y() - radius, 2 * radius, 2 * radius);

    return;
}
