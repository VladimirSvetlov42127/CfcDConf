#include "cfc_return_delay.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QSizeF>
#include <QPainterPath>
#include <QRectF>
#include <QPen>
#include <QColor>
#include <QFont>

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    QColor shape_color = QColor(70, 100, 120);
    QColor shape_bkcolor = QColor(245, 245, 245);
    int shape_width = 2;
    QFont CHANNEL_TEXT_FONT = QFont("Arial", 11);
    QFont LABEL_TEXT_FONT = QFont("Arial", 11, QFont::Bold);
    QFont INTO_TEXT_FONT = QFont("Arial", 8);
    uint8_t FROM_LEFT_TO_TEXT = 8;
    uint8_t FROM_BOTTOM_TO_TEXT = 5;
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcReturnDelay::CfcReturnDelay(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("ReturnDelay");
    setNodeType(RZA_DELAY_OFF);
    node_size == QSizeF() ? setSize(QSizeF(120, 60)) : setSize(node_size);
    setInversion(false);
    initInputs(1, 1);
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");

    setOutput();
    setInputs(1);
}

CfcReturnDelay::CfcReturnDelay(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("ReturnDelay");
    setNodeType(RZA_DELAY_OFF);
    setInversion(false);
    initInputs(1, 1);
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");
    for (int i = 0; i < paramNames().count(); i++)
        setParam(paramNames().at(i), paramValues().at(i));
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcReturnDelay::paintElement(QPainter* painter)
{
    QRectF rectangle = availableRect();
    QPainterPath path;
    path.addRoundedRect(rectangle, 5, 5);

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(shape_color, shape_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(path, shape_bkcolor);
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
    painter->setFont(LABEL_TEXT_FONT);
    painter->drawText(QPointF(rectangle.right() - 2 * margin_w, 2 * margin_h), "t");

    //  Вывод параметров элемента
    if (paramsList().size() == 1) {
        painter->setFont(INTO_TEXT_FONT);
        QString  param_text = paramsList().at(0).info + paramsList().at(0).value.toString();
        painter->drawText(QPointF(rectangle.left() + FROM_LEFT_TO_TEXT, rectangle.bottom() - FROM_BOTTOM_TO_TEXT), param_text);
    }
    painter->restore();

    return;
}
