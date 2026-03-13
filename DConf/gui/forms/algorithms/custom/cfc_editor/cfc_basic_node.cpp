#include "cfc_basic_node.h"


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_input_socket.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_output_socket.h"

//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcBasicNode::CfcBasicNode(QString id, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    //  Формирование id соединения
    bool ok;
    if (!id.isEmpty()) {
        uint16_t int_id = id.toUInt(&ok);
        if (int_id >= _counter) _counter = int_id;
        _id = id; }
    else _id = QString::number(++_counter);

    //	Установк флагов
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setZValue(10);
}

CfcBasicNode::CfcBasicNode(QDomNode xml, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    //	Получение параметров связи
    bool ok;
    _id = xml.toElement().attribute("__id");
    int int_id = _id.toInt(&ok);
    if (ok && int_id >= _counter)
        _counter = _id.toInt();
    _name = xml.toElement().attribute("__type");

    //	Получение параметров узла
    QStringList param_names;
    QStringList param_values;
    QDomNode child_node = xml.firstChild();

    uint8_t inputs_count = 0;
    while (!child_node.isNull()) {
        QString tag_name = child_node.toElement().tagName();

        //	Чтение координат элемента
        if (tag_name == "pos") {
            double x_pos = child_node.toElement().attribute("x").toFloat();
            double y_pos = child_node.toElement().attribute("y").toFloat();
            this->setPos(QPointF(x_pos, y_pos));
            child_node = child_node.nextSibling();
            continue;
        }

        //	Чтение размера элемента
        if (tag_name == "size") {
            double node_width = child_node.toElement().attribute("width").toFloat();
            double node_height = child_node.toElement().attribute("height").toFloat();
            setSize(QSizeF((int)node_width, (int)node_height));
            child_node = child_node.nextSibling();
            continue;
        }

        //	Чтение списка соединителей
        if (tag_name == "connectors") {
            QDomNodeList connect_list = child_node.toElement().elementsByTagName("connector");
            for (int i = 0; i < connect_list.count(); i++) {
                QDomNode connect_node = connect_list.at(i);
                QString type_tag = connect_node.toElement().attribute("type");
                if (type_tag == "Output") setOutput();
                if (type_tag == "Input") inputs_count++;
            }
            child_node = child_node.nextSibling();
            continue;
        }

        //	Чтение параметров элемента
        QString value = child_node.toElement().attribute("value");
        _param_names.append(tag_name);
        _param_values.append(value);
        child_node = child_node.nextSibling();
        continue;
    }
    if (inputs_count >0)
        setInputs(inputs_count);

    //	Установк флагов
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    setFlag(QGraphicsItem::ItemIsMovable);
    setZValue(10);
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
uint8_t CfcBasicNode::inputs()
{
    return hasOutput() ? sockets().count() - 1 : sockets().count();
}

void CfcBasicNode::initInputs(uint8_t min, uint8_t max)
{
    _min_inputs = min;
    _max_inputs = max;
}

bool CfcBasicNode::hasOutput() const
{
    if (sockets().count() < 1)
        return false;
    CfcOutputSocket* output = static_cast<CfcOutputSocket*>(_sockets.at(0));
    return output ? true : false;
}

void CfcBasicNode::setOutput()
{
    CfcOutputSocket* socket = new CfcOutputSocket(0, inversion(), this, this);
    socket->setPos(size().width(),  size().height() / 2);
    _sockets.append(socket);
}

void CfcBasicNode::setInputs(uint8_t count)
{
    if (count < 1) return;
    int index = sockets().count();
    qreal delta = size().height() / (count + 1);

    //  Добавление входных сокетов
    for (uint8_t i = 0; i < count; i++) {
        CfcInputSocket* socket = new CfcInputSocket(index + i, this, this);
        socket->setPos(0, delta + i * delta);
        _sockets.append(socket);
    }

    return;
}

void CfcBasicNode::addInput()
{
    int count = _sockets.count();
    if (hasOutput()) count--;
    if (count == maxInputs()) return;

    //	Изменение размера элемента
    count++;
    qreal delta = availableRect().height() / count;
    QSizeF new_size = QSizeF(size().width(), size().height() + delta);
    setSize(new_size);

    //	Добавление входного сокета
    CfcInputSocket* socket = new CfcInputSocket(sockets().count(), this, this);
    socket->setPos(QPointF(availableRect().x(), availableRect().y() + delta * count));
    _sockets.append(socket);

    //	Изменение положения выходного сокета
    if (!hasOutput()) return;
    if (count < 1) return;
    sockets().at(0)->setPos(QPointF(availableRect().width(), size().height() / 2));

    return;
}

void CfcBasicNode::removeInput()
{
    //	Проверка количества сокетов
    int count = _sockets.count();
    if (hasOutput()) count--;
    if (count == minInputs()) return;

    //	Проверка возможности удаления сокета
    if (!sockets().at(sockets().count() - 1)->links().isEmpty())
        return;

    //	Изменение размера элемента
    qreal delta = availableRect().height() / (count + 1);
    QSizeF new_size = QSizeF(size().width(), size().height() - delta);
    setSize(new_size);
    count--;


    //	Удаление последнего входа
    delete sockets().at(sockets().count() - 1);
    _sockets.removeLast();

    //	Изменение положения выходного сокета
    if (!hasOutput()) return;
    if (count < 1) return;
    sockets().at(0)->setPos(QPointF(availableRect().width(), size().height() / 2));

    return;
}


//===================================================================================================================================================
//	Параметры элемента
//===================================================================================================================================================
void CfcBasicNode::addParam(QString name, QString id, QVariant value, QString short_info)
{
    NodeParam element({ name, id, value, short_info });
    _params_list.append(element);

    return;
}

void CfcBasicNode::addParam(QString name, QString id, QVariant value)
{
    QString info = name + " = ";
    NodeParam elem({ name, id, value, info });
    _params_list.append(elem);

    return;
}

NodeParam CfcBasicNode::param(QString id) const
{
    for (auto& it : _params_list)
        if (it.index == id)
            return it;

    return NodeParam();
}

void CfcBasicNode::setParam(QString id, QVariant value)
{
    if (_params_list.isEmpty())
        return;

    int size = _params_list.size();
    for (int i = 0; i < size; i++) {
        if (_params_list.at(i).index != id)
            continue;
        NodeParam parameter = _params_list.at(i);
        parameter.value = value;
        _params_list.replace(i, parameter);
        return;
    }

    return;
}

void CfcBasicNode::setParam(int index, QVariant value)
{
    if (_params_list.isEmpty())
        return;
    _params_list[index].value = value;

    return;
}

//===================================================================================================================================================
//	Сериализация элемента
//===================================================================================================================================================
QDomNode CfcBasicNode::toXML()
{
    QDomDocument xml;
    QDomElement xml_node = xml.createElement("ELEMENT");

    //	Аттрибуты элемента
    xml_node.setAttribute("__type", name());
    xml_node.setAttribute("__id", id());

    //	Расположение элемента
    QDomElement xml_pos = xml.createElement("pos");
    xml_pos.setAttribute("x", pos().x());
    xml_pos.setAttribute("y", pos().y());
    xml_node.appendChild(xml_pos);

    //	Размер элемента
    QDomElement xml_size = xml.createElement("size");
    xml_size.setAttribute("width", size().width());
    xml_size.setAttribute("height", size().height());
    xml_node.appendChild(xml_size);

    //	Список соединений
    QDomElement xml_sockets_list = xml.createElement("connectors");
    xml_sockets_list.setAttribute("__node_type", "list");
    for (int i = 0; i < sockets().count(); i++) {
        QDomElement xml_socket = xml.createElement("connector");
        xml_socket.setAttribute("id", sockets().at(i)->id());
        sockets().at(i)->socketType() == CfcSocket::INPUT_SOCKET ? xml_socket.setAttribute("type", "Input") :
            xml_socket.setAttribute("type", "Output");
        xml_sockets_list.appendChild(xml_socket);
    }
    xml_node.appendChild(xml_sockets_list);

    //	Список свойств элемента
    for (int i = 0; i < paramsList().count(); i++) {
        QDomElement xml_param = xml.createElement(paramsList().at(i).index);
        xml_param.setAttribute("value", paramsList().at(i).value.toString());
        xml_node.appendChild(xml_param);
    }

    return xml_node;
}

