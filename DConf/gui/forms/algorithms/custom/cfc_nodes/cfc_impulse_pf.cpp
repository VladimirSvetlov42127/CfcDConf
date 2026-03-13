#include "cfc_impulse_pf.h"


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
    uint8_t FROM_TOP_TO_TEXT = 11;
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcImpulsePF::CfcImpulsePF(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("ImpulsePF");
    node_size == QSizeF() ? setSize(QSizeF(120, 60)) : setSize(node_size);
    setNodeType(RZA_IMPULSE_LEADING);
    setInversion(false);
    initInputs(1, 1);
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");

    setOutput();
    setInputs(1);
}

CfcImpulsePF::CfcImpulsePF(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("ImpulsePF");
    setNodeType(RZA_IMPULSE_LEADING);
    setInversion(false);
    initInputs(1, 1);
    addParam("Время срабатывания, мс", "time", 200, "t, мс = ");

    for (int i = 0; i < paramNames().count(); i++)
        setParam(paramNames().at(i), paramValues().at(i));
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcImpulsePF::paintElement(QPainter* painter)
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
    if (paramsList().size() == 1) {
        painter->setFont(INTO_TEXT_FONT);
        QString  text = paramsList().at(0).info + paramsList().at(0).value.toString();
        painter->drawText(QPointF(rectangle.left() + FROM_LEFT_TO_TEXT, rectangle.top() + FROM_TOP_TO_TEXT), text);
    }
    painter->restore();

    return;
}
