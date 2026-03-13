#include "dep_parser.h"


//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>
#include <QFile>
#include <QPointF>
#include <QTextStream>
#include <QDir>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>
#include <gui/forms/algorithms/custom/Editor/editor_node.h>
#include <gui/forms/algorithms/custom/Editor/editor_link.h>
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

//===================================================================================================================================================
//	Константы класса
//===================================================================================================================================================
#define MAX_PIN_NUMBER 33
#define MAX_NODES 255
#define MAX_INPUTS 32


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DepCfcParser::DepCfcParser(QObject *parent)
    : QObject{parent}
{
	init();
}

//DepCfcParser::DepCfcParser(const QList<EditorNode*>& nodes, const QList<EditorLink*>& links)
//{
//	init();
//	_nodes = nodes;
//	_links = links;
//}

DepCfcParser::~DepCfcParser()
{
    for (int i = 0; i < _nodes.count(); i++)
        delete _nodes.at(i);
    for (int i = 0; i < _links.count(); i++)
        delete _links.at(i);
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void DepCfcParser::init()
{
    // _version = 0;
	_title = QString();
    // _error_list.clear();
	_nodes.clear();
	_links.clear();
    // _width = 1280;
    // _height = 1024;
}

// void DepCfcParser::setCanvas(int width, int height)
// {
// 	_width = width;
// 	_height = height;
// }

bool DepCfcParser::loadFile(const QString& file_name)
{
	//	Проверка файла проекта
	QFile xml_file(file_name);
	if (!xml_file.exists()) {
		QString error_text = "Файл алгоритма не найден.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        return false;
    }

	if (!xml_file.open(QIODevice::ReadOnly)) {
		QString error_text = "Ошибка открытия файла алгоритма.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
		xml_file.close();
        return false;
    }

	//	Получение данных из XML файла
	QDomDocument xml;
    QString content_error = QString("Ошибка обработки файла алгоритма: %1.\n Ошибка: %2, Строка: %3, Колонка %4.");
    QString error_message;
    int line, column;
    if (!xml.setContent(&xml_file, &error_message, &line, &column)) {
        QString error_text = content_error.arg(file_name, error_message, QString::number(line), QString::number(column));
        // _error_list.append(error_text);
        emit errorToLog(error_text);
        xml_file.close();
        return false;
    }
	xml_file.close();

	//	Получение элементов схемы
    // _error_list.clear();
	_nodes = QList<EditorNode*>();
	_links = QList<EditorLink*>();

    return deserialize(xml);
}

bool DepCfcParser::saveFile(const QString& file_name)
{
    //  Создание директории
    QString cfc_path = file_name.left(file_name.lastIndexOf('/'));
    QDir cfc_dir(cfc_path);
    if (!cfc_dir.exists(cfc_path)) cfc_dir.mkdir(cfc_path);

	//	Открытие файла рисунка
    QFile xml_file(file_name);
    if (xml_file.exists())
        xml_file.remove();
    if (!xml_file.open(QIODevice::WriteOnly)) {
		QString error_text = "Ошибка создания файла алгоритма.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        return false;
    }

	//	Проверка корректности схемы
	bool error = validate();

	//	Сохранение данных
    QTextStream stream(&xml_file);
	QDomDocument xml = serialize();
	if (xml_file.write(xml.toByteArray(4)) < 0) { 
		QString error_text = "Ошибка записи файла алгоритма.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        error = false;
    }
	xml_file.close();

	return error;
}

bool DepCfcParser::validate()
{
    // _error_list.clear();
	bool exit = true;
	QString node_text = "Элемент %1 тип %2 : ";
	int input_count = 0, output_count = 0;

	//	Отсутствие элементов на сцене
	emit infoToLog(QString("Проверка алгоритма..."));
	if (_nodes.isEmpty()) {
		QString error_text = "Отсутствие элементов на схеме.";
		emit errorToLog(error_text);
        // _error_list.append(error_text);
        return false;
    }

	//	Проверка сокетов для каждого узла
	for (int i = 0; i < _nodes.count(); i++) {
        QString node_name = _nodes.at(i)->name();
        if (node_name == "BI")
            input_count++;
        if (node_name == "BO")
            output_count++;
        for (int ii = 0; ii < _nodes.at(i)->SocketsCount(); ii++) {
            FlexLogic::SocketType socket_type = _nodes.at(i)->Sockets().at(ii)->SocketType();

			//	Проверка отсутствия соединений
            if (_nodes.at(i)->Sockets().at(ii)->SocketLinks().isEmpty()) {
				QString error_text;
                socket_type == FlexLogic::SocketType::OUTPUT_SOCKET ? error_text = node_text.arg(QString::number(i + 1), node_name) + QString("Отсустствие связей на выходе.") :
                    error_text = node_text.arg(QString::number(i + 1), node_name) + QString("Отсустствие связей на входе ") + QString::number(ii) + QString(".");
				emit errorToLog(error_text);
                // _error_list.append(error_text);
                exit = false;
            }

			//	Несколько соединений на входе
            if (socket_type == FlexLogic::SocketType::INPUT_SOCKET && _nodes.at(i)->Sockets().at(ii)->SocketLinks().count() > 1) {
                QString error_text = node_text.arg(QString::number(i + 1), node_name) + QString("Несколько соединений на входе %1.").arg(QString::number(ii));
                // _error_list.append(error_text);
				emit errorToLog(error_text);
                exit = false;
            }
		}
	}

	//	Обработка дискретных входов/выходов
	if (input_count < 1) {
		QString error_text = "На схеме отсутствуют дискретные входы.";
        emit errorToLog(error_text);
        // _error_list.append(error_text);
        exit = false;
    }

	if (output_count < 1) {
		QString error_text = "На схеме отсутствуют дискретные выходы.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        exit = false;
    }

	//	Проверка списка сигналов
	for (int i = 0; i < _nodes.count(); i++) {
        QString node_type = _nodes.at(i)->name();
        if (_nodes.at(i)->nodeType() != FlexLogic::RZA_LOAD)
            continue;
        FlexLogic::NodeParam param = _nodes.at(i)->param("signal");
		int signal = param.value.toInt();
		if (signal < 0) {
			QString error_text = node_text.arg(QString::number(i + 1), node_type) + QString("Отсутствие сигнала.");
			emit errorToLog(error_text);
            // _error_list.append(error_text);
            exit = false;
        }
	}

	//	Обработка связей
	if (_links.isEmpty()) {
		QString error_text = "На схеме отсутствуют связи между элементами.";
		emit errorToLog(error_text);
        // _error_list.append(error_text);
        exit = false;
    }

	//	Проверка дублирования связей
	for (int i = 0; i < _links.count(); i++) {
        QString sourse = _links.at(i)->SourceID();
        QString target = _links.at(i)->TargetID();
		for (int ii = i + 1; ii < _links.count(); ii++) {
            if (_links.at(ii)->SourceID() != sourse || _links.at(ii)->TargetID() != target)
                continue;
			QString error_text = QString("Дублирование связей %1 и %2.").arg(QString::number(i + 1), QString::number(ii + 1));
			emit errorToLog(error_text);
            // _error_list.append(error_text);
            exit = false;
        }
	}

	//	Проверка соответствия source-target
	for (int i = 0; i < _links.count(); i++) {
        QString source_id = _links.at(i)->SourceID();
        QString target_id = _links.at(i)->TargetID();
		for (int node_id = 0; node_id < _nodes.count(); node_id++) {
            for (int socket = 0; socket < _nodes.at(node_id)->SocketsCount(); socket++) {
                QString socket_id = _nodes.at(node_id)->Sockets().at(socket)->ID();
				if (socket_id == source_id) {
                    if (_nodes.at(node_id)->Sockets().at(socket)->SocketType() != FlexLogic::SocketType::OUTPUT_SOCKET) {
						QString error_link = QString("Соединение ") + QString::number(i + 1) + QString(" некорректный исходный узел.");
						emit errorToLog(error_link);
                        // _error_list.append(error_link);
                        exit = false;
                    }
				}
				if (socket_id == target_id) {
                    if (_nodes.at(node_id)->Sockets().at(socket)->SocketType() != FlexLogic::SocketType::INPUT_SOCKET) {
						QString error_link = QString("Соединение ") + QString::number(i + 1) + QString(" некорректный финальный узел.");
						emit errorToLog(error_link);
                        // _error_list.append(error_link);
                        exit = false;
                    }
				}
			}
		}
	}

	//	Выход в случае наличия ошибок
    if (exit == false) {
		emit errorToLog("Схема содержит ошибки.");
        return false;
    }
    emit infoToLog("Проверка прошла успешно.");

	return true;
}

void DepCfcParser::setData(const QList<EditorNode*>& nodes, const QList<EditorLink*>& links)
{
	//	Проверка элементов
	if (nodes.size() > MAX_NODES) {
		QString error_text = "Количество элементов превышает максимум.";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        return;
    }
	uint8_t inputs_count = 0;
	
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes.at(i)->nodeType() == FlexLogic::RZA_LOAD)
            inputs_count++;
		if (inputs_count > MAX_INPUTS) {
			QString error_text = "Количество входов и выходов превышает максимум.";
            // _error_list.append(error_text);
			emit errorToLog(error_text);
            return;
        }
	}
	
	//	Формирование списков
	_nodes = nodes;
	_links = links;
}

