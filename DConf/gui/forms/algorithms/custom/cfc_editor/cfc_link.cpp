#include "cfc_link.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QColor>
#include <QGraphicsScene>
#include <QList>
#include <QDebug>

//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    QColor connection_color = QColor(220, 95, 30);
    int connection_width = 2;
    int intersection_radius = 5;

    uint8_t select_shape = 10;
    QColor select_color = QColor(Qt::blue);
}


//===================================================================================================================================================
//	Конструкторы класса
//===================================================================================================================================================
CfcLink::CfcLink(QString id, QList<QPointF> points) : CfcBasicLink(id, points)
{
    //  Свойства класса
    _sorting_line = nullptr;
    _is_moving = false;
    _need_update = true;
    _path = QPainterPath();

    //  Инициализация меню
    _menu_point = QPointF();
    _menu.addAction("Добавить узел", this, &CfcLink::linkAddNode);
    _menu.addAction("Убрать лишние узлы", this, &CfcLink::removeNodes);
}

CfcLink::CfcLink(QDomNode xml) : CfcBasicLink(xml)
{
    //  Свойства класса
    _sorting_line = nullptr;
    _is_moving = false;
    _need_update = true;
    _path = QPainterPath();

    //  Инициализация меню
    _menu_point = QPointF();
    _menu.addAction("Добавить узел", this, &CfcLink::linkAddNode);
    _menu.addAction("Убрать лишние узлы", this, &CfcLink::removeNodes);
}


//===================================================================================================================================================
//	открытые методы класса
//===================================================================================================================================================
void CfcLink::needUpdate()
{
    _need_update = true;
    update();
}

void CfcLink::move()
{
    bool source_selected = false;
    bool target_selected = false;
    if (source()->parent()->isSelected())
        source_selected = true;
    if (target()->parent()->isSelected())
        target_selected = true;
    QList<QPointF> cfc_points = points();

    if (target_selected && source_selected) {
        QPointF delta = source()->scenePos() - cfc_points[0];
        for (int i = 0; i < cfc_points.count(); i++)
            cfc_points[i] = cfc_points[i] + delta;
    }

    if (source_selected) {
        cfc_points[0] = source()->scenePos();
        cfc_points[1].setY( cfc_points[0].y());
    }

    if (target_selected) {
        cfc_points[cfc_points.count() - 1] = target()->scenePos();
        cfc_points[cfc_points.count() - 2].setY(cfc_points[cfc_points.count() - 1].y());
    }
    setPoints(cfc_points);
    needUpdate();
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
QRectF CfcLink::boundingRect() const
{
    QPolygonF cfc_polygon = polygon();
    if (cfc_polygon.isEmpty())
        return QRectF();
    return cfc_polygon.boundingRect().adjusted(-select_shape, -select_shape, select_shape, select_shape);
}

QPainterPath CfcLink::shape() const
{
    QPainterPathStroker ps;
    ps.setWidth(select_shape);

    return ps.createStroke(_path);
}

void CfcLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    painter->setPen(QPen(connection_color, connection_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    _path = path();
    painter->drawPath(_path);
    if (isSelected())
        paintSelected(painter);
    painter->restore();
}

void CfcLink::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (!shape().contains(event->scenePos())) {
        event->setAccepted(false);
        return;
    }

    //  Не реагирует на клик по узлу
    QPointF position = QPointF((int)event->pos().x(), (int)event->pos().y());
    for (int i = 0; i < points().count(); i++) {
        QLineF line(position, points().at(i));
        if (line.length() < select_shape) {
            event->setAccepted(false);
            return;
        }
    }

    //  Вызов меню
    if (event->buttons() & Qt::RightButton) {
        if (!isSelected())
            return;
        _menu_point = event->scenePos();
        _menu.exec(event->screenPos());
        event->ignore();
        return;
    }

    //  Выделение линии
    if (event->buttons() & Qt::LeftButton) {
        _is_moving = true;
        setCursor(QCursor(Qt::ClosedHandCursor));
        QGraphicsItem::mousePressEvent(event);
    }

    return;
}

void CfcLink::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    _is_moving = false;
    QGraphicsItem::mouseReleaseEvent(event);
    if (!shape().contains(event->pos())) event->setAccepted(false);

    return;
}

