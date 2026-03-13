#include "editor_scene.h"


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
#include <QMimeData>
#include <QDebug>
#include <QGraphicsView>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/dialogs/params_dialog.h>

#include <gui/forms/algorithms/custom/Nodes/and_node.h>
#include <gui/forms/algorithms/custom/Nodes/bi_node.h>
#include <gui/forms/algorithms/custom/Nodes/bo_node.h>
#include <gui/forms/algorithms/custom/Nodes/generator_node.h>
#include <gui/forms/algorithms/custom/Nodes/impulse_pf_node.h>
#include <gui/forms/algorithms/custom/Nodes/impulse_zf_node.h>
#include <gui/forms/algorithms/custom/Nodes/not_node.h>
#include <gui/forms/algorithms/custom/Nodes/or_node.h>
#include <gui/forms/algorithms/custom/Nodes/return_delay_node.h>
#include <gui/forms/algorithms/custom/Nodes/rs_trigger_node.h>
#include <gui/forms/algorithms/custom/Nodes/trigger_delay_node.h>
#include <gui/forms/algorithms/custom/Nodes/xor_node.h>
#include "gui/dialogs/binding_dialog.h"

//===================================================================================================================================================
//	Константы класса
//===================================================================================================================================================
#define MAX_NODES 255
#define MAX_INPUTS 32


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
EditorScene::EditorScene(CfcAlgService *cfcAlg, ServiceManager *smanager, QGraphicsScene* parent)
    : QGraphicsScene(parent)
    , _service_manager{ smanager }
    , _cfcAlg{cfcAlg}
{
	//	Свойства класса
	_status = FlexLogic::SceneStatus::IS_NOTHING;
	_menu = nullptr;
	_editor_link = nullptr;
    _new_link = nullptr;
	_menu_point = QPointF();

	//  Инициализация меню
    _add_signal_menu = new QMenu;
    _add_signal_menu->addAction("Привязать сигнал", this, &EditorScene::AddSignal);
    _remove_signal_menu = new QMenu;
    _remove_signal_menu->addAction("Отвязать сигнал", this, &EditorScene::removeSignal);

	//	Настройка сцены
	setItemIndexMethod(QGraphicsScene::NoIndex);
	setBackgroundBrush(FlexLogic::scene_bkcolor);
}

EditorScene::~EditorScene()
{
	if (_new_link) delete _new_link;
	if (_editor_link) delete _editor_link;
    if (_add_signal_menu) delete _add_signal_menu;
    if (_remove_signal_menu) delete _remove_signal_menu;
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
EditorNode* EditorScene::NewEditorNode(QString type, QString ID, QSizeF node_size)
{
	if (type == "And") return new And(ID, node_size);
    if (type == "BI") {
        auto input = cfcAlg()->makeInput();
        if (!input)
            return nullptr;

        return new BI(QString(), node_size, input);
    }

    if (type == "BO") {
        auto output = cfcAlg()->makeOutput();
        if (!output)
            return nullptr;

        return new BO(QString(), node_size, output);
    }

    if (type == "Generator") return new Generator(ID, node_size);
	if (type == "ImpulsePF") return new ImpulsePF(ID, node_size);
	if (type == "ImpulseZF") return new ImpulseZF(ID, node_size);
	if (type == "Not") return new Not(ID, node_size);
	if (type == "Or") return new Or(ID, node_size);
	if (type == "ReturnDelay") return new ReturnDelay(ID, node_size);
	if (type == "RsTrigger") return new RsTrigger(ID, node_size);
	if (type == "TriggerDelay") return new TriggerDelay(ID, node_size);
	if (type == "Xor") return new Xor(ID, node_size);

	return nullptr;
}

QList<EditorNode*> EditorScene::Nodes()
{
	QList<EditorNode*> list;
	int count = items().count();
	for (int i = 0; i < count; i++) {
		EditorNode* node = dynamic_cast<EditorNode*>(items().at(i));
		if (!node) continue;
		list.append(node); }

	return list;
}

QList<EditorLink*> EditorScene::Links()
{
	QList<EditorLink*> list;
	int count = items().count();
	for (int i = 0; i < count; i++) {
		EditorLink* link = dynamic_cast<EditorLink*>(items().at(i));
		if (!link) continue;
		list.append(link); }

	return list;
}

QList<EditorNode*> EditorScene::SelectedNodes()
{
	QList<EditorNode*> list;
	int count = items().count();
	for (int i = 0; i < count; i++) {
		EditorNode* node = dynamic_cast<EditorNode*>(items().at(i));
		if (!node) continue;
		if (!node->IsSelected()) continue;
		list.append(node); }

	return list;
}

QList<EditorLink*> EditorScene::SelectedLinks()
{
	QList<EditorLink*> list;
	int count = items().count();
	for (int i = 0; i < count; i++) {
		EditorLink* link = dynamic_cast<EditorLink*>(items().at(i));
		if (!link) continue;
		if (!link->isSelected()) continue;
		list.append(link); }

	return list;
}

QPointF EditorScene::SocketPosition(QString id, QList<EditorNode*> nodes)
{
	if (nodes.isEmpty()) nodes = Nodes();
	for (int i = 0; i < nodes.count(); i++) for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) if (nodes.at(i)->SocketID(ii) == id)
		return nodes.at(i)->Sockets().at(ii)->scenePos();
	return QPointF();
}

