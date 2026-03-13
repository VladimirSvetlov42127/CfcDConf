#include "editor_link.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QUuid>
#include <QGraphicsScene>
#include <QLineF>


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/Editor/editor_socket.h>
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>
#include <gui/forms/algorithms/custom/Editor/graphic_line.h>

//===================================================================================================================================================
//	Подключение внешних функций
//===================================================================================================================================================
const GraphicLine* gSortingLine = NULL;

bool less(const QPointF& p1, const QPointF& p2)
{
    if (!gSortingLine) return false;
    if (p1 == p2) return false;

    QPointF v = gSortingLine->end() - gSortingLine->begin();
    bool x = (v.x() >= 0) ? p1.x() <= p2.x() : p1.x() > p2.x();
    bool y = (v.y() >= 0) ? p1.y() <= p2.y() : p1.y() > p2.y();
    return x && y;
}

void sortOnLine(const GraphicLine& line, QList<QPointF>& list)
{
    gSortingLine = &line;
    std::sort(list.begin(), list.end(), less);
    gSortingLine = NULL;
}


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorLink::EditorLink(QList<QPointF> points, QString ID, QGraphicsItem* parent) : QGraphicsObject(parent), EditorLinkData(ID)
{
    //	Свойства класса
    _graphic_path = new GraphicPath(points);
    _need_update = true;
    _is_moving = false;
    _selected_line = -1;

    //  Инициализация меню
    _menu_point = QPointF();
    _menu = new QMenu;
    _menu->addAction("Добавить узел", this, &EditorLink::addNode);
    _menu->addAction("Убрать лишние узлы", this, &EditorLink::removeNodes);

    //	Установк флагов
    setFlag(QGraphicsItem::ItemIsSelectable);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

EditorLink::~EditorLink()
{
    if (_graphic_path) delete _graphic_path;
    if (_menu) delete _menu;
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void EditorLink::setPoints(const QList<QPointF> points)
{
    if (points == graphicPath()->points()) return;
    prepareGeometryChange();
    graphicPath()->setPoints(points);
    updatePath();

    return;
}

void EditorLink::changeSource(QPointF point)
{
    QList<QPointF> link_points = points();
    if (link_points.at(0) == point) return;

    link_points[0] = point;
    link_points[1].setY(point.y());
    prepareGeometryChange();
    graphicPath()->setPoints(link_points);
    update();
}

void EditorLink::changeTarget(QPointF point)
{
    QList<QPointF> link_points = points();
    if (link_points.last() == point) return;
    int number = link_points.count() - 1;
    link_points[number] = point;
    link_points[number - 1].setY(point.y());
    prepareGeometryChange();
    graphicPath()->setPoints(link_points);
    update();
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QPainterPath EditorLink::path()
{
    //  Проверка исходных данных
    if (!_need_update) return _path;
    if (!scene()) return _path;

   //  Сздание нового многоугольника
    _need_update = false;
    _path = QPainterPath();
    QList<GraphicLine> lines = graphicPath()->graphicLines();
    if (lines.isEmpty()) return _path;

    //  Заполнение списка графических элементов
    QList<QGraphicsItem*> items;
    QPainterPath default_path;
    default_path.moveTo(graphicPath()->points().first());
    for (int i = 1; i < graphicPath()->points().count(); i++) default_path.lineTo(graphicPath()->points().at(i));
    items = scene()->items(default_path, Qt::IntersectsItemShape, Qt::AscendingOrder); 

    //  Заполнение списка пересекаемых линий
    QList<GraphicLine> under_lines;
    for (int i = 0; i < items.count(); i++) {
        EditorLink* graph_link = dynamic_cast<EditorLink*>(items.at(i));
        if (!graph_link) continue;
        if (graph_link == this) break;
        under_lines.append(graph_link->_graphic_path->graphicLines()); }

    //  Вывод линии с пересечениями
    _path.moveTo(lines.first().begin());
    for (int i = 0; i < lines.count(); i++) {
        GraphicLine line = lines.at(i);

        //  Заполнение точек пересечения
        QList<QPointF> intersections;
        for (int j = 0; j < under_lines.count(); j++) {
            QPointF ipoint;
            if (line.intersects(under_lines.at(j), ipoint)) intersections.append(ipoint); }
        sortOnLine(line, intersections);

        qreal sa = 180;
        if (line.type() == FlexLogic::LineTypes::HORIZONTAL)  sa = line.begin().x() >= line.end().x() ? 0 : 180;
        if (line.type() == FlexLogic::LineTypes::VERTICAL)    sa = line.begin().y() >= line.end().y() ? 270 : 90;
        qreal la = (sa == 0 || sa == 90) ? 180 : -180;
        for (int j = 0; j < intersections.count(); j++) {
            QPointF ip = intersections.at(j);
            int sw = FlexLogic::intersection_radius;
            _path.arcTo(QRectF(ip, ip).adjusted(-sw, -sw, sw, sw), sa, la); }
        _path.lineTo(line.end());
        under_lines.append(line); }

    return _path;
}

void EditorLink::updatePath()
{
    needUpdate();
    if (scene()) {
        QList<QGraphicsItem*> items = scene()->items(shape(), Qt::IntersectsItemShape, Qt::AscendingOrder);
        QList<QGraphicsItem*>::iterator it = items.begin();
        while (it != items.end() && *it != this) it++;
        while (it != items.end()) {
            EditorLink* c = dynamic_cast<EditorLink*>(*it);
            if (c) c->needUpdate();
            it++; }
    }

    return;
}

void EditorLink::paintSelected(QPainter* painter)
{
    painter->setBrush(QBrush());
    painter->setPen(QPen(FlexLogic::select_color, 0, Qt::DotLine));
    painter->drawPath(shape());
    painter->setPen(QPen(FlexLogic::select_color, 0));
    painter->setBrush(QBrush(Qt::white));
    for (int i = 0; i < _graphic_path->points().count(); i++) painter->drawEllipse(_graphic_path->points().at(i),
        FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);

    return;
}


//===================================================================================================================================================
//	Виртуальные методы класса
//===================================================================================================================================================
QRectF EditorLink::boundingRect() const
{
    QList<QPointF> points = _graphic_path->points();
    if (points.isEmpty()) return QRectF();
    QPolygonF polygon;
    for (int i = 0; i < points.count(); i++) polygon.append(points.at(i));
    return polygon.boundingRect().adjusted(-FlexLogic::select_shape, -FlexLogic::select_shape, FlexLogic::select_shape, FlexLogic::select_shape);
}

QPainterPath EditorLink::shape() const
{
    QPainterPathStroker ps;
    ps.setWidth(FlexLogic::select_shape);
    return ps.createStroke(_path);
}

void EditorLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    painter->save();
    painter->setPen(QPen(FlexLogic::connection_color, FlexLogic::connection_width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(path());
    if (isSelected()) paintSelected(painter);
    painter->restore();

    return;
}


//===================================================================================================================================================
//	Обработка сигналов мышки
//===================================================================================================================================================
void EditorLink::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //  Проверка области срабатывания мышки
    QPointF position = QPointF((int)event->pos().x(), (int)event->pos().y());
    if (!shape().contains(event->pos())) {
        event->setAccepted(false);
        return; }
    for (int i = 0; i < graphicPath()->points().count(); i++) {
        QLineF line(position, graphicPath()->points().at(i));
        if (line.length() < FlexLogic::select_shape) {
            event->setAccepted(false);
            return; }
    }
    
    //  Вызов меню
    if (event->buttons() & Qt::RightButton) {
        if (!isSelected()) return;
        _menu_point = position;
        _menu->exec(event->screenPos());
        return; }

    //  Выделение линии
    if (event->buttons() & Qt::LeftButton) {
        setMoving(true);
        _selected_line = graphicPath()->selectedLine(position);
        setCursor(QCursor(Qt::ClosedHandCursor));
        QGraphicsItem::mousePressEvent(event); }

    return;
}

void EditorLink::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    setCursor(QCursor(Qt::ArrowCursor));
    setMoving(false);
    _selected_line = -1;
    QGraphicsItem::mouseReleaseEvent(event);
    if (!shape().contains(event->pos())) event->setAccepted(false);

    return;
}

void EditorLink::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //  Проверка нажатой кнопки мышки
    if (!(event->buttons() & Qt::LeftButton) || selectedLine() < 0 || !isMoving()) {
        QGraphicsItem::mouseMoveEvent(event);
        return; }

    QPointF point = QPointF((int)event->pos().x(), (int)event->pos().y());
    prepareGeometryChange();
    if (graphicPath()->moveLine(_selected_line, point)) needUpdate();
    QGraphicsItem::mouseMoveEvent(event);

    return;
}


//===================================================================================================================================================
//	Обработка сигналов меню
//===================================================================================================================================================
void EditorLink::addNode()
{
    //  Добавление узла
    prepareGeometryChange();
    _graphic_path->addNode(_menu_point);

    return;
}

void EditorLink::removeNodes()
{
    //  Проверка выделенной связи
    if (!isSelected()) return;

    //  Удаление лишних узлов
    prepareGeometryChange();
    _graphic_path->normalize();

    return;
}
