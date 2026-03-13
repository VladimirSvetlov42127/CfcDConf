#include "cfc_bo.h"


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
    int socket_radius = 3;
    QColor shape_color = QColor(70, 100, 120);
    QColor shape_bkcolor = QColor(245, 245, 245);
    QColor notbinded_bkcolor = QColor(255, 222, 222);

    int shape_width = 2;
    QFont CHANNEL_TEXT_FONT = QFont("Arial", 10);
    QFont LABEL_TEXT_FONT = QFont("Arial", 11, QFont::Bold);
    QFont INTO_TEXT_FONT = QFont("Arial", 8);
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcBO::CfcBO(QString id, QSizeF node_size, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    setName("BO");
    setNodeType(RZA_LOAD);
    node_size == QSizeF() ? setSize(QSizeF(150, 30)) : setSize(node_size);
    setInversion(false);
    initInputs(1, 1);

    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);

    setInputs(1);
}

CfcBO::CfcBO(QDomNode xml, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    setName("BO");
    setNodeType(RZA_LOAD);
    setInversion(false);
    initInputs(1, 1);

    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);

    for (int i = 0; i < paramNames().count(); i++)
        setParam(paramNames().at(i), paramValues().at(i));
}


//===================================================================================================================================================
//	Виртуальные методы класса
//===================================================================================================================================================
void CfcBO::setCfcOutput(CfcServiceOutput* output)
{
    //  Подключение входа сервиса
    _output = output;
    outputReset();

    return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcBO::paintElement(QPainter* painter)
{
    //  Формирование контура элемента
    QRectF rectangle = availableRect();
    QPainterPath path;
    path.moveTo(rectangle.x() + rectangle.width(), rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x() + rectangle.height() / 2, rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height() / 2 + socket_radius);
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height() / 2 - socket_radius);
    path.lineTo(rectangle.x() + +rectangle.height() / 2, rectangle.y());
    path.closeSubpath();

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(shape_color, shape_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
    cfcOutput()->target() ? painter->fillPath(path, shape_bkcolor) : painter->fillPath(path, notbinded_bkcolor);

    //  Перегрузка параметров элемента
    outputReset();

    //  Вывод названия сигнала
    QString text = cfcOutput()->text();

    if (!text.isEmpty()) {
        QFontMetrics fm(CHANNEL_TEXT_FONT);
        QRectF text_rect(QPointF(rectangle.left() + 10, rectangle.top()), QPointF(rectangle.right() - 10, rectangle.bottom()));
        int text_width = fm.horizontalAdvance(text) + rectangle.height();
        int old_width = size().width();
        int delta_width = text_width - old_width;
        if (abs(delta_width) > 10) setSize(QSizeF(text_width, size().height()));
        painter->setFont(CHANNEL_TEXT_FONT);
        painter->drawText(text_rect, Qt::AlignCenter, text);
    }
    update();
    painter->restore();

    return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void CfcBO::outputReset()
{
    if (cfcOutput()) {
        setParam("io_id", cfcOutput()->id() + 1);
        setParam("alg_pin", cfcOutput()->pin() + 1);
        if (cfcOutput()->target()) {
            setParam("name", cfcOutput()->target()->fullText());
            setParam("signal", cfcOutput()->target()->internalID());
        }
    }

    return;
}