void EditorScene::Clear()
{
	QList<EditorNode*> nodes = Nodes();
	QList<EditorLink*> links = Links();
	for (int i = 0; i < nodes.count(); i++) {
		removeItem(nodes.at(i));
		delete nodes.at(i);	}
	for (int i = 0; i < links.count(); i++) {
		removeItem(links.at(i));
		delete links.at(i); }

	return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QList<FlexLogic::MemoryLink> EditorScene::MemoryLinks(QList<EditorNode*> selected_nodes, QList<EditorLink*> selected_links)
{
	//	Формирование списка выделенных связей
	QList<FlexLogic::MemoryLink> memory_links;
	if (selected_nodes.isEmpty() || selected_links.isEmpty()) return memory_links;
	for (int i = 0; i < selected_links.count(); i++) {
		FlexLogic::MemoryLink new_link = { QPointF(), QList<QPointF>(), -1, -1, -1, -1 };
		QString source_id = selected_links.at(i)->SourceID();
		QString target_id = selected_links.at(i)->TargetID();

		//	Перебор по выделенным элементам
		for (int node_idx = 0; node_idx < selected_nodes.count(); node_idx++) {
			for (int ii = 0; ii < selected_nodes.at(node_idx)->SocketsCount(); ii++) {
				if (selected_nodes.at(node_idx)->Sockets().at(ii)->ID() == source_id) {
					new_link.from_node = node_idx;
					new_link.from_socket = ii; }
				if (selected_nodes.at(node_idx)->Sockets().at(ii)->ID() == target_id) {
					new_link.to_node = node_idx;
					new_link.to_socket = ii; }
			}
		}

		//	Проверка связей
		if (new_link.from_node < 0 || new_link.from_socket < 0 || new_link.to_node < 0 || new_link.to_socket < 0) continue;
		new_link.position = selected_links.at(i)->scenePos();
        new_link.points = selected_links.at(i)->points();
		memory_links.append(new_link);
	}

	return memory_links;
}

EditorLink* EditorScene::CreateLink(DummyLink* dummy_link, EditorSocket* socket)
{
	QPointF first_point = dummy_link->StartPoint();
	QPointF last_point = socket->scenePos();
	QPointF mid_point = (first_point + last_point) / 2;

	//	Список точек соединения
	QList<QPointF> points_list;
	points_list.append(first_point);
	points_list.append(QPointF(mid_point.x(), first_point.y()));
	points_list.append(QPointF(mid_point.x(), last_point.y()));
	points_list.append(QPointF(last_point));

	//	Создание соединения
	EditorLink* link = new EditorLink(points_list);

	//	Параметры соединения
	FlexLogic::SocketType type = dummy_link->Type();
	QString first_id = dummy_link->SocketID();
	QString last_id = socket->ID();
	if (type == FlexLogic::SocketType::OUTPUT_SOCKET) {
		link->SetSource(first_id);
		link->SetTarget(last_id);
	} else {
		link->SetSource(last_id);
		link->SetTarget(first_id); }

	//	Добавление связей к сокетам
	QList<EditorNode*> nodes = Nodes();
	for (int i = 0; i < nodes.count(); i++) {
		for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) {
			EditorSocket* socket = nodes.at(i)->Sockets().at(ii);
			QString socket_id = socket->ID();
			if (socket_id == first_id || socket_id == last_id) socket->AppendLink(link->ID()); }
	}
	emit DataChanged(true);

	return link;
}

