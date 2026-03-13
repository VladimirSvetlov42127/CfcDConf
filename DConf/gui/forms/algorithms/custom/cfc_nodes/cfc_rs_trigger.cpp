#include "cfc_rs_trigger.h"


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
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcRsTrigger::CfcRsTrigger(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("RsTrigger");
    setNodeType(RZA_TRIGGER);
    node_size == QSizeF() ? setSize(QSizeF(60, 90)) : setSize(node_size);
    setInversion(false);
    initInputs(2, 2);

    setOutput();
    setInputs(2);
}

CfcRsTrigger::CfcRsTrigger(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("RsTrigger");
    setNodeType(RZA_TRIGGER);
    setInversion(false);
    initInputs(2, 2);
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcRsTrigger::paintElement(QPainter* painter)
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
 
    //  Вывод вертикальной линии
    qreal center = rectangle.left() + rectangle.width() / 2;
    painter->drawLine(center, 0, center, rectangle.bottom());

    //  Вывод символов сигналов
    painter->setFont(LABEL_TEXT_FONT);
    QRectF t_rect(QPointF(center, 0), QPointF(rectangle.right(), rectangle.bottom()));
    painter->drawText(t_rect, Qt::AlignCenter, "T");
    QRectF s_rect(rectangle.topLeft(), QPointF(center, rectangle.height() / 2));
    painter->drawText(s_rect, Qt::AlignCenter, "S");
    QRectF r_rect(QPointF(rectangle.left(), rectangle.height() / 2), QPointF(center, rectangle.bottom()));
    painter->drawText(r_rect, Qt::AlignCenter, "R");
    painter->restore();

    return;
}