// QList<uint16_t> DepCfcParser::cfcSignals() const
// {
// 	//	Проверка списка сигналов
//     QList<uint16_t> list;
//     if (_nodes.isEmpty()) return list;

// 	//	Заполнение списка сигналов
// 	for (int pin = 1; pin < MAX_PIN_NUMBER; pin++) {
// 		uint16_t signal = 0xFFFF;
// 		for (int i = 0; i < _nodes.count(); i++) {
//             if (_nodes.at(i)->nodeType() != FlexLogic::RZA_LOAD)
//                 continue;
//             if (_nodes.at(i)->param("alg_pin").value.toInt() != pin)
//                 continue;
//             signal = _nodes.at(i)->param("signal").value.toUInt();
//             break;
//         }
//         list.append(signal);
//     }

// 	return list;
// }


//===================================================================================================================================================
//	Методы обработки XML
//===================================================================================================================================================
EditorNode* DepCfcParser::newEditorNode(const QString& name, const QString& id, const QSizeF& node_size)
{
    if (name == "And")
        return new And(id, node_size);
    if (name == "BI")
        return new BI(id, node_size);
    if (name == "BO")
        return new BO(id, node_size);
    if (name == "Generator")
        return new Generator(id, node_size);
    if (name == "ImpulsePF")
        return new ImpulsePF(id, node_size);
    if (name == "ImpulseZF")
        return new ImpulseZF(id, node_size);
    if (name == "Not")
        return new Not(id, node_size);
    if (name == "Or")
        return new Or(id, node_size);
    if (name == "ReturnDelay")
        return new ReturnDelay(id, node_size);
    if (name == "RsTrigger")
        return new RsTrigger(id, node_size);
    if (name == "TriggerDelay")
        return new TriggerDelay(id, node_size);
    if (name == "Xor")
        return new Xor(id, node_size);

    return nullptr;
}