void EditorScene::Move(QPointF delta)
{
	QList<EditorNode*> nodes = SelectedNodes();
	QList<EditorLink*> links = SelectedLinks();
	for (int i = 0; i < nodes.count(); i++) nodes.at(i)->setPos(nodes.at(i)->pos() + delta);
	for (int i = 0; i < links.count(); i++) { 
		QList<QPointF> new_points;
        QList<QPointF> points = links.at(i)->points();
		for (int ii = 0; ii < points.count(); ii++) new_points.append(QPointF(points.at(ii) + delta));
        links.at(i)->setPoints(new_points);
        links.at(i)->needUpdate(); }

	//	Выравнивание связей
	nodes = Nodes();
	links = Links();
	for (int i = 0; i < links.count(); i++) {
		QString source_id = links.at(i)->SourceID();
		QPointF source_position = SocketPosition(source_id);
        if (source_position != links.at(i)->firstPoint()) {
            links.at(i)->changeSource(source_position);
            links.at(i)->needUpdate(); }
	}

	for (int i = 0; i < links.count(); i++) {
		QString target_id = links.at(i)->TargetID();
		QPointF target_position = SocketPosition(target_id);
        if (target_position != links.at(i)->lastPoint()) {
            links.at(i)->changeTarget(target_position);
            links.at(i)->needUpdate(); }
	}

	return;
}

void EditorScene::DeleteSignal(EditorNode* node)
{
    BO* bo_node = dynamic_cast<BO*>(node);
    BI* bi_node = dynamic_cast<BI*>(node);

    if (bo_node) {
        auto output = bo_node->cfcOutput();
        cfcAlg()->removeOutput(output);
        return;
    }
    if (bi_node) {
        auto input = bi_node->cfcInput();
        cfcAlg()->removeInput(input);
        return;
    }

    return;
}


//===================================================================================================================================================
//	Методы обработки сигналов
//===================================================================================================================================================
void EditorScene::EditNode()
{
	//	Проверка свойств
	EditorNode* node = dynamic_cast<EditorNode*>(itemAt(_menu_point, QTransform()));
	if (!node) return;
	if (node->ParamsList().count() < 1) return;

	//	Редактирование параметров элемента
    ParamsDialog dialog(node->ParamsList());
    dialog.exec();
    if (dialog.result() != QDialog::Accepted) return;
    QList<FlexLogic::NodeParam> params = dialog.ParamsList();
    for (int i = 0; i < params.count(); i++) node->setParam(params.at(i).index, params.at(i).value);
    node->update();

	return;
}

void EditorScene::AddSignal()
{
	//	Проверка свойств
	EditorNode* node = dynamic_cast<EditorNode*>(itemAt(_menu_point, QTransform()));
	if (!node) return;
    if (node->name() != "BI" && node->name() != "BO") return;

	//	Добавление входного сигнала
    if (node->name() == "BI") {
        BI* bi_node =static_cast<BI*>(node);
        if (!bi_node)
            return;

        BindingDialog dialog(BindingDialog::TYPE_INPUT, serviceManager());
        if (dialog.exec() != QDialog::Accepted)
            return;

        InputSignal* input_signal = dialog.selectedSignal();
        bi_node->cfcInput()->setSource(input_signal);
        return;
	}

	//	Добавление выходного сигнала
    if (node->name() == "BO") {
        BO* bo_node =static_cast<BO*>(node);
        if (!bo_node)
            return;

        BindingDialog dialog(BindingDialog::TYPE_OUTPUT, serviceManager());
        if (dialog.exec() != QDialog::Accepted)
            return;

        VirtualInputSignal* output_signal = dynamic_cast<VirtualInputSignal*>(dialog.selectedSignal());
        if (!output_signal)
            return;

        bo_node->cfcOutput()->setTarget(output_signal);
        return;
    }

	return;
}

void EditorScene::removeSignal()
{
    BO* bo_node = dynamic_cast<BO*>(itemAt(_menu_point, QTransform()));
    BI* bi_node = dynamic_cast<BI*>(itemAt(_menu_point, QTransform()));

    if (bo_node) {
        auto output = bo_node->cfcOutput();
        output->setTarget(nullptr);

        return;
    }
    if (bi_node) {
        auto input = bi_node->cfcInput();
        input->setSource(nullptr);
        return;
    }
    return;
}


