#ifndef __CFC_BASIC_SCENE_H__
#define __CFC_BASIC_SCENE_H__


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QGraphicsScene>
#include <QObject>
#include <QString>
#include <QList>
#include <QVariant>
#include <QPointF>
#include <QMenu>


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "service_manager/service_manager.h"
#include "service_manager/services/alg_cfc/cfc_alg_service.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_node.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_link.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_title_item.h"


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
//	Базовый класс для грвфической сцены
//  Методы работы со свойствами класса
//===================================================================================================================================================
class CfcBasicScene : public QGraphicsScene
{
    Q_OBJECT

public:
    //===============================================================================================================================================
    //	Конструктор класса
    //===============================================================================================================================================
    CfcBasicScene(CfcAlgService* service, ServiceManager* service_manager, QGraphicsScene* parent = nullptr);

    //===============================================================================================================================================
    //	Открытые методы класса
    //===============================================================================================================================================
    CfcAlgService* service() const { return _service; }
    ServiceManager* serviceManager() const { return _service_manager; }


    QList<CfcNode*> bufferNodes() const { return _buffer_nodes; }
    QList<CfcLink*> bufferLinks() const { return _buffer_links; }
    void setTitle(const QString& title) { _title_item->setTitle(title); }
    QString title() const {return _title_item->title(); }
    bool gridEnabled() const { return _grid_enable; }
    void setGrid(bool flag) { _grid_enable = flag; }
    uint16_t bufferCount() { return _buffer_nodes.count() + _buffer_links.count(); }
    QMenu* contextMenu() { return _context_menu; }
    void setContextMenu(QMenu* menu) { _context_menu = menu; }
    QPointF menuPoint() const { return _menu_point; }
    void setMenuPoint(const QPointF& point) { _menu_point = point; }

    CfcNode* newEditorNode(QString name);
    QList<CfcNode*> nodes() const;
    QList<CfcLink*> links() const;
    QList<CfcNode*> selectedNodes() const;
    QList<CfcLink*> selectedLinks() const;

    bool dataProcessing(QList<CfcNode*> nodes, QList<CfcLink*> links);
    bool validate(QList<CfcNode*> nodes, QList<CfcLink*> links);

public slots:
    //===============================================================================================================================================
    //	Методы обработки сигналов сцены
    //===============================================================================================================================================
    void removeSelected();
    void copySelected();
    void pasteSelected();
    void cutSelected();

private:
    //===============================================================================================================================================
    //	Вспомогательные методы класса
    //===============================================================================================================================================
    CfcNode* nodeByID(const QString& id);
    CfcLink* linkByID(const QString& id);
    void nodesProcessing(QList<CfcNode*> nodes);
    void removeLink(CfcLink* link);
    void removeNode(CfcNode* node);
    CfcNode* copyNode(CfcNode* source);
    bool CheckCfc(QList<TargetElement*> targets) const;

signals:
    //===============================================================================================================================================
    //	Сигналы парсера
    //===============================================================================================================================================
    void infoToLog(const QString& message);
    void warningToLog(const QString& message);
    void errorToLog(const QString& message);

private:
    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    ServiceManager* _service_manager;           //  Менеджер сервисов
    CfcAlgService* _service;                    //  Сервис алгоритма гибкой логикиж
    QMenu* _context_menu;                       //  Контекстное меню сцены
    CfcTitleItem* _title_item;                  //  Объект заголовка окна
    QList<CfcNode*> _buffer_nodes;              //  Список узлов для копирования
    QList<CfcLink*> _buffer_links;              //  Список связей для копирования
    bool _grid_enable;                          //  Флаг активации сетки
    QPointF _basic_point;                       //  Базовая точка для копирования и вставки
    QPointF _menu_point;                        //  Точка вызова меню
};

#endif // __CFC_BASIC_SCENE_H__