EditorNode* DepCfcParser::nodeFromXML(QDomNode xml)
{
	//	Получение базовых параметров узла
	QString id = xml.toElement().attribute("__id");
	QString type = xml.toElement().attribute("__type");

	//	Резервирование параметров узла
	QPointF node_position = QPointF(); QSizeF node_size = QSizeF(); int node_inputs = 0;
	QStringList param_names, param_values;

	//	Получение параметров узла
	QDomNode child_node = xml.firstChild();
	while (!child_node.isNull()) {
		QString tag_name = child_node.toElement().tagName();

		//	Чтение координат элемента
		if (tag_name == "pos") {
			double x_pos = child_node.toElement().attribute("x").toFloat();
			double y_pos = child_node.toElement().attribute("y").toFloat();
			node_position = QPointF((int)x_pos, (int)y_pos);
			child_node = child_node.nextSibling();
            continue;
        }

		//	Чтение размера элемента
		if (tag_name == "size") {
			double node_width = child_node.toElement().attribute("width").toFloat();
			double node_height = child_node.toElement().attribute("height").toFloat();
			node_size = QSizeF((int)node_width, (int)node_height);
			child_node = child_node.nextSibling();
            continue;
        }

		//	Чтение списка соединителей
		if (tag_name == "connectors") {
			QDomNodeList connect_list = child_node.toElement().elementsByTagName("connector");
			for (int i = 0; i < connect_list.count(); i++) {
				QDomNode connect_node = connect_list.at(i);
				QString type_tag = connect_node.toElement().attribute("type");
                //QString id_tag = connect_node.toElement().attribute("id");
                if (type_tag == "Input") node_inputs++;
            }
			child_node = child_node.nextSibling();
            continue;
        }

		//	Чтение параметров элемента
		QString value = child_node.toElement().attribute("value");
		param_names.append(tag_name);
		param_values.append(value);
		child_node = child_node.nextSibling();
        continue;
    }

	//	Установка параметров элемента
	EditorNode* node = newEditorNode(type, id, node_size);
    if (node == nullptr)
        return nullptr;
    if (node_position == QPointF())
        return nullptr;

	//	Настройка узла по полученным параметрам
    node->SetInputs(node_inputs);
    for (int i = 0; i < param_names.count(); i++)
        if (!param_values.at(i).isEmpty())
            node->setParam(param_names.at(i), param_values.at(i));

    node->setPos(node_position);

	return node;
}

