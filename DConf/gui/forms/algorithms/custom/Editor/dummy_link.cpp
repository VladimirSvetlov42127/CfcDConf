#include "dummy_link.h"


//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DummyLink::DummyLink(EditorSocket* socket, QGraphicsItem* parent) : QGraphicsObject(parent)
{
	_type = socket->SocketType();
	_socket_id = socket->ID();
	_start_point = socket->scenePos();
	_end_point = QPointF();
	_mid_point = QPointF();
	_polygon = QPolygonF();
	_link_error = QString();
}

DummyLink::~DummyLink()
{
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void DummyLink::SetPoint(QPointF point)
{
	_end_point = point;
	_mid_point = (_end_point + _start_point) / 2;

	_polygon.clear();
	_polygon.append(_start_point);
	_polygon.append(QPointF(_mid_point.x(), _start_point.y()));
	_polygon.append(QPointF(_mid_point.x(), _end_point.y()));
	_polygon.append(QPointF(_end_point));
	update();

	return;
}

bool DummyLink::CheckSocket(EditorSocket* socket)
{
	_link_error = QString();
	if (!socket) return false;
	FlexLogic::SocketType socket_type = socket->SocketType();
	int link_count = socket->SocketLinks().count();

	//	Проверка входов
	if (socket_type == FlexLogic::SocketType::INPUT_SOCKET && link_count > 0) { 
		_link_error = QString("Данный вход уже имеет связь с другим элементом.");
		return false; }
	if (socket_type == _type) { 
		_link_error = QString("Ошибка соединения однотипных сокетов.");
		return false; }

	return true;
}


//===================================================================================================================================================
//	Виртуальные методы класса
//===================================================================================================================================================
QRectF DummyLink::boundingRect() const
{
	if (_end_point == QPointF()) return QRectF(QPointF(0, 0), QSize(FlexLogic::select_box, FlexLogic::select_box));
	return _polygon.boundingRect().adjusted(-FlexLogic::select_shape, -FlexLogic::select_shape,
		FlexLogic::select_shape, FlexLogic::select_shape);
}

void DummyLink::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	//  Настройка вывода
	painter->save();
	painter->setPen(QPen(FlexLogic::select_color, 1, Qt::DotLine));
	painter->setRenderHint(QPainter::Antialiasing);
	painter->setBrush(QBrush());

	//	Начало вывода
	if (_end_point == QPointF()) {
		painter->drawEllipse(_start_point, FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);
		painter->restore();
		return;	}

	//  Вывод ломаной линии
	painter->drawLine(_start_point, QPointF(_mid_point.x(), _start_point.y()));
	painter->drawLine(QPointF(_mid_point.x(), _start_point.y()), QPointF(_mid_point.x(), _end_point.y()));
	painter->drawLine(QPointF(_mid_point.x(), _end_point.y()), _end_point);

	//	Вывод точек
	painter->setPen(QPen(FlexLogic::select_color, 0));
	painter->setBrush(QBrush(Qt::white));
	painter->drawEllipse(_start_point, FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);
	painter->drawEllipse(QPointF(_mid_point.x(), _start_point.y()), FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);
	painter->drawEllipse(QPointF(_mid_point.x(), _end_point.y()), FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);
	painter->drawEllipse(_end_point, FlexLogic::select_shape / 2, FlexLogic::select_shape / 2);
	painter->restore();

	return;
}
