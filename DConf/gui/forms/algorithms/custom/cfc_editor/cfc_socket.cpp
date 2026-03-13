#include "cfc_socket.h"


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "gui/forms/algorithms/custom/cfc_editor/cfc_basic_node.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_link.h"

//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcSocket::CfcSocket(uint8_t index, CfcBasicNode* node, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    _index = index;
    _parent = node;     //  dynamic_cast<CfcNode*>(parent);
    _id = QString::number(++_counter);
    _highlight = false;
    setAcceptHoverEvents(true);
    setAcceptDrops(true);
}

//===================================================================================================================================================
//	Базовые методы класса
//===================================================================================================================================================
void CfcSocket::appendLink(CfcLink* link)
{
    if (!link)
        return;
    _links.append(link);
}


void CfcSocket::removeLink(CfcLink* link)
{
    if (!link)
        return;

    int index = _links.indexOf(link);
    if (index < 0)
        return;
    _links.removeAt(index);

    return;
}

//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
QRectF CfcSocket::boundingRect() const
{
    return QRectF(-_select_shape, -_select_shape, 2 * _select_shape, 2 * _select_shape);
}

void CfcSocket::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    _highlight = true;
    QGraphicsItem::hoverEnterEvent(event);
}

void CfcSocket::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    _highlight = false;
    QGraphicsItem::hoverLeaveEvent(event);
}