void CfcLink::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //  Проверка нажатой кнопки мышки
    QPointF position = QPointF((int)event->pos().x(), (int)event->pos().y());
    int selected_line = selectedLine(position);
    if (!(event->buttons() & Qt::LeftButton) || selected_line < 0 || !_is_moving) {
        QGraphicsItem::mouseMoveEvent(event);
        return;
    }

    prepareGeometryChange();
    if (moveLine(selected_line, position)) needUpdate();
    QGraphicsItem::mouseMoveEvent(event);

    return;
}


//===================================================================================================================================================
//	Обработка сигналов меню
//===================================================================================================================================================
void CfcLink::linkAddNode()
{
    //  Добавление узла
    prepareGeometryChange();
    addNode(_menu_point);

    return;
}

void CfcLink::removeNodes()
{
    //  Проверка выделенной связи
    if (!isSelected()) return;

    //  Удаление лишних узлов
    prepareGeometryChange();
    normalize();

    return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QPainterPath CfcLink::path()
{
    //  Проверка исходных данных
    if (!scene())
        return _path;
    if (!_need_update && _path.elementCount() == 0)
        return _path;

    //  Формирование соединения
    QPainterPath _path = QPainterPath();
    _path.moveTo(points().at(0));
    for (int i = 1; i < points().count(); i++)
        _path.lineTo(points().at(i));
    _need_update = false;

    //  Заполнение списка графических элементов
    QList<QGraphicsItem*> items = scene()->items(_path, Qt::IntersectsItemShape, Qt::AscendingOrder);

    //  Заполнение списка пересекаемых линий
    QList<CfcLine> under_lines;
    for (int i = 0; i < items.count(); i++) {
        CfcLink* graph_link = dynamic_cast<CfcLink*>(items.at(i));
        if (!graph_link) continue;
        if (graph_link == this) break;
        under_lines.append(graph_link->lines());
    }

    if (under_lines.isEmpty())
        return _path;

    //  Вывод линии с пересечениями
    QList<CfcLine> lines = CfcBasicLink::lines();
    _path = QPainterPath();
    _path.moveTo(points().at(0));
    for (int i = 0; i < lines.count(); i++) {
        CfcLine line = lines.at(i);

        //  Заполнение точек пересечения
        QList<QPointF> intersections;
        for (int j = 0; j < under_lines.count(); j++) {
            QPointF ipoint;
            if (line.intersects(under_lines.at(j), ipoint)) intersections.append(ipoint);
        }
        sortOnLine(line, intersections);

        qreal sa = 180;
        if (line.type() == CfcLine::HORIZONTAL)  sa = line.begin().x() >= line.end().x() ? 0 : 180;
        if (line.type() == CfcLine::VERTICAL)    sa = line.begin().y() >= line.end().y() ? 270 : 90;
        qreal la = (sa == 0 || sa == 90) ? 180 : -180;
        for (int j = 0; j < intersections.count(); j++) {
            QPointF ip = intersections.at(j);
            int sw = intersection_radius;
            _path.arcTo(QRectF(ip, ip).adjusted(-sw, -sw, sw, sw), sa, la); }
        _path.lineTo(line.end());
        under_lines.append(line);
    }

    return _path;
}

void CfcLink::paintSelected(QPainter* painter)
{
    painter->setBrush(QBrush());
    painter->setPen(QPen(select_color, 0, Qt::DotLine));
    painter->drawPath(shape());
    painter->setPen(QPen(select_color, 0));
    painter->setBrush(QBrush(Qt::white));
    for (int i = 0; i < points().count(); i++)
        painter->drawEllipse(points().at(i), select_shape / 2, select_shape / 2);

    return;
}

bool CfcLink::less(const QPointF& p1, const QPointF& p2) const
{
    if (!_sorting_line) return false;
    if (p1 == p2) return false;

    QPointF v = _sorting_line->end() - _sorting_line->begin();
    bool x = (v.x() >= 0) ? p1.x() <= p2.x() : p1.x() > p2.x();
    bool y = (v.y() >= 0) ? p1.y() <= p2.y() : p1.y() > p2.y();

    return x && y;
}

void CfcLink::sortOnLine(CfcLine& line, QList<QPointF>& list)
{
    _sorting_line = &line;
    std::sort(list.begin(), list.end(), [this](const QPointF& p1, const QPointF& p2) {
        return this->less(p1, p2);
    });
    _sorting_line = nullptr;

    return;
}


