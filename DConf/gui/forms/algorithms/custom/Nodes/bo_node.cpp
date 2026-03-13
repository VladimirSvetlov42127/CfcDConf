#include "bo_node.h"


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
BO::BO(QString ID, QSizeF node_size, CfcServiceOutput* output, QGraphicsItem* parent)
    : EditorNode(ID, parent)
    , _output{output}
{
    //  Настройка параметров
    SetType("BO");
    SetNodeType(FlexLogic::RZA_LOAD);
    node_size == QSizeF() ? SetNodeSize(QSizeF(150, 30)) : SetNodeSize(node_size);
    SetOutput(false);
    SetInversion(false);
    InitInputs(1, 1);
    InitSockets();

    //  Добавление параметров элемента
    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);
}

BO::~BO()
{
}

//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void BO::setCfcOutput(CfcServiceOutput* output)
{
    _output = output;
}

QList<FlexLogic::NodeParam> BO::ParamsList()
{
    if (!cfcOutput())
        return EditorNode::ParamsList();

    QList<FlexLogic::NodeParam> result;
    result.append({QString(), "io_id", cfcOutput()->id(), QString()});
    result.append({QString(), "alg_pin", cfcOutput()->pin() + 1, QString()});
    result.append({QString(), "name", cfcOutput()->text(), QString()});
    result.append({QString(), "signal", cfcOutput()->target() ? cfcOutput()->target()->internalID() : -1, QString()});
    return result;
}

//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void BO::paintElement(QPainter* painter)
{
    //  Формирование контура элемента
    QRectF rectangle = AvailableRect();
    QPainterPath path;
    path.moveTo(rectangle.x() + rectangle.width(), rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x() + rectangle.height() / 2, rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height() / 2 + FlexLogic::socket_radius);
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height() / 2 - FlexLogic::socket_radius);
    path.lineTo(rectangle.x() + +rectangle.height() / 2, rectangle.y());
    path.closeSubpath();

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(FlexLogic::shape_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(path, cfcOutput()->target() ? FlexLogic::shape_bkcolor_binded : FlexLogic::shape_bkcolor);
    painter->drawPath(path);

    //  Вывод названия сигнала
    QString text = cfcOutput()->text();
    QFontMetrics fm(FlexLogic::CHANNEL_TEXT_FONT);
    QRectF text_rect(QPointF(rectangle.left() + 10, rectangle.top()), QPointF(rectangle.right() - rectangle.height(), rectangle.bottom()));
    int text_width = fm.horizontalAdvance(text) + 40 + rectangle.height();
    int old_width = NodeSize().width();
    int delta_width = text_width - old_width;
    if (abs(delta_width) > 10) SetNodeSize(QSizeF(text_width, NodeSize().height()));
    painter->drawText(text_rect, Qt::AlignCenter, text);
    update();
    painter->restore();

    return;
}
