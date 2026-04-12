#include "cfc_basic_scene.h"


#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <QLineF>
#include <QDebug>

#include <gui/forms/algorithms/custom/cfc_nodes/cfc_nodes_list.h>
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"


namespace {
    QColor scene_bkcolor = QColor(255,255,255);
    uint8_t select_shape = 10;
}


CfcBasicScene::CfcBasicScene(CfcAlgService* service, QGraphicsScene* parent) : QGraphicsScene(parent)
{
    _menu_point = QPointF();
    _basic_point = QPointF();
    _buffer_nodes.clear();
    _buffer_links.clear();
    _service = service;

    setItemIndexMethod(QGraphicsScene::NoIndex);
    setBackgroundBrush(scene_bkcolor);
}


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

CfcNode* CfcBasicScene::ramNode(RamNode node)
{
    QString name = node.name;
    if (name == "And")
        return new CfcAnd(node);
    if (name == "Generator")
        return new CfcGenerator(node);
    if (name == "ImpulsePF")
        return new CfcImpulsePF(node);
    if (name == "ImpulseZF")
        return new CfcImpulseZF(node);
    if (name == "Not")
        return new CfcNot(node);
    if (name == "Or")
        return new CfcOr(node);
    if (name == "ReturnDelay")
        return new CfcReturnDelay(node);
    if (name == "RsTrigger")
        return new CfcRsTrigger(node);
    if (name == "TriggerDelay")
        return new CfcTriggerDelay(node);
    if (name == "Xor")
        return new CfcXor(node);

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


//	Методы обработки сигналов сцены
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
        RamNode node= nodes.at(i)->toRam();
        _buffer_nodes.append(node);
    }

    // Копирование выделенных связей (копируются только связи имеющие два соединения)
    QList<CfcLink*> links = selectedLinks();
    for (int i = 0; i < links.count(); i++) {
        CfcLink* link = links.at(i);
        if (!link->source()->parent()->isSelected() || !link->target()->parent()->isSelected())
            continue;
        RamLink ram_link;
        for (int ii = 0; ii < link->points().count(); ii++)
            ram_link.points.append(link->points().at(ii));
        _buffer_links.append(ram_link);
    }

    _basic_point = menuPoint();

    return;
}

void CfcBasicScene::pasteSelected()
{
    QPointF delta = menuPoint() - _basic_point;

    //  Вывод узлов
    QList<CfcNode*> nodes;
    for (int i = 0; i < _buffer_nodes.count(); i++) {
        CfcNode* node = ramNode(_buffer_nodes.at(i));
        node->setPos(_buffer_nodes.at(i).position + delta);
        nodes.append(node);
        addItem(node);
    }
    _buffer_nodes.clear();

    //  Вывод связей
    for (int i = 0; i < _buffer_links.count(); i++) {
        QList<QPointF> new_points;
        for (int ii = 0; ii < _buffer_links.at(i).points.count(); ii++)
            new_points.append(_buffer_links.at(i).points.at(ii) + delta);
        CfcLink* link = new CfcLink(QString(), new_points);
        QPointF source_point = link->points().at(0);
        QPointF target_point = link->points().at(link->points().count() - 1);
        if (link->source() != nullptr && link->target() != nullptr)
            continue;

        //  Привязка связей к сокетам
        for (int n = 0; n < nodes.count(); n++) {
            CfcNode* node = nodes.at(n);
            for (int nn = 0; nn < node->sockets().count(); nn++) {
                CfcSocket* socket = node->sockets().at(nn);
                QPointF x = socket->scenePos();
                QLineF source_line = QLineF(x, source_point);
                QLineF target_line = QLineF(x, target_point);

                if (source_line.length() < 1) {
                    link->setSource(socket);
                    socket->appendLink(link);
                }
                if (target_line.length() < 1) {
                    link->setTarget(socket);
                    socket->appendLink(link);
                }
            }
            if (link->source() != nullptr && link->target() != nullptr)
                break;
        }
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

    //  Удаление приемника чигнала
    QString target_id = link->targetID();
    uint8_t target_index = link->targetIndex();
    CfcNode* target_node =nodeByID(target_id);
    if (target_node->sockets().at(target_index))
        target_node->sockets().at(target_index)->removeLink(link);

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




