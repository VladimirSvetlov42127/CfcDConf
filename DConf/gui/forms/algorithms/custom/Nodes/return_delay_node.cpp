#include "return_delay_node.h"

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
ReturnDelay::ReturnDelay(QString ID, QSizeF node_size, QGraphicsItem* parent) : EditorNode(ID, parent)
{
    //  Настройка параметров
    SetType("ReturnDelay");
    SetNodeType(FlexLogic::RZA_DELAY_OFF);
    node_size == QSizeF() ? SetNodeSize(QSizeF(120, 60)) : SetNodeSize(node_size);
    SetOutput(true);
    SetInversion(false);
    InitInputs(1, 1);
    InitSockets();
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");
}

ReturnDelay::~ReturnDelay()
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void ReturnDelay::paintElement(QPainter* painter)
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

    //  Вывод символа элемента
    qreal margin_w = rectangle.width() / 10;
    qreal margin_h = rectangle.width() / 12;
    qreal center_h = rectangle.top() + rectangle.height() / 2;
    painter->drawLine(QPointF(rectangle.left() + margin_w, center_h), QPointF(rectangle.right() - margin_w, center_h));
    painter->drawLine(QPointF(rectangle.left() + margin_w, center_h - margin_h), QPointF(rectangle.left() + margin_w, center_h + margin_h));
    painter->drawLine(QPointF(rectangle.right() - margin_w, center_h - margin_h), QPointF(rectangle.right() - margin_w, center_h + margin_h));
 
    QLineF diagonal(QPointF(rectangle.right() - 3 * margin_w, center_h - margin_h), QPointF(rectangle.left() + 3 * margin_w, center_h + margin_h));
    painter->drawLine(diagonal);
    QLineF normal_line = diagonal.normalVector();
    normal_line.setLength(margin_h / 2);
    normal_line.setPoints(normal_line.p2(), normal_line.p1());
    normal_line.setLength(margin_h);
    painter->drawLine(normal_line);

    //  Вывод метки
    painter->setFont(FlexLogic::LABEL_TEXT_FONT);
    painter->drawText(QPointF(rectangle.right() - 2 * margin_w, 2 * margin_h), "t");

    //  Вывод параметров элемента
    if (ParamsList().size() == 1) {
        painter->setFont(FlexLogic::INTO_TEXT_FONT);
        QString  param_text = ParamsList().first().info + ParamsList().first().value.toString();
        painter->drawText(QPointF(rectangle.left() + FlexLogic::FROM_LEFT_TO_TEXT, rectangle.bottom() - FlexLogic::FROM_BOTTOM_TO_TEXT), param_text); }
    painter->restore();

    return;
}