void EditorScene::Delete()
{
	//	Подготовка исходных данных
	QList<EditorNode*> selected_nodes = SelectedNodes();
	QList<EditorLink*> selected_links = SelectedLinks();
	if (selected_nodes.count() < 1 && selected_links.count() < 1) return;

	//	Заполнение списка связей
	QStringList links_list;
	for (int i = 0; i < selected_links.count(); i++) links_list.append(selected_links.at(i)->ID());
	for (int i = 0; i < selected_nodes.count(); i++) {
		for (int ii = 0; ii < selected_nodes.at(i)->SocketsCount(); ii++) {
			int count = selected_nodes.at(i)->Sockets().at(ii)->SocketLinks().count();
			for (int ll = 0; ll < count; ll++) {
				QString link_id = selected_nodes.at(i)->Sockets().at(ii)->SocketLinks().at(ll);
				if (links_list.indexOf(link_id) < 0) links_list.append(link_id); }
		}
	}

	//	Удаление выделенных элементов
	for (int i = 0; i < selected_links.count(); i++) {
		removeItem(selected_links.at(i));
		delete selected_links.at(i); }
	for (int i = 0; i < selected_nodes.count(); i++) {
		DeleteSignal(selected_nodes.at(i));
		removeItem(selected_nodes.at(i));
		delete selected_nodes.at(i); }

	//	Удаление связей удаленных узлов
	QList<EditorLink*> links = Links();
	for (int i = 0; i < links.count(); i++) {
		QString link_id = links.at(i)->ID();
		int index = links_list.indexOf(link_id);
		if (index < 0) continue;
		removeItem(links.at(i));
		delete links.at(i);	}

	//	Отвязывание удаленных связей
	QList<EditorNode*> nodes = Nodes();
	for (int i = 0; i < nodes.count(); i++) {
		for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) {
			EditorSocket* socket = nodes.at(i)->Sockets().at(ii);
			for (int ll = 0; ll < links_list.count(); ll++) socket->RemoveLink(links_list.at(ll)); }
	}
	emit DataChanged(true);

	return;
}

void EditorScene::Copy(bool select)
{
	//	Подготовка исходных данных
	QList<EditorNode*> selected_nodes = SelectedNodes();
	if (selected_nodes.count() < 1) return;
	QList<EditorLink*> selected_links = SelectedLinks();

	//	Формирование списка выделенных связей
	QList<FlexLogic::MemoryLink> memory_links = MemoryLinks(selected_nodes, selected_links);

	//	Заполнение списка выделенных узлов
	QList<FlexLogic::MemoryNode> memory_nodes;
	for (int i = 0; i < selected_nodes.count(); i++) {
		FlexLogic::MemoryNode memory_node;
		memory_node.position = selected_nodes.at(i)->scenePos();
        memory_node.type = selected_nodes.at(i)->name();
		for (int ii = 0; ii < selected_nodes.at(i)->ParamsList().count(); ii++) {
			QVariant value = selected_nodes.at(i)->ParamsList().at(ii).value;
			memory_node.params.append(value); }
		for (int ii = 0; ii < selected_nodes.at(i)->SocketsCount(); ii++) {
			FlexLogic::MemorySocket memory_socket;
			memory_node.sockets.append(memory_socket); }
		memory_nodes.append(memory_node); }

	//	Привязка связей к узлам
	for (int i = 0; i < memory_links.count(); i++) {
		int node = memory_links.at(i).from_node;
		int socket = memory_links.at(i).from_socket;
		memory_nodes[node].sockets[socket].links.append(i);
		node = memory_links.at(i).to_node;
		socket = memory_links.at(i).to_socket;
		memory_nodes[node].sockets[socket].links.append(i);	}

	//	Фиксация переменных
	for (int i = 0; i < selected_nodes.count(); i++) selected_nodes.at(i)->setSelected(select);
	for (int i = 0; i < selected_links.count(); i++) selected_links.at(i)->setSelected(select);
	_buffer_links.clear();
	_buffer_nodes.clear();
	if (!memory_links.isEmpty()) _buffer_links = memory_links;
	_buffer_nodes = memory_nodes;
	emit BufferChanged(true);

	return;
}

void EditorScene::Cut()
{
	Copy(true);
	Delete();
	emit BufferChanged(true);
	emit DataChanged(true);

	return;
}

