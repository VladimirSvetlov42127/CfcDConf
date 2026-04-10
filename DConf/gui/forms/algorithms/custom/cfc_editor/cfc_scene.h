#ifndef __CFC_SCENE_H__
#define __CFC_SCENE_H__


#include <QObject>
#include <QString>
#include <QGraphicsSceneDragDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>
#include <QAction>
#include <QMenu>
#include <QPointF>

#include "service_manager/services/alg_cfc/cfc_alg_service.h"
#include "service_manager/signals/signal_manager.h"
#include "cfc_basic_scene.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_new_link.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_namespace.h"


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
//	Класс грвфической сцены
//  Методы работы с графикой и с сигналами управления
//===================================================================================================================================================
class CfcScene : public CfcBasicScene
{
    Q_OBJECT

public:
    CfcScene(CfcAlgService* service, SignalManager* signalManager, QGraphicsScene* parent = nullptr);

    SignalManager* signalManager() const { return _signal_manager; }
    bool gridEnabled() const { return CfcNamespace::grid_enable;}
    void setGrid(bool flag) { CfcNamespace::grid_enable = flag; }
    QMenu* contextMenu() { return _context_menu; }
    void setContextMenu(QMenu* menu) { _context_menu = menu; }

public slots:
    //===============================================================================================================================================
    //	Методы обработки сигналов меню
    //===============================================================================================================================================
    void onActionBI();
    void onActionBO();
    void onActionRemoveBI();
    void onActionRemoveBO();
    void onActionParam();

protected:
    //===============================================================================================================================================
    //	Методы обработки сигналов мышки
    //===============================================================================================================================================
    virtual void drawBackground(QPainter* painter, const QRectF& rect) override;
    virtual void dropEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    //===============================================================================================================================================
    //	Свойства класса
    //===============================================================================================================================================
    SignalManager* _signal_manager;             //  Менеджер сигналов
    CfcNewLink* _new_link;                      //  Объект для создания новой связи
    QAction* _action_bi;                        //  Вызов смены сигнала дискретного входа
    QAction* _action_bo;                        //  Вызов смены сигнала дискретного выхода
    QAction* _action_remove_bi;                 //  Вызов удаления сигнала дискретного входа
    QAction* _action_remove_bo;                 //  Вызов удаления сигнала дискретного выхода
    QAction* _action_param;                     //  Вызов смены параметров элемента
    QMenu* _context_menu;                       //  Контекстное меню сцены
    CfcSocket* _last_socket;                    //  Сокет для рисования связи
};

#endif // __CFC_BASIC_SCENE_H__
