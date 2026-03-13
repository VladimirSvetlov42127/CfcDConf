#include "generator_node.h"

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
Generator::Generator(QString ID, QSizeF node_size, QGraphicsItem* parent) : EditorNode(ID, parent)
{
    //  Настройка параметров
    SetType("Generator");
    SetNodeType(FlexLogic::RZA_PULSE_REPETITIVE);
    node_size == QSizeF() ? SetNodeSize(QSizeF(120, 60)) : SetNodeSize(node_size);
    SetOutput(true);
    SetInversion(false);
    InitInputs(0, 0);
    InitSockets();
    addParam("Частота", "period", 50, "Частота, Гц = ");
    addParam("Величина", "value", 0, "Величина = ");
}

Generator::~Generator()
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void Generator::paintElement(QPainter* painter)
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

    //  Вывод значка генератора
    path.moveTo(rectangle.left() + rectangle.width() * 0.33, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.44, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.44, rectangle.top() + rectangle.height() * 0.33);
    path.lineTo(rectangle.left() + rectangle.width() * 0.55, rectangle.top() + rectangle.height() * 0.33);
    path.lineTo(rectangle.left() + rectangle.width() * 0.55, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.66, rectangle.top() + rectangle.height() * 0.66);
    painter->drawPath(path);

    //  Вывод параметров генератора
    if (ParamsList().size() == 2) {
        painter->setFont(FlexLogic::INTO_TEXT_FONT);
        QString  freq_param = ParamsList().first().info + ParamsList().first().value.toString();
        painter->drawText(QPointF(rectangle.left() + FlexLogic::FROM_LEFT_TO_TEXT, rectangle.bottom() - FlexLogic::FROM_BOTTOM_TO_TEXT), freq_param);
        QString  value_param = ParamsList().last().info + ParamsList().last().value.toString();
        painter->drawText(QPointF(rectangle.left() + FlexLogic::FROM_LEFT_TO_TEXT, rectangle.top() + FlexLogic::FROM_TOP_TO_TEXT), value_param); }
    painter->restore();

    return;
}