EditorLink* DepCfcParser::linkFromXML(QDomNode xml, QList<EditorNode*> nodes)
{
	//	Базовые параметрв связи
	QList<QPointF> points;
	points.clear();

	//	Получение параметров связи
	QString id = xml.toElement().attribute("__id");
	int source_connector = xml.toElement().attribute("source_connector").toInt();
	int target_connector = xml.toElement().attribute("target_connector").toInt();

	//	Получение параметров связи
	QDomNode child_node = xml.firstChild();
    QString source_id = QString();
    QString target_id = QString();
    while (!child_node.isNull()) {
		QString tag_name = child_node.toElement().tagName();

		//	Чтение source_ref
		if (tag_name == "source") {
			source_id = child_node.toElement().attribute("__ref");
			child_node = child_node.nextSibling();
            continue;
        }

		//	Чтение target_ref
		if (tag_name == "target") {
			target_id = child_node.toElement().attribute("__ref");
			child_node = child_node.nextSibling();
            continue;
        }

		//	Получение координат точек
		if (tag_name == "points") {
			QDomNodeList points_list = xml.toElement().elementsByTagName("point");
			for (int ii = 0; ii < points_list.count(); ii++) {
				QDomNode point_node = points_list.at(ii);
				if (point_node.isNull()) continue;
				double x = point_node.toElement().attribute("x").toDouble();
				double y = point_node.toElement().attribute("y").toDouble();
                points.append(QPointF((int)x, (int)y));
            }
			child_node = child_node.nextSibling();
            continue;
        }
	}

	//	Проверка источников сигналов
    if (source_id.toInt() < 0 || target_id.toInt() < 0)
        return nullptr;

	//	Поиск источников сигналов для версии 0
    QString source_socket = QString();
    QString target_socket = QString();
    QPointF source_point = QPointF();
    QPointF target_point = QPointF();
	for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i)->ID() != source_id)
            continue;
        source_socket = nodes.at(i)->Sockets().at(source_connector)->ID();
        source_point = nodes.at(i)->Sockets().at(source_connector)->scenePos();
        break;
    }
	for (int i = 0; i < nodes.count(); i++) {
        if (nodes.at(i)->ID() != target_id)
            continue;
        target_socket = nodes.at(i)->Sockets().at(target_connector)->ID();
        target_point = nodes.at(i)->Sockets().at(target_connector)->scenePos();
        break;
    }
    if (source_socket.isEmpty() || target_socket.isEmpty()) return nullptr;

	//	Создание связи между узлами
	EditorLink* link = new EditorLink(points, id);
    link->SetSource(source_socket);
    if (points.first() != source_point)
        link->changeSource(source_point);

    link->SetTarget(target_socket);
    if (points.last() != target_point)
        link->changeSource(target_point);

	//	Проверка вертикальных начальных и конечных отрезков
    int last_index = points.count() - 1;
    if (points.first().x() == points.at(1).x())
        points.push_front(points.first());
    if (points.last().x() == points.at(last_index - 1).x())
        points.push_back(points.last());

    return link;
}

void DepCfcParser::linkNodes(const QList<EditorNode*>& nodes, const QList<EditorLink*>& links)
{
	for (int i = 0; i < nodes.count(); i++) {
        for (int ii = 0; ii < nodes.at(i)->SocketsCount(); ii++) {
            QString socket_id = nodes.at(i)->SocketID(ii);
            for (int j = 0; j < links.count(); j++)
                if (links.at(j)->SourceID() == socket_id || links.at(j)->TargetID() == socket_id)
                    nodes.at(i)->Sockets().at(ii)->AppendLink(links.at(j)->ID());
        }
	}

	return;
}

