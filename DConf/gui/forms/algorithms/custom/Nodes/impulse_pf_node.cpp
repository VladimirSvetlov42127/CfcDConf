#include "impulse_pf_node.h"


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
ImpulsePF::ImpulsePF(QString ID, QSizeF node_size, QGraphicsItem* parent) : EditorNode(ID, parent)
{
    //  Настройка параметров
    SetType("ImpulsePF");
    node_size == QSizeF() ? SetNodeSize(QSizeF(120, 60)) : SetNodeSize(node_size);
    SetNodeType(FlexLogic::RZA_IMPULSE_LEADING);
    SetOutput(true);
    SetInversion(false);
    InitInputs(1, 1);
    InitSockets();
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");
}

ImpulsePF::~ImpulsePF()
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void ImpulsePF::paintElement(QPainter* painter)
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
    path.clear();

    //  Вывод диагонали
    painter->drawLine(QPointF(rectangle.bottomLeft().x() + 2, rectangle.bottomLeft().y() - 2), QPointF(rectangle.topRight().x() - 2, rectangle.topRight().y() + 2));

    //  Вывод левой фигуры
    QPainterPath left_figure;
    left_figure.moveTo(rectangle.left() + rectangle.width() * 0.07, rectangle.top() + rectangle.height() * 0.65);
    left_figure.lineTo(rectangle.left() + rectangle.width() * 0.17, rectangle.top() + rectangle.height() * 0.65);
    left_figure.lineTo(rectangle.left() + rectangle.width() * 0.17, rectangle.top() + rectangle.height() * 0.25);
    left_figure.lineTo(rectangle.left() + rectangle.width() * 0.27, rectangle.top() + rectangle.height() * 0.25);
    painter->drawPath(left_figure);

    //  Вывод правой фигуры
    QPainterPath right_figure;
    right_figure.moveTo(rectangle.left() + rectangle.width() * 0.6, rectangle.top() + rectangle.height() * 0.85);
    right_figure.lineTo(rectangle.left() + rectangle.width() * 0.7, rectangle.top() + rectangle.height() * 0.85);
    right_figure.lineTo(rectangle.left() + rectangle.width() * 0.7, (rectangle.top() + rectangle.height() / 2));
    right_figure.lineTo(rectangle.left() + rectangle.width() * 0.8, (rectangle.top() + rectangle.height() / 2));
    right_figure.lineTo(rectangle.left() + rectangle.width() * 0.8, rectangle.top() + rectangle.height() * 0.85);
    right_figure.lineTo(rectangle.left() + rectangle.width() * 0.9, rectangle.top() + rectangle.height() * 0.85);
    painter->drawPath(right_figure);

    //  Вывод параметров элемента
    if (ParamsList().size() == 1) {
        painter->setFont(FlexLogic::INTO_TEXT_FONT);
        QString  text = ParamsList().first().info + ParamsList().first().value.toString();
        painter->drawText(QPointF(rectangle.left() + FlexLogic::FROM_LEFT_TO_TEXT, rectangle.top() + FlexLogic::FROM_TOP_TO_TEXT), text); }
    painter->restore();

    return;
}
