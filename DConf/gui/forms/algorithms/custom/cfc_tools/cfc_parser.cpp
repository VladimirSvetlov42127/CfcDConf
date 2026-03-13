#include "cfc_parser.h"

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
#include <QDebug>
#include <QFile>
#include <QPointF>
#include <QLineF>
#include <QTextStream>
#include <QDir>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/cfc_nodes/cfc_nodes_list.h>


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
CfcParser::CfcParser()
{
    _title = QString();
    _nodes.clear();
    _links.clear();
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
void CfcParser::setData(QList<CfcNode*> nodes, QList<CfcLink*> links)
{
    _nodes = nodes;
    _links = links;
}

bool CfcParser::loadData(const QString& file_name)
{
    //	Проверка файла проекта
    QFile xml_file(file_name);
    if (!xml_file.exists()) {
        QString error_text = "Файл алгоритма не найден.";
        // _error_list.append(error_text);
        emit errorToLog(error_text);
        return false;
    }

    //  Открытие графического файла
    if (!xml_file.open(QIODevice::ReadOnly)) {
        QString error_text = "Ошибка открытия файла алгоритма.";
        // _error_list.append(error_text);
        emit errorToLog(error_text);
        xml_file.close();
        return false;
    }

    //	Очистка параметров
    _nodes.clear();
    _links.clear();

    //	Получение данных из XML файла
    QDomDocument xml;
    QString content_error = QString("Ошибка обработки файла алгоритма: %1.\n Ошибка: %2, Строка: %3, Колонка %4.");
    QString error_message;
    int line, column;
    if (!xml.setContent(&xml_file, &error_message, &line, &column)) {
        QString error_text = content_error.arg(file_name, error_message, QString::number(line), QString::number(column));
        emit errorToLog(error_text);
        xml_file.close();
        return false;
    }
    xml_file.close();

    //  Получение элементов схемы
    if (!deserialize(xml)) return false;

    return true;
}

bool CfcParser::saveData(const QString& file_name)
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
        emit errorToLog(error_text);
        error = false;
    }
    xml_file.close();

    return error;
}