bool DepCfcParser::deserialize(QDomDocument xml)
{
	//	Чтение версии файла алгоритма
	QDomElement root = xml.documentElement();
	QDomNode version_node = root.firstChild();
    int version = 0;
    if (version_node.toElement().tagName() == "VERSION")
        version = version_node.toElement().attribute("version").toInt();

    //	Чтение заголовка алгоритма
    QDomNode canvas_node = version_node.nextSibling();
    _title = canvas_node.toElement().attribute("title");

	//	Загрузка списка элементов
	QDomNodeList nodes_list = xml.elementsByTagName("ELEMENT");
	if (nodes_list.size() > MAX_NODES) {
		QString error_text = "Количество элементов превышает максимум";
        // _error_list.append(error_text);
		emit errorToLog(error_text);
        return false;
    }

    if (nodes_list.count() > 0) {
		uint8_t inputs_count = 0;
        for (int i = 0; i < nodes_list.size(); i++) {
            if (nodes_list.at(i).isNull())
                continue;
            EditorNode* node = nodeFromXML(nodes_list.at(i));
            if (node->nodeType() == FlexLogic::RZA_LOAD)
                inputs_count++;
			if (inputs_count > MAX_INPUTS) {
				QString error_text = "Количество входов и выходов превышает максимум";
                // _error_list.append(error_text);
				emit errorToLog(error_text);
                return false;
            }
			if (node == nullptr) {
				QString error_text = "Ошибка чтения элемента номер " + QString::number(i) + ".";
                // _error_list.append(error_text);
				emit errorToLog(error_text);
                return false;
            }
            _nodes.append(node);
        }
	}

	//	Загрузка связей между элементами 
	QDomNodeList links_list = xml.elementsByTagName("CONNECTION");
	if (links_list.count() > 0) {
		for (int i = 0; i < links_list.count(); i++) {
            if (links_list.at(i).isNull())
                continue;
            EditorLink* link = linkFromXML(links_list.at(i), _nodes);
			if (link == nullptr) {
				QString error_text = "Ошибка чтения связи номер " + QString::number(i) + ".";
                // _error_list.append(error_text);
				emit errorToLog(error_text);
                return false;
            }
            _links.append(link);
        }
	}

	//	Добавление связей элементам
	linkNodes(_nodes, _links);

	return true;
}

void DepCfcParser::nodeToXML(EditorNode* node, QDomDocument& xml, QDomElement& xml_node)
{
	//	Аттрибуты элемента
    xml_node.setAttribute("__type", node->name());
    xml_node.setAttribute("__id", node->ID());

	//	Расположение элемента
	QDomElement xml_pos = xml.createElement("pos");
	xml_pos.setAttribute("x", node->pos().x());
	xml_pos.setAttribute("y", node->pos().y());
	xml_node.appendChild(xml_pos);

	//	Размер элемента
	QDomElement xml_size = xml.createElement("size");
    xml_size.setAttribute("width", node->NodeSize().width());
    xml_size.setAttribute("height", node->NodeSize().height());
	xml_node.appendChild(xml_size);

	//	Список соединений
	QDomElement xml_sockets_list = xml.createElement("connectors");
	xml_sockets_list.setAttribute("__node_type", "list");
    for (int i = 0; i < node->SocketsCount(); i++) {
		QDomElement xml_socket = xml.createElement("connector");
        xml_socket.setAttribute("id", node->Sockets().at(i)->ID());
        node->Sockets().at(i)->SocketType() == FlexLogic::SocketType::INPUT_SOCKET ?
			xml_socket.setAttribute("type", "Input") : xml_socket.setAttribute("type", "Output"); 
        xml_sockets_list.appendChild(xml_socket);
    }
	xml_node.appendChild(xml_sockets_list);

	//	Список свойств элемента
    for (int i = 0; i < node->ParamsList().count(); i++) {
        QDomElement xml_param = xml.createElement(node->ParamsList().at(i).index);
        xml_param.setAttribute("value", node->ParamsList().at(i).value.toString());
        xml_node.appendChild(xml_param);
    }

	return;
}