void EditorScene::Paste()
{
	//	Размер смещения
	QPointF shift = QPoint(100, 100);
	if (_menu_point != QPointF()) shift = _menu_point;

	//	Проверка данных
	if ((Nodes().size() + _buffer_nodes.size()) > MAX_NODES) {
		Dpc::Gui::MsgBox::error(QString("Количество элементов превышает максимум."));
		return; }
	uint8_t inputs_count = 0;
    for (int i = 0; i < Nodes().size(); i++) if (Nodes().at(i)->nodeType() == FlexLogic::RZA_LOAD) inputs_count++;
	for (int i = 0; i < _buffer_nodes.count(); i++) if (_buffer_nodes.at(i).type == "BI" || _buffer_nodes.at(i).type == "BO") inputs_count++;
	if (inputs_count > MAX_INPUTS) {
		Dpc::Gui::MsgBox::error(QString("Количество входов и выходов превышает максимум."));
		return; }

	//	Формирование списка узлов
	QList<EditorNode*> nodes;
	for (int i = 0; i < _buffer_nodes.count(); i++) {
		QString type = _buffer_nodes.at(i).type;
		EditorNode* node = NewEditorNode(type);
		node->setSelected(true);
		node->setPos(_buffer_nodes.at(i).position + shift);
		if (type != "BO" && type != "BI") {
			int count = _buffer_nodes.at(i).params.count();
            for (int ii = 0; ii < count; ii++) node->setParam(ii, _buffer_nodes.at(i).params.at(ii)); }
		int sockets = _buffer_nodes.at(i).sockets.count();
		int delta = sockets - node->SocketsCount();
		for (int ii = 0; ii < delta; ii++) node->AddInput();
		nodes.append(node);	}

	//	Формирование списка связей
	QList<EditorLink*> links;
	for (int i = 0; i < _buffer_links.count(); i++) {
		QList<QPointF> points; 
		for (int ii = 0; ii < _buffer_links.at(i).points.count(); ii++) points.append(_buffer_links.at(i).points.at(ii) + shift);
		EditorLink* link = new EditorLink(points);
		link->setSelected(true);
		int node = _buffer_links.at(i).from_node;
		int socket = _buffer_links.at(i).from_socket;
		link->SetSource(nodes.at(node)->Sockets().at(socket)->ID());
		node = _buffer_links.at(i).to_node;
		socket = _buffer_links.at(i).to_socket;
		link->SetTarget(nodes.at(node)->Sockets().at(socket)->ID());
		links.append(link);	}

	//	Привязка элементов
	for (int i = 0; i < _buffer_nodes.count(); i++) {
		for (int ii = 0; ii < _buffer_nodes.at(i).sockets.count(); ii++) {
			for (int idx = 0; idx < _buffer_nodes.at(i).sockets.at(ii).links.count(); idx++) {
				int link_index = _buffer_nodes.at(i).sockets.at(ii).links.at(idx);
				QString link_id = links.at(link_index)->ID();
				nodes.at(i)->Sockets().at(ii)->AppendLink(link_id);	}
		}
	}

	//	Вывод элементов на сцену
	for (int i = 0; i < nodes.count(); i++) addItem(nodes.at(i));  
	for (int i = 0; i < links.count(); i++) addItem(links.at(i));

	//	Очистка буфера
	_buffer_links.clear();
	_buffer_nodes.clear();
	emit BufferChanged(true);
	emit DataChanged(true);

	return;
}

void EditorScene::AddInputs()
{
	QList<EditorNode*> nodes = SelectedNodes();
	if (nodes.isEmpty()) return;

	QList<EditorLink*> links = Links();
	int links_count = links.count();
	for (int i = 0; i < nodes.count(); i++) {
		nodes.at(i)->AddInput();
		for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) {
			if (nodes.at(i)->Sockets().at(ii)->SocketType() != FlexLogic::SocketType::OUTPUT_SOCKET) continue;
			QPointF position = nodes.at(i)->Sockets().at(ii)->scenePos();
			int count = nodes.at(i)->Sockets().at(ii)->SocketLinks().count();
			for (int ll = 0; ll < count; ll++) {
				QString link_id = nodes.at(i)->Sockets().at(ii)->SocketLinks().at(ll);
                for (int nn = 0; nn < links_count; nn++) if (links.at(nn)->ID() == link_id) links.at(nn)->changeSource(position); }
		}
	}
	emit DataChanged(true);

	return;
}