bool CfcParser::validate()
{
    bool error = true;

    //  Проверка состояния элементов BI/BO
    QString error_text_1 = QString("Элемент %1 ID= %2. - Отстутствие сервиса.");
    QString error_text_2 = QString("Элемент %1 ID= %2. - Не привязан сигнал.");
    for (int i = 0; i < nodes().count(); i++) {
        if (nodes().at(i)->name() != "BI" && nodes().at(i)->name() != "BO")
            continue;

        if (nodes().at(i)->name() == "BI") {
            CfcBI* bi_node = static_cast<CfcBI*>(nodes().at(i));
            if (!bi_node->cfcInput()) {
                error = false;
                QString message = error_text_1.arg(bi_node->name(), bi_node->id());
                emit warningToLog(message);
            }
            if (bi_node->cfcInput() && !bi_node->cfcInput()->source()) {
                error = false;
                QString message = error_text_2.arg(bi_node->name(), bi_node->id());
                emit warningToLog(message);
            }
            continue;
        }

        if (nodes().at(i)->name() == "BO") {
            CfcBO* bo_node = static_cast<CfcBO*>(nodes().at(i));
            if (!bo_node->cfcOutput()) {
                error = false;
                QString message = error_text_1.arg(bo_node->name(), bo_node->id());
                emit warningToLog(message);
            }
            if (bo_node->cfcOutput() &&  !bo_node->cfcOutput()->target()) {
                error = false;
                QString message = error_text_2.arg(bo_node->name(), bo_node->id());
                emit warningToLog(message);
            }
            continue;
        }
    }

    //  Перебор элементов для проверки наличия соединений
    QString error_text = QString("Элемент %1 ID= %2. Сокет[%3] - Отстутствие соединения.");
    for (int i = 0; i < nodes().count(); i++) {
        CfcNode* node = nodes().at(i);
        QString node_name = node->name();
        QString node_id = node->id();

        if (node_name == "BI" || node_name == "BO")
            continue;

        //  Проверка сокетов
        for (uint8_t ii = 0; ii < node->sockets().count(); ii++) {
            CfcSocket* socket = node->sockets().at(ii);
            if (socket->links().isEmpty()) {
                error = false;
                QString message = error_text.arg(node_name, node_id, QString::number(ii));
                emit warningToLog(message);
            }
        }
    }

    //  Проверка соединений
    QString link_source = QString("Связь ID= %1 Отсутствие источника сигнала.");
    QString link_target = QString("Связь ID= %1 Отсутствие приемника сигнала.");
    for (int i = 0; i < links().count(); i++) {
        CfcLink* link = links().at(i);
        if (!link->source()) {
            error = false;
            QString message = link_source.arg(link->id());
            emit warningToLog(message);
        }
        if (!link->target()) {
            error = false;
            QString message = link_target.arg(link->id());
            emit warningToLog(message);
        }
    }

    return error;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
bool CfcParser::deserialize(QDomDocument xml)
{
    //	Чтение версии файла алгоритма
    QDomElement root = xml.documentElement();
    QDomNode version_node = root.firstChild();
    int version = 0;
    if (version_node.toElement().tagName() == "VERSION")
        version = version_node.toElement().attribute("version").toInt();

    QDomNode canvas_node;
    version == 0 ? canvas_node = root.firstChild() : canvas_node = version_node.nextSibling();

    //	Чтение заголовка алгоритма
    _title = canvas_node.toElement().attribute("title");

    //	Загрузка списка элементов
    QDomNodeList nodes_list = xml.elementsByTagName("ELEMENT");
    for (int i = 0; i < nodes_list.size(); i++) {
        if (nodes_list.at(i).isNull())
            continue;

        CfcNode* node = CfcParser::newNode(nodes_list.at(i));
        _nodes.append(node);
    }

    //	Загрузка списка соединений
    QDomNodeList links_list = xml.elementsByTagName("CONNECTION");
    for (int i = 0; i < links_list.count(); i++) {
        if (links_list.at(i).isNull())
              continue;

        CfcLink* link = new CfcLink(links_list.at(i));
        if (link == nullptr) {
            QString error_text = "Ошибка чтения связи номер " + QString::number(i) + ".";
            emit errorToLog(error_text);
            return false;
        }
        _links.append(link);
    }
    linksProcessing(_nodes, _links);

    return true;
}

CfcNode* CfcParser::newNode(const QDomNode& xml)
{
    QString name = xml.toElement().attribute("__type");

    if (name == "And")
        return new CfcAnd(xml);
    if (name == "BI")
        return new CfcBI(xml);
    if (name == "BO")
        return new CfcBO(xml);
    if (name == "Generator")
        return new CfcGenerator(xml);
    if (name == "ImpulsePF")
        return new CfcImpulsePF(xml);
    if (name == "ImpulseZF")
        return new CfcImpulseZF(xml);
    if (name == "Not")
        return new CfcNot(xml);
    if (name == "Or")
        return new CfcOr(xml);
    if (name == "ReturnDelay")
        return new CfcReturnDelay(xml);
    if (name == "RsTrigger")
        return new CfcRsTrigger(xml);
    if (name == "TriggerDelay")
        return new CfcTriggerDelay(xml);
    if (name == "Xor")
        return new CfcXor(xml);

    return nullptr;
}

QDomDocument CfcParser::serialize()
{
    //  Размеры сцены
    uint8_t cfc_version = 1;
    int width = 1280;
    int height = 1024;

    //	Создание корневого узла
    QDomDocument xml;
    QDomProcessingInstruction instruction = xml.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\"");
    xml.appendChild(instruction);
    QDomElement root = xml.createElement("MODEL");
    xml.appendChild(root);
    QDomElement version = xml.createElement("VERSION");
    version.setAttribute("version", cfc_version);
    root.appendChild(version);

    //	Вывод параметров сцены
    QDomElement canvas = xml.createElement("CANVAS");
    canvas.setAttribute("__type", "DseGraphicsCanvas");
    canvas.setAttribute("__id", "1");
    canvas.setAttribute("title", _title);
    QDomElement size = xml.createElement("size");
    size.setAttribute("height", height);
    size.setAttribute("width", width);
    canvas.appendChild(size);
    root.appendChild(canvas);

    //	Создание списка узлов
    QDomElement nodes_list = xml.createElement("ELEMENTS");
    nodes_list.setAttribute("__node_type", "list");
    for (int i = 0; i < nodes().count(); i++) {
        QDomNode xml_node = nodes().at(i)->toXML();
        nodes_list.appendChild(xml_node);
    }
    root.appendChild(nodes_list);

    //	Создание списка связей
    QDomElement links_list = xml.createElement("CONNECTIONS");
    links_list.setAttribute("__node_type", "list");
    for (int i = 0; i < links().count(); i++) {
        QDomNode xml_link = links().at(i)->toXML();
        links_list.appendChild(xml_link);
    }
    root.appendChild(links_list);

    return xml;
}

void CfcParser::linksProcessing(QList<CfcNode*> nodes, QList<CfcLink*> links)
{
    for (int i = 0; i < links.count(); i++) {
        CfcLink* link = links.at(i);
        QString source_id = link->sourceID();
        QString target_id = link->targetID();
        uint8_t source_index = link->sourceIndex();
        uint8_t target_index = link->targetIndex();

        for (int ii = 0; ii < nodes.count(); ii++) {
            CfcNode* node = nodes.at(ii);
            if (node->id() != source_id && node->id() != target_id)
                continue;

            if (node->id() == source_id) {
                CfcSocket* socket = node->sockets().at(source_index);
                link->setSource(socket);
                socket->appendLink(link);
                continue;
            }

            if (node->id() == target_id) {
                CfcSocket* socket = node->sockets().at(target_index);
                link->setTarget(socket);
                socket->appendLink(link);
                continue;
            }
        }
    }

    return;
}




