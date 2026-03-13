#include "bi_node.h"

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
#include <QFontMetrics>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>

#include "service_manager/signals/virtual_input_signal.h"


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
BI::BI(QString ID, QSizeF node_size, CfcServiceInput* input, QGraphicsItem* parent)
    : EditorNode(ID, parent)
    , _input{input}
{
    //  Настройка параметров
    SetType("BI");
    SetNodeType(FlexLogic::RZA_LOAD);
    node_size == QSizeF() ? SetNodeSize(QSizeF(150, 30)) : SetNodeSize(node_size);
    SetOutput(true);
    SetInversion(false);
    InitInputs(0, 0);
    InitSockets();

    //  Добавление параметров элемента
    addParam("Название", "name", QString());
    addParam("Сигнал", "signal", -1);
    addParam("io_id", "io_id", -1);
    addParam("AlgPin", "alg_pin", -1);
}


BI::~BI()
{
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void BI::setCfcInput(CfcServiceInput* input)
{
    //  Подключение входа сервиса
    _input = input;
}

QList<FlexLogic::NodeParam> BI::ParamsList()
{
    if (!cfcInput())
        return EditorNode::ParamsList();

    QList<FlexLogic::NodeParam> result;
    result.append({QString(), "io_id", cfcInput()->id(), QString()});
    result.append({QString(), "alg_pin", cfcInput()->pin() + 1, QString()});
    result.append({QString(), "name", cfcInput()->text(), QString()});
    result.append({QString(), "signal", cfcInput()->source() ? cfcInput()->source()->internalID() : -1, QString()});
    return result;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void BI::paintElement(QPainter* painter)
{
    //  Формирование контура элемента
    QRectF rectangle = AvailableRect();
    QPainterPath path;
    path.moveTo(rectangle.x(), rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width() - rectangle.height() / 2, rectangle.y());
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height() / 2 - FlexLogic::socket_radius);
    path.lineTo(rectangle.x() + rectangle.width(), rectangle.y() + rectangle.height() / 2 + FlexLogic::socket_radius);
    path.lineTo(rectangle.x() + rectangle.width() - rectangle.height() / 2, rectangle.y() + rectangle.height());
    path.lineTo(rectangle.x(), rectangle.y() + rectangle.height());
    path.closeSubpath();

    auto fillColor = FlexLogic::shape_bkcolor;
    if (cfcInput()->source()) {
        fillColor = FlexLogic::shape_bkcolor_binded;
        auto vdin = cfcInput()->source()->to<VirtualInputSignal>();
        if (vdin && !vdin->source())
            fillColor = Qt::yellow;
    }

    //  Вывод и заливка контура элемента
    painter->save();
    painter->setPen(QPen(FlexLogic::shape_color, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->setRenderHint(QPainter::Antialiasing);
    painter->fillPath(path, fillColor);
    painter->drawPath(path);

    //  Вывод названия сигнала
    QString text = cfcInput()->text();
    QFontMetrics fm(FlexLogic::CHANNEL_TEXT_FONT);
    QRectF text_rect(QPointF(rectangle.x() + rectangle.height(), rectangle.y()), QPointF(rectangle.right() - 10, rectangle.bottom()));
    int text_width = fm.horizontalAdvance(text) + 40 + rectangle.height();
    int old_width = NodeSize().width();
    int delta_width = text_width - old_width;
    if (abs(delta_width) > 10) {
        SetNodeSize(QSizeF(text_width, NodeSize().height()));
        setPos(pos().x() - delta_width, pos().y());
        Sockets().at(0)->setPos(QPointF(NodeSize().width(), NodeSize().height() / 2));	}
    painter->setFont(FlexLogic::CHANNEL_TEXT_FONT);
    painter->drawText(text_rect, Qt::AlignCenter, text);
    update();
    painter->restore();

    return;
}
