#ifndef __EDITOR_SCENE_H__
#define __EDITOR_SCENE_H__

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QObject>
#include <QGraphicsScene>
#include <QString>
#include <QPointF>
#include <QColor>
#include <QList>
#include <QDomDocument>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QCursor>
#include <QSizeF>

#include <gui/forms/algorithms/custom/flexlogic_namespace.h>
#include <gui/forms/algorithms/custom/Editor/dummy_link.h>
#include <gui/forms/algorithms/custom/Editor/editor_node.h>
#include <gui/forms/algorithms/custom/Editor/editor_link.h>
#include "service_manager/service_manager.h"
#include "service_manager/services/alg_cfc/cfc_alg_service.h"


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
class EditorScene : public QGraphicsScene
{
	Q_OBJECT

public:
	//===============================================================================================================================================
	//	Конструктор и деструктор класса
	//===============================================================================================================================================
    EditorScene(CfcAlgService* cfcAlg, ServiceManager* smanager, QGraphicsScene* parent = nullptr);
	~EditorScene();

	//===============================================================================================================================================
	//	Открытые методы класса
	//===============================================================================================================================================
    ServiceManager* serviceManager() { return _service_manager; }
    CfcAlgService* cfcAlg() { return _cfcAlg; }

	void SetMenu(QMenu* menu) { _menu = menu; }
	QMenu* Menu() { return _menu; }
    int BufferCount() { return _buffer_links.count() + _buffer_nodes.count(); }
	FlexLogic::SceneStatus Status() { return _status; }
	EditorNode* NewEditorNode(QString type, QString ID = QString(), QSizeF node_size = QSizeF());
	QList<EditorNode*> Nodes();
	QList<EditorLink*> Links();
	QList<EditorNode*> SelectedNodes();
	QList<EditorLink*> SelectedLinks();
	QPointF SocketPosition(QString id, QList<EditorNode*> nodes = QList<EditorNode*>());
	void Clear();

private:
	//===============================================================================================================================================
	//	Вспомогательные методы класса
	//===============================================================================================================================================
	QList<FlexLogic::MemoryLink> MemoryLinks(QList<EditorNode*> selected_nodes, QList<EditorLink*> selected_links);
	EditorLink* CreateLink(DummyLink* dummy_link, EditorSocket* socket);
	void Move(QPointF delta);
	void DeleteSignal(EditorNode* node);

public slots:
	//===============================================================================================================================================
	//	Методы обработки сигналов
	//===============================================================================================================================================
	void EditNode();
	void AddSignal();
    void removeSignal();
	void AddInputs();
	void RemoveInputs();
	void Delete();
	void Copy(bool select = false);
	void Cut();
	void Paste();

protected:
	//===============================================================================================================================================
	//	Перегружаемые методы класса
	//===============================================================================================================================================
	virtual void drawBackground(QPainter* painter, const QRectF& rect);

	//===============================================================================================================================================
	//	Методы обработки событий мыши
	//===============================================================================================================================================
	virtual void dropEvent(QGraphicsSceneDragDropEvent* event);
	virtual void dragEnterEvent(QGraphicsSceneDragDropEvent* event);
	virtual void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent);

signals:
	//===============================================================================================================================================
	//	Сигнал изменения данных
	//===============================================================================================================================================
	void DataChanged(bool flag);
	void BufferChanged(bool flag);

private:
	//===============================================================================================================================================
	//	Элементы формы
	//===============================================================================================================================================
	QMenu* _menu;
    QMenu* _add_signal_menu;
    QMenu* _remove_signal_menu;

	//===============================================================================================================================================
	//	Свойства класса
	//===============================================================================================================================================
    ServiceManager* _service_manager;
    CfcAlgService* _cfcAlg;
	FlexLogic::SceneStatus _status;
	QList<FlexLogic::MemoryLink> _buffer_links;
	QList<FlexLogic::MemoryNode> _buffer_nodes;
	QPointF _base_point;
	QPointF _menu_point;
	DummyLink* _new_link;
	EditorLink* _editor_link;
};

#endif	//	__EDITOR_SCENE_H__
