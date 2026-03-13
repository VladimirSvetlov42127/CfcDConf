#include "graphic_path.h"


//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
GraphicPath::GraphicPath(QList<QPointF> points)
{
	_points = points;
}

GraphicPath::~GraphicPath()
{
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
QList<GraphicLine> GraphicPath::graphicLines()
{
	QList<GraphicLine> lines;
	int count = points().count() - 1;
	for (int i = 0; i < count; i++) lines.append(GraphicLine(points().at(i), points().at(i + 1)));

	return lines;
}

void GraphicPath::normalize()
{
	QList<QPointF> delete_list;
	QList<GraphicLine> lines = graphicLines();

	//	Удаление лишних точек
	int count = lines.count() - 1;
	for (int i = 0; i < count; i++) if (lines.at(i).type() == lines.at(i + 1).type()) delete_list.append(lines.at(i).end());
	if (!delete_list.isEmpty()) for (int i = 0; i < delete_list.count(); i++) _points.removeOne(delete_list.at(i));

	return;
}

bool GraphicPath::addNode(const QPointF& node)
{
	//	Проверка исходных данных
	if (node == QPointF()) return false;

	//	Перебор узлов
	int count = points().count() - 1;
	for (int i = 0; i < count; i++) {
		if (!isOnLine(points().at(i), points().at(i + 1), node)) continue;
		QPointF new_node = node;
		if (points().at(i).x() == points().at(i + 1).x()) new_node.setX(points().at(i).x());
		if (points().at(i).y() == points().at(i + 1).y()) new_node.setY(points().at(i).y());
		_points.insert(i + 1, new_node);
		return true; }

	return false;
}

int GraphicPath::selectedLine(const QPointF& point)
{
	QList<GraphicLine> lines = graphicLines();
	for (int i = 0; i < lines.count(); i++) if (isOnLine(lines.at(i).begin(), lines.at(i).end(), point)) return i;
	return -1;
}

bool GraphicPath::moveLine(int& line, const QPointF& point)
{
	QList<GraphicLine> lines = graphicLines();
	if (line < 1 || line == lines.count() - 1) return false;
	GraphicLine moving_line = lines.at(line);
	QList<QPointF> old_points = points();
	if (moving_line.type() == FlexLogic::LineTypes::VERTICAL) {
		if (moving_line.begin().x() == point.x()) return false;
		_points[line].setX(point.x());
		_points[line + 1].setX(point.x()); }

	if (moving_line.type() == FlexLogic::LineTypes::HORIZONTAL) {
		if (moving_line.begin().y() == point.y()) return false;
		_points[line].setY(point.y());
		_points[line + 1].setY(point.y()); }
	arrangePoints(old_points);
	line = selectedLine(point);

	return true;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
bool GraphicPath::isOnLine(const QPointF& begin, const QPointF& end, const QPointF& node)
{
	//	Проверка точки на пределы
	qreal min_x = begin.x();
	if (end.x() < min_x) min_x = end.x();
	qreal max_x = begin.x();
	if (end.x() > max_x) max_x = end.x();

	qreal min_y = begin.y();
	if (end.y() < min_y) min_y = end.y();
	qreal max_y = begin.y();
	if (end.y() > max_y) max_y = end.y();

	if (node.y() < min_y - FlexLogic::delta || node.y() > max_y + FlexLogic::delta) return false;
	if (node.x() < min_x - FlexLogic::delta || node.x() > max_x + FlexLogic::delta) return false;

	return true;
}

void GraphicPath::arrangePoints(QList<QPointF> old_points)
{
	QList<QPointF> new_points = points();
	QList<GraphicLine> current_lines = graphicLines();
	for (int i = 0; i < current_lines.count(); i++) {
		if (current_lines.at(i).type() != FlexLogic::LineTypes::NONE) continue;
		if (current_lines.at(i).end() != old_points.at(i + 1)) new_points.insert(i + 1, old_points.at(i + 1));
		if (current_lines.at(i).begin() != old_points.at(i)) new_points.insert(i + 1, old_points.at(i));
		break; }

	setPoints(new_points);

	return;
}

