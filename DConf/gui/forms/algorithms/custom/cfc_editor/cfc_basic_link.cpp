#include "cfc_basic_link.h"

#include <QDebug>
#include <QLineF>

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    int delta = 5;
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcBasicLink::CfcBasicLink(QString id, QList<QPointF> points, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    //  Формирование id соединения
    if (!id.isEmpty()) {
        uint16_t int_id = id.toUInt();
        if (int_id >= _counter) _counter = int_id;
        _id = id; }
    else _id = QString::number(++_counter);

    //  Свойства класса
    _source = nullptr;
    _target = nullptr;
    _source_id = QString();
    _target_id = QString();
    _source_index = -1;
    _target_index = -1;

    //  Заполнение списка точек
    _points = points;

    //	Установк флагов
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    //setFlag(QGraphicsItem::ItemIsMovable);
    //setZValue(5);
}

CfcBasicLink::CfcBasicLink(QDomNode xml, QGraphicsItem* parent) : QGraphicsObject(parent)
{
    //  Свойства класса
    _source = nullptr;
    _target = nullptr;
    _source_id = QString();
    _target_id = QString();
    _source_index = -1;
    _target_index = -1;

    //	Получение параметров связи
    _id = xml.toElement().attribute("__id");
    if (_id.toInt() >= _counter)
        _counter = _id.toInt();
    _source_index = xml.toElement().attribute("source_connector").toInt();
    _target_index = xml.toElement().attribute("target_connector").toInt();

    //	Получение параметров связи
    QDomNode child_node = xml.firstChild();
    while (!child_node.isNull()) {
        QString tag_name = child_node.toElement().tagName();

        //	Чтение source_ref
        if (tag_name == "source") {
            _source_id = child_node.toElement().attribute("__ref");
            child_node = child_node.nextSibling();
            continue; }

        //	Чтение target_ref
        if (tag_name == "target") {
            _target_id = child_node.toElement().attribute("__ref");
            child_node = child_node.nextSibling();
            continue; }

        //	Получение координат точек
        if (tag_name == "points") {
            QDomNodeList points_list = xml.toElement().elementsByTagName("point");
            for (int ii = 0; ii < points_list.count(); ii++) {
                QDomNode point_node = points_list.at(ii);
                if (point_node.isNull()) continue;
                double x = point_node.toElement().attribute("x").toDouble();
                double y = point_node.toElement().attribute("y").toDouble();
                _points.append(QPointF((int)x, (int)y));
            }
            child_node = child_node.nextSibling();
            continue;
        }
    }

    //	Установк флагов
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
    //setFlag(QGraphicsItem::ItemIsMovable);
    //setZValue(5);
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void CfcBasicLink::setSource(CfcSocket* source)
{
    _source =  source;
    if (source) {
        _points[0] = source->scenePos();
        _points[1].setY(source->scenePos().y());
        update();
    }
}

void CfcBasicLink::setTarget(CfcSocket* target)
{
    _target = target;
    if (target) {
        int last = _points.count() - 1;
        _points[last] = target->scenePos();
        _points[last - 1].setY(target->scenePos().y());
        update();
    }
}

QList<CfcLine> CfcBasicLink::lines() const
{
    QList<CfcLine> lines = QList<CfcLine>();
    QList<QPointF> cfc_points = points();
    if (cfc_points.count() < 2)
        return lines;

    for (int i = 0; i < cfc_points.count() - 1; i++) {
        CfcLine line(cfc_points.at(i), cfc_points.at(i + 1));
        if (i != 0 && (line.type() == CfcLine::NONE || line.type() == CfcLine::DIAGONAL))
            line.setType(lines.last().type());
        lines.append(line);
    }

    return lines;
}

void CfcBasicLink::normalize()
{
    QList<QPointF> delete_list;
    QList<CfcLine> cfc_lines = lines();

    //	Удаление лишних точек
    int count = cfc_lines.count() - 1;
    for (int i = 0; i < count; i++)
        if (cfc_lines.at(i).type() == cfc_lines.at(i + 1).type())
            delete_list.append(cfc_lines.at(i).end());
    if (!delete_list.isEmpty())
        for (int i = 0; i < delete_list.count(); i++) _points.removeOne(delete_list.at(i));

    return;
}

bool CfcBasicLink::addNode(const QPointF& node)
{
    //	Проверка исходных данных
    if (node == QPointF())
        return false;

    //	Перебор узлов
    int count = points().count() - 1;
    for (int i = 0; i < count; i++) {
        if (!isOnLine(points().at(i), points().at(i + 1), node))
            continue;

        QPointF new_node = node;
        if (points().at(i).x() == points().at(i + 1).x())
            new_node.setX(points().at(i).x());
        if (points().at(i).y() == points().at(i + 1).y())
            new_node.setY(points().at(i).y());
        _points.insert(i + 1, new_node);
        _points.insert(i + 1, new_node);
        return true;
    }

    return false;
}

int CfcBasicLink::selectedLine(const QPointF& point)
{
    QList<CfcLine> cfc_lines = lines();
    for (int i = 0; i < cfc_lines.count(); i++)
        if (isOnLine(cfc_lines.at(i).begin(), cfc_lines.at(i).end(), point))
            return i;
    return -1;
}

bool CfcBasicLink::moveLine(int line, const QPointF& point)
{
    QList<CfcLine> cfc_lines = lines();
    if (line < 1 || line == cfc_lines.count() - 1)
        return false;

    CfcLine moving_line = cfc_lines.at(line);
    QList<QPointF> old_points = points();
    if (moving_line.type() == CfcLine::VERTICAL) {
        if (moving_line.begin().x() == point.x())
            return false;
        _points[line].setX(point.x());
        _points[line + 1].setX(point.x());
    }

    if (moving_line.type() == CfcLine::HORIZONTAL) {
        if (moving_line.begin().y() == point.y())
            return false;
        _points[line].setY(point.y());
        _points[line + 1].setY(point.y());
    }
    arrangePoints(old_points);

    return true;
}

QPolygonF CfcBasicLink::polygon() const
{
    QPolygonF cfc_polygon = QPolygonF();
    QList<QPointF> cfc_points = points();
    if (cfc_points.isEmpty()) return cfc_polygon;

    for (int i = 0; i < cfc_points.count(); i++)
        cfc_polygon.append(cfc_points.at(i));

    return cfc_polygon;
}


//===================================================================================================================================================
//	Сериализация элемента
//===================================================================================================================================================
QDomNode CfcBasicLink::toXML() const
{
    QDomDocument xml;
    QDomElement xml_link = xml.createElement("CONNECTION");

    //	Стандартные аттрибуты соединения
    xml_link.setAttribute("__type", "DseGraphicsConnection");
    xml_link.setAttribute("__id", id());
    xml_link.setAttribute("source", source()->id());
    xml_link.setAttribute("target", target()->id());
    xml_link.setAttribute("source_connector", sourceIndex());
    xml_link.setAttribute("target_connector", targetIndex());

    //	Добавление source
    xml_link.setAttribute("source_connector", sourceIndex());
    QDomElement xml_source = xml.createElement("source");
    xml_source.setAttribute("__ref", sourceID());
    xml_link.appendChild(xml_source);

    //	Добавление target
    xml_link.setAttribute("target_connector", targetIndex());
    QDomElement xml_target = xml.createElement("target");
    xml_target.setAttribute("__ref", targetID());
    xml_link.appendChild(xml_target);

    //	Добавление координат точек
    QDomElement xml_points_list = xml.createElement("points");
    xml_points_list.setAttribute("__node_type", "list");
    for (int i = 0; i < points().count(); i++) {
        QDomElement xml_point = xml.createElement("point");
        xml_point.setAttribute("x", points().at(i).x());
        xml_point.setAttribute("y", points().at(i).y());
        xml_points_list.appendChild(xml_point);	}
    xml_link.appendChild(xml_points_list);

    return xml_link;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
bool CfcBasicLink::isOnLine(const QPointF& begin, const QPointF& end, const QPointF& node)
{
    //	Проверка точки на пределы
    qreal min_x = begin.x();
    if (end.x() < min_x)
        min_x = end.x();
    qreal max_x = begin.x();
    if (end.x() > max_x)
        max_x = end.x();

    qreal min_y = begin.y();
    if (end.y() < min_y)
        min_y = end.y();
    qreal max_y = begin.y();
    if (end.y() > max_y)
        max_y = end.y();

    if (node.y() < min_y - delta || node.y() > max_y + delta) return false;
    if (node.x() < min_x - delta || node.x() > max_x + delta) return false;

    return true;
}

void CfcBasicLink::arrangePoints(QList<QPointF> old_points)
{
    QList<QPointF> new_points = points();
    QList<CfcLine> current_lines = lines();
    for (int i = 0; i < current_lines.count(); i++) {
        if (current_lines.at(i).type() != CfcLine::NONE)
            continue;
        if (current_lines.at(i).end() != old_points.at(i + 1))
            new_points.insert(i + 1, old_points.at(i + 1));
        if (current_lines.at(i).begin() != old_points.at(i))
            new_points.insert(i + 1, old_points.at(i));
        break; }

    setPoints(new_points);
}