void EditorScene::RemoveInputs()
{
	QList<EditorNode*> nodes = SelectedNodes();
	if (nodes.isEmpty()) return;

	QList<EditorLink*> links = Links();
	int links_count = links.count();
	for (int i = 0; i < nodes.count(); i++) {
		nodes.at(i)->RemoveInput();
		for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) {
			if (nodes.at(i)->Sockets().at(ii)->SocketType() != FlexLogic::SocketType::OUTPUT_SOCKET) continue;
			QPointF position = nodes.at(i)->Sockets().at(ii)->scenePos();
			int count = nodes.at(i)->Sockets().at(ii)->SocketLinks().count();
			for (int ll = 0; ll < count; ll++) {
				QString link_id = nodes.at(i)->Sockets().at(ii)->SocketLinks().at(ll);
                for (int nn = 0; nn < links_count; nn++) if (links.at(nn)->ID() == link_id) links.at(nn)->changeSource(position); }
		}
	}
	emit DataChanged(true);

	return;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void EditorScene::drawBackground(QPainter* painter, const QRectF& rect)
{
	if (!FlexLogic::grid_enable) return;

	//  Определение переменных
	int step = FlexLogic::grid_step;
	int small_step = FlexLogic::grid_small_step;

	int left = int(sceneRect().left());
	int right = int(rect.right());
	int top = int(sceneRect().top());
	int bottom = int(rect.bottom());

	int first_top = top - top % step;
	int first_left = left - left % step;

	//  Крупная сетка
	painter->setPen(QPen(FlexLogic::grid_color, 2));
	for (int x = first_left; x <= right; x = x + step) painter->drawLine(x, top, x, bottom);
	for (int y = first_top; y <= bottom; y = y + step) painter->drawLine(left, y, right, y);

	//  Мелкая сетка
	painter->setPen(QPen(FlexLogic::grid_color, 1));
	for (int y = first_top; y <= bottom; y = y + small_step) painter->drawLine(left, y, right, y);
	for (int x = first_left; x <= right; x = x + small_step) painter->drawLine(x, top, x, bottom);

	return;
}


//===================================================================================================================================================
//	Методы обработки событий мыши
//===================================================================================================================================================
void EditorScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
}

void EditorScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
}

void EditorScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
	const QMimeData* mime = event->mimeData();
	if (!mime->hasFormat("Node")) {
		event->ignore();
		return;	}

	QByteArray data = mime->data("Node");
	QString idx = QString(data);
	clearSelection();

	//	Проверка элементов
	if (Nodes().size() == MAX_NODES) {
		Dpc::Gui::MsgBox::error(QString("Количество элементов превышает максимум."));
		return; }

	uint8_t inputs_count = 0;
    for (int i = 0; i < Nodes().size(); i++) {
        if (Nodes().at(i)->nodeType() == FlexLogic::RZA_LOAD) inputs_count++;
		if (inputs_count > MAX_INPUTS) {
			Dpc::Gui::MsgBox::error(QString("Количество входов и выходов превышает максимум."));
			return;	}
	}

	EditorNode* node = NewEditorNode(idx);
	if (!node) {
		Dpc::Gui::MsgBox::error(QString("Ошибка создания графического элемента %1").arg(idx));
		return;	}
	QSizeF size = node->NodeSize();
	QPointF position = QPointF(event->scenePos().x() - size.width() / 2, event->scenePos().y() - size.height() / 2);
	node->setPos(position);
	node->setSelected(true);
	addItem(node);
	emit DataChanged(true);

	return;
}

void EditorScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF position = QPointF((int)event->scenePos().x(), (int)event->scenePos().y());
    EditorNode* node = dynamic_cast<EditorNode*>(itemAt(position, QTransform()));
    BO* bo_node = dynamic_cast<BO*>(itemAt(position, QTransform()));
    BI* bi_node = dynamic_cast<BI*>(itemAt(position, QTransform()));

	EditorLink* link = dynamic_cast<EditorLink*>(itemAt(position, QTransform()));
	EditorSocket* socket = dynamic_cast<EditorSocket*>(itemAt(position, QTransform()));
	_menu_point = position;

	//	Вызов меню
	if (event->button() == Qt::RightButton) {

        //	Вызов меню привязки сигналов
        if (node->IsSelected() && SelectedNodes().count() == 1 && (bo_node || bi_node)) {
            if (bo_node) {
                auto output = bo_node->cfcOutput();
                output->target() ? _remove_signal_menu->exec(event->screenPos()) : _add_signal_menu->exec(event->screenPos());
            }
            if (bi_node) {
                auto input = bi_node->cfcInput();
                input->source() ? _remove_signal_menu->exec(event->screenPos()) : _add_signal_menu->exec(event->screenPos());
            }
			event->accept();
			_menu_point = QPointF();
            return;
        }

		//	Вызов большого меню
		if (!link && !node && (selectedItems().count() > 0 || !_buffer_nodes.isEmpty())) {
			Menu()->exec(event->screenPos());
			event->accept();
			_menu_point = QPointF();
            return;
        }
	}

	//	Перемещение элементов
	if (event->button() == Qt::LeftButton && node) {
		if (!node->isSelected()) node->setSelected(true);
		_status = FlexLogic::SceneStatus::IS_MOVING;
		views().at(0)->setCursor(Qt::ClosedHandCursor);
		views().at(0)->setDragMode(QGraphicsView::RubberBandDrag);
		_base_point = position; }

	//	Рисование связи
	if (event->button() == Qt::LeftButton && socket) {
		FlexLogic::SocketType type = socket->SocketType();
		if (type == FlexLogic::SocketType::INPUT_SOCKET && socket->SocketLinks().count() > 0) {
			Dpc::Gui::MsgBox::error(QString("Данный вход уже используется."));
			QGraphicsScene::mousePressEvent(event);
			return; }

		//	Создание связи
		_status = FlexLogic::SceneStatus::IS_DRAWING;
		_new_link = new DummyLink(socket);
		addItem(_new_link);
		views().at(0)->setCursor(Qt::CrossCursor);
		views().at(0)->setDragMode(QGraphicsView::NoDrag);
		clearSelection(); }

	QGraphicsScene::mousePressEvent(event);
	return;
}

void EditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	//	Определение позиции мышки
	QPointF position = QPointF((int)event->scenePos().x(), (int)event->scenePos().y());
	QPointF delta = position - _base_point;

	//	Вывод новой связи
	if (_status == FlexLogic::SceneStatus::IS_DRAWING && _new_link != nullptr) {
		_new_link->SetPoint(position);
		EditorSocket* socket = dynamic_cast<EditorSocket*>(itemAt(position, QTransform()));

		bool checked = _new_link->CheckSocket(socket);
		if (!socket) views().at(0)->setCursor(Qt::CrossCursor);
		if (socket && checked == false) views().at(0)->setCursor(Qt::ForbiddenCursor);
		if (socket && checked == true) views().at(0)->setCursor(Qt::DragCopyCursor);
        QGraphicsScene::mouseMoveEvent(event);
        return;	}

	//	Перемещение выделенных элементов
	if (_status == FlexLogic::SceneStatus::IS_MOVING) {
		Move(delta);
		_base_point = _base_point + delta;
		emit DataChanged(true);
        return;
        QGraphicsScene::mouseMoveEvent(event);
    }

	QGraphicsScene::mouseMoveEvent(event);
}

void EditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF position = QPointF((int)event->scenePos().x(), (int)event->scenePos().y());
	views().at(0)->setCursor(Qt::ArrowCursor);
	views().at(0)->setDragMode(QGraphicsView::RubberBandDrag);
	FlexLogic::SceneStatus status = _status;
	_base_point = QPoint();
	_status = FlexLogic::SceneStatus::IS_NOTHING;

	//	Окончание вывода новой связи
	if (status == FlexLogic::SceneStatus::IS_DRAWING) {
		EditorSocket* socket = dynamic_cast<EditorSocket*>(itemAt(position, QTransform()));
		if (!socket) {
			removeItem(_new_link);
			delete _new_link;
			_new_link = nullptr;
			return;	}
		if (!_new_link->CheckSocket(socket)) {
			QString error_text = _new_link->Error();
			Dpc::Gui::MsgBox::error(error_text);
		} else {
			clearSelection();
			_editor_link = CreateLink(_new_link, socket);
			_editor_link->setSelected(true);
			addItem(_editor_link); }

		removeItem(_new_link);
		delete _new_link;
		_new_link = nullptr;
	}

	QGraphicsScene::mouseReleaseEvent(event);
	return;
}