void DepCfcParser::linkToXML(EditorLink* link, QDomDocument& xml, QDomElement& xml_link)
{
	//	Получение первой и последней точки
    QList<QPointF> points = link->points();
	 
	//	Стандартные аттрибуты элемента
	xml_link.setAttribute("__type", "DseGraphicsConnection");
    xml_link.setAttribute("__id", link->ID());
    xml_link.setAttribute("source", link->SourceID());
    xml_link.setAttribute("target", link->TargetID());

	//	Параметры коннектора
	int source_connector = -1, target_connector = -1;
	QString source_id = QString(), target_id = QString();

	//	Вычисление параметров источника для версии 0
	bool exit = false;
	for (int i = 0; i < editorNodes().count(); i++) {
        for (int ii = 0; ii < editorNodes().at(i)->Sockets().count(); ii++) {
            if (editorNodes().at(i)->Sockets().at(ii)->ID() != link->SourceID())
                continue;
            source_id = editorNodes().at(i)->ID();
			source_connector = ii;
			exit = true;
            break;
        }
        if (exit)
            break;
    }

	//	Вычисление параметров цели для версии 0
	exit = false;
	for (int i = 0; i < editorNodes().count(); i++) {
        for (int ii = 0; ii < editorNodes().at(i)->Sockets().count(); ii++) {
            if (editorNodes().at(i)->Sockets().at(ii)->ID() != link->TargetID())
                continue;
            target_id = editorNodes().at(i)->ID();
			target_connector = ii;
			exit = true;
            break;
        }
        if (exit)
            break;
    }

	//	Добавление source
	xml_link.setAttribute("source_connector", source_connector);
	QDomElement xml_source = xml.createElement("source");
	xml_source.setAttribute("__ref", source_id);
	xml_link.appendChild(xml_source);

	//	Добавление target
	xml_link.setAttribute("target_connector", target_connector);
	QDomElement xml_target = xml.createElement("target");
	xml_target.setAttribute("__ref", target_id);
	xml_link.appendChild(xml_target);

	//	Добавление координат точек
	QDomElement xml_points_list = xml.createElement("points");
	xml_points_list.setAttribute("__node_type", "list");
	for (int i = 0; i < points.count(); i++) {
		QDomElement xml_point = xml.createElement("point");
		xml_point.setAttribute("x", points.at(i).x());
		xml_point.setAttribute("y", points.at(i).y());
        xml_points_list.appendChild(xml_point);
    }
	xml_link.appendChild(xml_points_list);

	return;
}

QDomDocument DepCfcParser::serialize()
{
	//	Создание корневого узла
	QDomDocument xml;
	QDomProcessingInstruction instruction = xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
	xml.appendChild(instruction);
	QDomElement root = xml.createElement("MODEL");
	xml.appendChild(root);
	QDomElement version = xml.createElement("VERSION");
	version.setAttribute("version", FlexLogic::CFC_VERSION);
	root.appendChild(version);

	//	Вывод параметров сцены
	QDomElement canvas = xml.createElement("CANVAS");
	canvas.setAttribute("__type", "DseGraphicsCanvas");
	canvas.setAttribute("__id", "1");
	canvas.setAttribute("title", _title);
	QDomElement size = xml.createElement("size");
    size.setAttribute("height", 1200);
    size.setAttribute("width", 1600);
	canvas.appendChild(size);
	root.appendChild(canvas);

	//	Создание списка узлов
	QDomElement nodes_list = xml.createElement("ELEMENTS");
	nodes_list.setAttribute("__node_type", "list");
	for (int i = 0; i < editorNodes().count(); i++) {
		QDomElement xml_node = xml.createElement("ELEMENT");
		nodeToXML(editorNodes().at(i), xml, xml_node);
        nodes_list.appendChild(xml_node);
    }
	root.appendChild(nodes_list);

	//	Создание списка связей
	QDomElement links_list = xml.createElement("CONNECTIONS");
	links_list.setAttribute("__node_type", "list");
	for (int i = 0; i < editorLinks().count(); i++) {
		QDomElement xml_link = xml.createElement("CONNECTION");
		linkToXML(editorLinks().at(i), xml, xml_link);
        links_list.appendChild(xml_link);
    }
	root.appendChild(links_list);

	return xml;
}
