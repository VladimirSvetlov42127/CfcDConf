#include "editor_node.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QUuid>
#include <QGraphicsScene>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>
#include <gui/dialogs/params_dialog.h>


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorNode::EditorNode(QString ID, QGraphicsItem* parent) : QGraphicsObject(parent), EditorNodeData(ID)
{
	//  Инициализация меню
	_param_menu = new QMenu;
	_param_menu->addAction("Параметры", this, &EditorNode::EditNode);

	//	Свойства класса
	_size = QSizeF();

	//	Установк флагов
	setFlag(QGraphicsItem::ItemIsSelectable);
	setFlag(QGraphicsItem::ItemIsFocusable);
	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setZValue(200);
}

EditorNode::~EditorNode()
{
	if (_param_menu) delete _param_menu;
	if (!_sockets.isEmpty()) {
		for (int i = 0; i < _sockets.count(); i++) delete _sockets.at(i);
		_sockets.clear(); }
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void EditorNode::paintSelected(QPainter* painter)
{
	//	Сохранение параметров рисования
	QRectF rectangle = boundingRect();
	painter->save();

	//	Вывод прямоугольника
	painter->setPen(QPen(FlexLogic::select_color, 0, Qt::DotLine));
	painter->setBrush(QBrush());
	painter->drawRect(rectangle.x() + FlexLogic::select_box / 2, rectangle.y() + FlexLogic::select_box / 2,
		rectangle.width() - FlexLogic::select_box, rectangle.height() - FlexLogic::select_box);

	//	Вывод узловых точек
	painter->setPen(QPen(FlexLogic::select_color, 0, Qt::SolidLine));
	painter->setBrush(QBrush(Qt::white));
	uint8_t select_box = FlexLogic::select_box;
	painter->drawRect(rectangle.x(), rectangle.y(), select_box, select_box);
	painter->drawRect(rectangle.x() + rectangle.width() / 2 - select_box / 2, rectangle.y(), select_box, select_box);
	painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y(), select_box, select_box);
	painter->drawRect(rectangle.x(), rectangle.y() + rectangle.height() / 2 - select_box / 2, select_box, select_box);
	painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y() + rectangle.height() / 2 - select_box / 2, select_box, select_box);
	painter->drawRect(rectangle.x(), rectangle.y() + rectangle.height() - select_box, select_box, select_box);
	painter->drawRect(rectangle.x() + rectangle.width() - select_box, rectangle.y() + rectangle.height() - select_box, select_box, select_box);
	painter->drawRect(rectangle.x() + rectangle.width() / 2 - select_box / 2, rectangle.y() + rectangle.height() - select_box, select_box, select_box);

	//	Восстановление параметров рисования
	painter->restore();

	return;
}

void EditorNode::InitSockets()
{
	if (HasOutput()) { 
		EditorSocket* socket = new EditorSocket(FlexLogic::SocketType::OUTPUT_SOCKET, Inversion(), this);
		socket->setPos(QPointF(NodeSize().width(), NodeSize().height() / 2));
		_sockets.append(socket); }
	if (MinInputs() < 1) return;

	float delta = AvailableRect().height() / (MinInputs() + 1);
	for (int i = 0; i < MinInputs(); i++) {
		EditorSocket* socket = new EditorSocket(FlexLogic::SocketType::INPUT_SOCKET, Inversion(), this);
		socket->setPos(QPointF(AvailableRect().x(), AvailableRect().y() + delta * (i + 1))); 
		_sockets.append(socket); }

	return;
}

void EditorNode::AddInput()
{
	//	Проверка количества сокетов
	int count = _sockets.count();
	if (HasOutput()) count--;
	if (count == MaxInputs()) return;

	//	Изменение размера элемента
	count++;
	float delta = AvailableRect().height() / count;
	QSizeF new_size = QSizeF(NodeSize().width(), NodeSize().height() + delta);
	SetNodeSize(new_size);

	//	Добавление входного сокета
	EditorSocket* socket = new EditorSocket(FlexLogic::SocketType::INPUT_SOCKET, Inversion(), this);
	socket->setPos(QPointF(AvailableRect().x(), AvailableRect().y() + delta * count));
	_sockets.append(socket);

	//	Изменение положения выходного сокета
	if (!HasOutput()) return;
	if (count < 1) return;
	for (int i = 0; i < count; i++) {
		EditorSocket* socket = Sockets().at(i);
		if (socket->SocketType() == FlexLogic::SocketType::INPUT_SOCKET) continue;
		socket->setPos(QPointF(AvailableRect().width(), NodeSize().height() / 2));
		return;	}

	return;
}

