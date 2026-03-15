#include "cfc_basic_scene.h"


//===================================================================================================================================================
//	Подключение внешних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/algorithms/custom/cfc_nodes/cfc_nodes_list.h>
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"


//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    QColor scene_bkcolor = QColor(255,255,255);
    uint8_t select_shape = 10;
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcBasicScene::CfcBasicScene(CfcAlgService* service, ServiceManager* service_manager, QGraphicsScene* parent) : QGraphicsScene(parent)
{
    _menu_point = QPointF();
    _basic_point = QPointF();
    _buffer_nodes.clear();
    _buffer_links.clear();
    _service_manager = service_manager;
    _service = service;

    setItemIndexMethod(QGraphicsScene::NoIndex);
    setBackgroundBrush(scene_bkcolor);
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
CfcNode* CfcBasicScene::newEditorNode(QString name)
{
    if (name == "And")
        return new CfcAnd();
    if (name == "Generator")
        return new CfcGenerator();
    if (name == "ImpulsePF")
        return new CfcImpulsePF();
    if (name == "ImpulseZF")
        return new CfcImpulseZF();
    if (name == "Not")
        return new CfcNot();
    if (name == "Or")
        return new CfcOr();
    if (name == "ReturnDelay")
        return new CfcReturnDelay();
    if (name == "RsTrigger")
        return new CfcRsTrigger();
    if (name == "TriggerDelay")
        return new CfcTriggerDelay();
    if (name == "Xor")
        return new CfcXor();

    if (name == "BI") {
        auto input = service()->makeInput();
        if (!input)
            return nullptr;
        return new CfcBI(QString(), QSizeF(), input);
    }

    if (name == "BO") {
        auto output = service()->makeOutput();
        if (!output)
            return nullptr;
        return new CfcBO(QString(), QSizeF(), output);
    }

    return nullptr;
}

QList<CfcNode*> CfcBasicScene::nodes() const
{
    QList<CfcNode*> list;
    int count = items().count();
    for (int i = 0; i < count; i++) {
        CfcNode* node = dynamic_cast<CfcNode*>(items().at(i));
        if (!node)
            continue;
        list.append(node);
    }

    return list;
}

QList<CfcLink*> CfcBasicScene::links() const
{
    QList<CfcLink*> list;
    int count = items().count();
    for (int i = 0; i < count; i++) {
        CfcLink* link = dynamic_cast<CfcLink*>(items().at(i));
        if (!link)
            continue;
        list.append(link);
    }

    return list;
}

QList<CfcNode*> CfcBasicScene::selectedNodes() const
{
    QList<CfcNode*> list;
    int count = selectedItems().count();
    for (int i = 0; i < count; i++) {
        CfcNode* node = dynamic_cast<CfcNode*>(selectedItems().at(i));
        if (!node)
            continue;
        list.append(node);
    }

    return list;
}

QList<CfcLink*> CfcBasicScene::selectedLinks() const
{
    QList<CfcLink*> list;
    int count = selectedItems().count();
    for (int i = 0; i < count; i++) {
        CfcLink* link = dynamic_cast<CfcLink*>(selectedItems().at(i));
        if (!link)
            continue;
        list.append(link);
    }

    return list;
}


//===================================================================================================================================================
//	Методы обработки сигналов сцены
//===================================================================================================================================================
void CfcBasicScene::removeSelected()
{
    //  Удаление выделенных связей
    QList<CfcLink*>cfc_links = selectedLinks();
    for (int i =0; i <cfc_links.count(); i++)
        removeLink(cfc_links.at(i));

    //  Удаление выделенных узлов
    QList<CfcNode*>cfc_nodes = selectedNodes();
    for (int i =0; i <cfc_nodes.count(); i++)
        removeNode(cfc_nodes.at(i));

    return;
}

void CfcBasicScene::copySelected()
{
    //  Копирование выделенных узлов
    QList<CfcNode*> nodes = selectedNodes();
    for (int i = 0; i < nodes.count(); i++) {
        CfcNode* node = copyNode(nodes.at(i));
        _buffer_nodes.append(node);
    }

    //  Копирование выделенных связей
    QList<CfcLink*> links = selectedLinks();
    for (int i = 0; i < links.count(); i++) {
        CfcLink* link = links.at(i);
        if (!link->source()->parent()->isSelected() || !link->target()->parent()->isSelected())
            continue;
        CfcLink* link_copy = new CfcLink(QString(), link->points());
        _buffer_links.append(link_copy);
    }

    //  Привязка источников связи
    for (int i = 0; i < _buffer_links.count(); i++) {
        CfcLink* link = _buffer_links.at(i);
        bool found = false;
        QPointF source_pos = link->points().at(0);
        for (int ii = 0; ii < _buffer_nodes.count(); ii++) {
            if (found)
                break;
            CfcNode* node = _buffer_nodes.at(ii);
            for (int iii = 0; iii < node->sockets().count(); iii++) {
                CfcSocket* socket = node->sockets().at(iii);
                QPointF socket_pos = socket->scenePos();
                QLineF check_line(source_pos, socket_pos);
                if (check_line.length() > select_shape)
                    continue;

                //  Найден источник связи
                socket->appendLink(link);
                link->setSource(socket);
                found = true;
                break;
            }
        }
    }

    //  Привязка целей связи
    for (int i = 0; i < _buffer_links.count(); i++) {
        CfcLink* link = _buffer_links.at(i);
        bool found = false;
        QPointF target_pos = link->points().last();
        for (int ii = 0; ii < _buffer_nodes.count(); ii++) {
            if (found)
                break;
            CfcNode* node = _buffer_nodes.at(ii);
            for (int iii = 0; iii < node->sockets().count(); iii++) {
                CfcSocket* socket = node->sockets().at(iii);
                QPointF socket_pos = socket->scenePos();
                QLineF check_line(target_pos, socket_pos);
                if (check_line.length() > select_shape)
                    continue;

                //  Найдена цель связи
                socket->appendLink(link);
                link->setTarget(socket);
                found = true;
                break;
            }
        }
    }

    _basic_point = menuPoint();

    return;
}

void CfcBasicScene::pasteSelected()
{
    QPointF delta = menuPoint() - _basic_point;

    //  Вывод узлов
    for (int i = 0; i < _buffer_nodes.count(); i++) {
        CfcNode* node = _buffer_nodes.at(i);
        node->setPos(node->pos() + delta);
        addItem(node);
    }
    _buffer_nodes.clear();

    //  Вывод связей
    for (int i = 0; i < _buffer_links.count(); i++) {
        CfcLink* link = _buffer_links.at(i);
        QList<QPointF> cfc_points = link->points();
        QList<QPointF> new_points;
        for (int i = 0; i < cfc_points.count(); i++)
            new_points.append(cfc_points.at(i) + delta);
        link->setPoints(new_points);
        addItem(link);
    }
    _buffer_links.clear();

    return;
}

void CfcBasicScene::cutSelected()
{
    copySelected();
    removeSelected();
    return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
CfcNode* CfcBasicScene::nodeByID(const QString& id)
{
    QList<CfcNode*> cfc_nodes = nodes();

    for (int i = 0; i < cfc_nodes.count(); i++)
        if (cfc_nodes.at(i)->id() == id)
            return cfc_nodes.at(i);

    return nullptr;
}

CfcLink* CfcBasicScene::linkByID(const QString& id)
{
    QList<CfcLink*> cfc_links = links();

    for (int i = 0; i < cfc_links.count(); i++)
        if (cfc_links.at(i)->id() == id)
            return cfc_links.at(i);

    return nullptr;
}

void CfcBasicScene::removeLink(CfcLink* link)
{
    //  Удаление источника чигнала
    QString source_id = link->sourceID();
    uint8_t source_index = link->sourceIndex();
    CfcNode* source_node =nodeByID(source_id);
    if (source_node->sockets().at(source_index))
        source_node->sockets().at(source_index)->removeLink(link);

    //  Отвязывание сигнала
    if (source_node->name() == "BI"){
        CfcBI* bi_node = static_cast<CfcBI*>(source_node);
        if (bi_node->cfcInput())
            bi_node->cfcInput()->setSource(nullptr);
        bi_node->setParam("signal", -1);
        bi_node->setParam("name", QString());
    }

    //  Удаление приемника чигнала
    QString target_id = link->targetID();
    uint8_t target_index = link->targetIndex();
    CfcNode* target_node =nodeByID(target_id);
    if (target_node->sockets().at(target_index))
        target_node->sockets().at(target_index)->removeLink(link);

    //  Отвязывание сигнала
    if (target_node->name() == "BO"){
        CfcBO* bo_node = static_cast<CfcBO*>(target_node);
        if (bo_node->cfcOutput())
            bo_node->cfcOutput()->setTarget(nullptr);
        bo_node->setParam("signal", -1);
        bo_node->setParam("name", QString());
    }
    removeItem(link);
    delete link;
    update();
}

void CfcBasicScene::removeNode(CfcNode* node)
{
    //  Удаление элементов BI/BO
    if (node->name() == "BI") {
        CfcBI* bi_node = static_cast<CfcBI*>(node);
        auto input = bi_node->cfcInput();
        service()->removeInput(input);
        CfcSocket* socket= node->sockets().at(0);
        for (int i = 0; i < socket->links().count(); i++)
            if (socket->links().at(i))
                delete socket->links().at(i);
        removeItem(node);
        delete node;
        return;
    }

    if (node->name() == "BO") {
        CfcBO* bo_node = static_cast<CfcBO*>(node);
        auto output = bo_node->cfcOutput();
        service()->removeOutput(output);
        CfcSocket* socket= node->sockets().at(0);
        for (int i = 0; i < socket->links().count(); i++)
            if (socket->links().at(i))
                delete socket->links().at(i);
        removeItem(node);
        delete node;
        return;
    }

    //  Удаление остальных элементов
    for (int i = 0; i < node->sockets().count(); i++) {
        CfcSocket* socket= node->sockets().at(i);
        for (int ii = 0; ii < socket->links().count(); ii++)
            if (socket->links().at(ii))
                delete socket->links().at(ii);
    }
    removeItem(node);
    delete node;

    return;
}

CfcNode* CfcBasicScene::copyNode(CfcNode* source)
{
    CfcNode* node = newEditorNode(source->name());
    if (!node)
        return nullptr;

    //  Добавление входов
    if (source->inputs() != node->inputs())
        node->setInputs(source->inputs());

    //  Обновление параметров
    for (int i = 0; i < source->paramsList().count(); i++)
        node->setParam(source->paramsList().at(i).index, source->paramsList().at(i).value);

    //  Основные параметры
    if (node->name() != "BI" && node->name() != "BO") {
        node->setSize(source->size());
        node->setPos(source->pos());
    }

    //  Обработка BI/BO
    if (node->name() == "BI") {
        node->setParam("signal", -1);
        node->setParam("name", QString());
        QPointF position = source->pos();
        position.setX(position.x() - (node->size().width() - source->size().width()));
        node->setPos(position);
    }
    if (node->name() == "BO") {
        node->setParam("signal", -1);
        node->setParam("name", QString());
        node->setPos(source->pos());
    }

    return node;
}

bool CfcBasicScene::CheckCfc(QList<TargetElement*> targets) const
{
    if (targets.isEmpty())
        return true;
    for (int i = 0; i < targets.count(); i++) {
        if (!targets.at(i))
            continue;
        CfcServiceInput* target = static_cast<CfcServiceInput*>(targets.at(i));
        if (target)
            return false;
    }
    return true;
}




