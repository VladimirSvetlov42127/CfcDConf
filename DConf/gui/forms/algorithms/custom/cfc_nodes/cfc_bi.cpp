#include "cfc_bi.h"


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
//	Подключение модулей проекта
//===================================================================================================================================================
#include "service_manager/signals/virtual_input_signal.h"


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
CfcBI::CfcBI(QString id, QSizeF node_size, CfcServiceInput* input, QGraphicsItem* parent) : CfcNode(id, parent)
{
    //  Настройка параметров
    _input = input;
    setName("BI");
    setNodeType(RZA_LOAD);
    node_size == QSizeF() ? setSize(QSizeF(150, 30)) : setSize(node_size);
    setInversion(false);
    initInputs(0, 0);

    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);

    setOutput();
}

CfcBI::CfcBI(QDomNode xml, CfcServiceInput* input, QGraphicsItem* parent) : CfcNode(xml, parent)
{
    //  Настройка параметров
    _input = input;
    setName("BI");
    setNodeType(RZA_LOAD);
    setInversion(false);
    initInputs(0, 0);

    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);

    //  Обновление и пересчет параметров
    for (int i = 0; i < paramNames().count(); i++)
        setParam(paramNames().at(i), paramValues().at(i));
}


//===================================================================================================================================================
//	Виртуальные методы класса
//===================================================================================================================================================
QList<NodeParam> CfcBI::paramsList() const
{
    if (!cfcInput())
        return CfcBasicNode::paramsList();

    QList<NodeParam> result;
    result.append({QString(), "io_id", cfcInput()->id(), QString()});
    result.append({QString(), "alg_pin", cfcInput()->pin() + 1, QString()});
    result.append({QString(), "name", cfcInput()->text(), QString()});
    result.append({QString(), "signal", cfcInput()->source() ? cfcInput()->source()->internalID() : -1, QString()});
    return result;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void CfcBI::paintElement(QPainter* painter)
{
    //  Формирование контура элемента
    QRectF rectangle = availableRect();
    QPainterPath path;
    path.moveTo(rectangle.x(), rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width() - rectangle.height() / 2, rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height() / 2 - socket_radius);
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height() / 2 + socket_radius);
    path.lineTo(rectangle.x() + rectangle.width() - rectangle.height() / 2, rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height());
    path.closeSubpath();

    QColor fill_color = notbinded_bkcolor;
    if (cfcInput()->source()) {
        fill_color = shape_bkcolor;
        auto vdin = cfcInput()->source()->to<VirtualInputSignal>();
        if (vdin && !vdin->source())
            fill_color = Qt::yellow;
    }

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(shape_color, shape_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->drawPath(path);
    painter->fillPath(path, fill_color);

    //  Вывод названия сигнала
    QString text = cfcInput()->text();
    if (!text.isEmpty()) {
        QFontMetrics fm(CHANNEL_TEXT_FONT);
        QRectF text_rect(QPointF(rectangle.x() + 10, rectangle.y()), QPointF(rectangle.right() - 10, rectangle.bottom()));
        int text_width = fm.horizontalAdvance(text) + rectangle.height();
        int old_width = size().width();
        int delta_width = text_width - old_width;
        if (abs(delta_width) > 10) {
            setSize(QSizeF(text_width, size().height()));
            setPos(pos().x() - delta_width, pos().y());
            sockets().at(0)->setPos(QPointF(size().width(), size().height() / 2));
        }
        painter->setFont(CHANNEL_TEXT_FONT);
        painter->drawText(text_rect, Qt::AlignCenter, text);
    }
    update();
    painter->restore();

    return;
}