void EditorNode::RemoveInput()
{
	//	Проверка количества сокетов
	int count = _sockets.count();
	if (HasOutput()) count--;
	if (count == MinInputs()) return;

	//	Индекс последнего входа
	int index = 0;
	for (int i = 0; i < Sockets().count(); i++) if (Sockets().at(i)->SocketType() == FlexLogic::SocketType::INPUT_SOCKET) index++;

	//	Проверка возможности удаления сокета
	if (Sockets().at(index)->SocketLinks().count() > 0) return;

	//	Изменение размера элемента
	float delta = AvailableRect().height() / (count + 1);
	QSizeF new_size = QSizeF(NodeSize().width(), NodeSize().height() - delta);
	SetNodeSize(new_size);
	count--;

	//	Удаление последнего входа
	EditorSocket* socket = Sockets().at(index);
	delete socket;
	_sockets.removeAt(index);

	//	Изменение положения выходного сокета
	if (!HasOutput()) return;
	if (count < 1) return;
	for (int i = 0; i < count; i++) {
		EditorSocket* socket = Sockets().at(i);
		if (socket->SocketType() == FlexLogic::SocketType::INPUT_SOCKET) continue;
		socket->setPos(QPointF(AvailableRect().width(), NodeSize().height() / 2));
		return;	}

	return;
}

void EditorNode::SetInputs(int inputs)
{
	//	Проверка входных данных
	if (inputs == 0 || inputs == MinInputs()) return;

	//	Очистка сокетов
	for (int i = 0; i < _sockets.count(); i++) _sockets.at(i)->deleteLater();
	_sockets.clear();
	
	//	Добавление выходного сокета
	if (HasOutput()) {
		EditorSocket* socket = new EditorSocket(FlexLogic::SocketType::OUTPUT_SOCKET, Inversion(), this);
		socket->setPos(QPointF(NodeSize().width(), NodeSize().height() / 2));
		_sockets.append(socket); }
	
	//	Добавление входных сокетов
	float delta = NodeSize().height() / (inputs + 1);
	for (int i = 0; i < inputs; i++) {
		EditorSocket* socket = new EditorSocket(FlexLogic::SocketType::INPUT_SOCKET, Inversion(), this);
		socket->setPos(QPointF(AvailableRect().x(), AvailableRect().y() + delta * (i + 1)));
		_sockets.append(socket); }

	return;
}


//===============================================================================================================================================
//	Методы обработки сигналов
//===============================================================================================================================================
void EditorNode::EditNode()
{
	//	Проверка свойств
	if (ParamsList().count() < 1) return;

	//	Редактирование параметров элемента
    ParamsDialog dialog(ParamsList());
    dialog.exec();
    if (dialog.result() != QDialog::Accepted) return;
    QList<FlexLogic::NodeParam> params = dialog.ParamsList();
    for (int i = 0; i < params.count(); i++) setParam(params.at(i).index, params.at(i).value);
    update();

	return;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void EditorNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    paintElement(painter);
	if (isSelected()) paintSelected(painter);
}

QRectF EditorNode::boundingRect() const
{
	QRectF rectangle = AvailableRect();
	int x_margin = FlexLogic::select_box + FlexLogic::select_shape;
	int y_margin = FlexLogic::select_box + FlexLogic::select_shape / 2;

	return rectangle.adjusted(-x_margin, -y_margin, x_margin, y_margin);
}

//===================================================================================================================================================
//	Обработка сигналов мышки
//===================================================================================================================================================
void EditorNode::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF position = QPointF((int)event->pos().x(), (int)event->pos().y());
	//  Проверка области срабатывания мышки
	if (!shape().contains(event->pos())) {
		event->setAccepted(false);
		return;	}

	//  Вызов меню
	if (event->buttons() & Qt::RightButton) {
		if (ParamsList().count() < 1) return;
        if (name() != "BI" && name() != "BO") _param_menu->exec(event->screenPos());
		return;	}
	event->setAccepted(true);

	return;
}
