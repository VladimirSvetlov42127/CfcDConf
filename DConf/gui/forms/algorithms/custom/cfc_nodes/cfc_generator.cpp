#include "cfc_generator.h"


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
    uint8_t FROM_TOP_TO_TEXT = 11;
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcGenerator::CfcGenerator(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("Generator");
    setNodeType(RZA_PULSE_REPETITIVE);
    setInversion(false);
    initInputs(0, 0);
    node_size == QSizeF() ? setSize(QSizeF(120, 60)) : setSize(node_size);
    setOutput();
    addParam("Частота", "period", 50, "Частота, Гц = ");
    addParam("Величина", "value", 0, "Величина = ");
}

CfcGenerator::CfcGenerator(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("Generator");
    setNodeType(RZA_PULSE_REPETITIVE);
    setInversion(false);
    initInputs(0, 0);
    addParam("Частота", "period", 50, "Частота, Гц = ");
    addParam("Величина", "value", 0, "Величина = ");

    for (int i = 0; i < paramNames().count(); i++)
        setParam(paramNames().at(i), paramValues().at(i));
}



//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcGenerator::paintElement(QPainter* painter)
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

    //  Вывод значка генератора
    path.moveTo(rectangle.left() + rectangle.width() * 0.33, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.44, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.44, rectangle.top() + rectangle.height() * 0.33);
    path.lineTo(rectangle.left() + rectangle.width() * 0.55, rectangle.top() + rectangle.height() * 0.33);
    path.lineTo(rectangle.left() + rectangle.width() * 0.55, rectangle.top() + rectangle.height() * 0.66);
    path.lineTo(rectangle.left() + rectangle.width() * 0.66, rectangle.top() + rectangle.height() * 0.66);
    painter->drawPath(path);

    //  Вывод параметров генератора
    if (paramsList().size() == 2) {
        painter->setFont(INTO_TEXT_FONT);
        QString  freq_param = paramsList().at(0).info + paramsList().at(0).value.toString();
        painter->drawText(QPointF(rectangle.left() + FROM_LEFT_TO_TEXT, rectangle.bottom() - FROM_BOTTOM_TO_TEXT), freq_param);
        QString  value_param = paramsList().at(1).info + paramsList().at(1).value.toString();
        painter->drawText(QPointF(rectangle.left() + FROM_LEFT_TO_TEXT, rectangle.top() + FROM_TOP_TO_TEXT), value_param);
    }
    painter->restore();

    return;
}
