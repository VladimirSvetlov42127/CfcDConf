#include "cfc_xor.h"


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
CfcXor::CfcXor(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("Xor");
    setNodeType(RZA_XOR);
    node_size == QSizeF() ? setSize(QSizeF(60, 90)) : setSize(node_size);
    setInversion(true);
    initInputs(2, 16);

    setOutput();
    setInputs(2);
}

CfcXor::CfcXor(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("Xor");
    setNodeType(RZA_XOR);
    setInversion(true);
    initInputs(2, 16);
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcXor::paintElement(QPainter* painter)
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

    //  Вывод текста преобразования
    int text_bottom = 30;
    QRectF textRect(rectangle.topLeft(), QPointF(rectangle.right(), text_bottom));
    painter->setFont(LABEL_TEXT_FONT);
    painter->drawText(textRect, Qt::AlignCenter, "XOR");
    painter->drawLine(rectangle.left(), text_bottom, rectangle.right(), text_bottom);
    painter->restore();

    return;
}
