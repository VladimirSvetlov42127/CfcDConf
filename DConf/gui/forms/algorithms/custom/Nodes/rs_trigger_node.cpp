#include "rs_trigger_node.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QPainterPath>
#include <QPainter>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
RsTrigger::RsTrigger(QString ID, QSizeF node_size, QGraphicsItem* parent) : EditorNode(ID, parent)
{
    //  Настройка параметров
    SetType("RsTrigger");
    SetNodeType(FlexLogic::RZA_TRIGGER);
    node_size == QSizeF() ? SetNodeSize(QSizeF(60, 90)) : SetNodeSize(node_size);
    SetOutput(true);
    SetInversion(false);
    InitInputs(2, 2);
    InitSockets();
}

RsTrigger::~RsTrigger()
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void RsTrigger::paintElement(QPainter* painter)
{
    QRectF rectangle = AvailableRect();
    QPainterPath path;
    path.addRoundedRect(rectangle, 5, 5);

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(FlexLogic::shape_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(path, FlexLogic::shape_bkcolor);
    painter->drawPath(path);
 
    //  Вывод вертикальной линии
    qreal center = rectangle.left() + rectangle.width() / 2;
    painter->drawLine(center, 0, center, rectangle.bottom());

    //  Вывод символов сигналов
    painter->setFont(FlexLogic::LABEL_TEXT_FONT);
    QRectF t_rect(QPointF(center, 0), QPointF(rectangle.right(), rectangle.bottom()));
    painter->drawText(t_rect, Qt::AlignCenter, "T");
    QRectF s_rect(rectangle.topLeft(), QPointF(center, rectangle.height() / 2));
    painter->drawText(s_rect, Qt::AlignCenter, "S");
    QRectF r_rect(QPointF(rectangle.left(), rectangle.height() / 2), QPointF(center, rectangle.bottom()));
    painter->drawText(r_rect, Qt::AlignCenter, "R");
    painter->restore();

    return;
}
