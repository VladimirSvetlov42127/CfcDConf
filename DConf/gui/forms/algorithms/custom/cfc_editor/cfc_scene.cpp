#include "cfc_scene.h"


//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>

#include <QColor>
#include <QMimeData>
#include <QByteArray>
#include <QSizeF>


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "service_manager/services/alg_cfc/cfc_service_input.h"
#include "service_manager/services/alg_cfc/cfc_service_output.h"
#include "gui/forms/algorithms/custom/cfc_editor/cfc_socket.h"
#include "gui/forms/algorithms/custom/cfc_nodes/cfc_bi.h"
#include "gui/forms/algorithms/custom/cfc_nodes/cfc_bo.h"
#include "gui/dialogs/params_dialog.h"
#include "gui/dialogs/binding_dialog.h"


//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
namespace {
    uint8_t grid_step = 50;
    uint8_t grid_small_step = 10;
    QColor grid_color = QColor(200, 200, 255, 125);
}


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcScene::CfcScene(CfcAlgService* service, ServiceManager* service_manager, QGraphicsScene* parent) : CfcBasicScene(service, service_manager, parent)
{
    //  Свойства класса
    _new_link = nullptr;
    //_menu_point = QPointF();
    //_basic_point = QPointF();
    _grid_enable = true;

    //  Параметры меню
    _action_bi = new QAction("Привязка", this);
    _action_bo = new QAction("Привязка", this);
    _action_param = new QAction("Параметры", this);

    connect(_action_bi, &QAction::triggered, this, &CfcScene::onActionBI);
    connect(_action_bo, &QAction::triggered, this, &CfcScene::onActionBO);
    connect(_action_param, &QAction::triggered, this, &CfcScene::onActionParam);
}


//===================================================================================================================================================
//	Методы обработки сигналов меню
//===================================================================================================================================================
void CfcScene::onActionBI()
{
    CfcBI* bi_node = dynamic_cast<CfcBI*>(itemAt(menuPoint(), QTransform()));
    if (!bi_node)
        return;

    BindingDialog dialog(BindingDialog::TYPE_INPUT, serviceManager());
    if (dialog.exec() != QDialog::Accepted)
        return;

    InputSignal* input_signal = dialog.selectedSignal();
    if (!input_signal)
        return;

    if (!bi_node->cfcInput())
        return;
    bi_node->cfcInput()->setSource(input_signal);
    update();

    return;
}

void CfcScene::onActionBO()
{
    CfcBO* bo_node = dynamic_cast<CfcBO*>(itemAt(menuPoint(), QTransform()));
    if (!bo_node)
        return;

    BindingDialog dialog(BindingDialog::TYPE_OUTPUT, serviceManager());
    if (dialog.exec() != QDialog::Accepted)
        return;

    VirtualInputSignal* output_signal = dynamic_cast<VirtualInputSignal*>(dialog.selectedSignal());
    if (!output_signal)
        return;

    if (!bo_node->cfcOutput())
        return;
    bo_node->cfcOutput()->setTarget(output_signal);
    update();

    return;
}

void CfcScene::onActionParam()
{
    CfcNode* node = dynamic_cast<CfcNode*>(itemAt(menuPoint(), QTransform()));
    if (!node)
        return;

    ParamsDialog dialog(node->paramsList());
    dialog.exec();
    if (dialog.result() != QDialog::Accepted)
        return;

    QList<NodeParam> params = dialog.paramsList();
    for (int i = 0; i < params.count(); i++)
        node->setParam(params.at(i).index, params.at(i).value);
    update();
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void CfcScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    if (!gridEnabled()) return;

    //  Определение переменных
    int left = int(rect.left());
    int right = int(rect.right());
    int top = int(rect.top());
    int bottom = int(rect.bottom());

    int first_top = top - top % grid_step;
    int first_left = left - left % grid_step;

    //  Крупная сетка
    painter->setPen(QPen(grid_color, 2));
    for (int x = first_left; x <= right; x = x + grid_step) painter->drawLine(x, top, x, bottom);
    for (int y = first_top; y <= bottom; y = y + grid_step) painter->drawLine(left, y, right, y);

    //  Мелкая сетка
    painter->setPen(QPen(grid_color, 1));
    for (int y = first_top; y <= bottom; y = y + grid_small_step) painter->drawLine(left, y, right, y);
    for (int x = first_left; x <= right; x = x + grid_small_step) painter->drawLine(x, top, x, bottom);
    update();

    return;
}

void CfcScene::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
}

void CfcScene::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
}

void CfcScene::dropEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mime = event->mimeData();
    if (!mime->hasFormat("Node")) {
        event->ignore();
        return;	}

    QByteArray data = mime->data("Node");
    QString idx = QString(data);
    clearSelection();

    CfcNode* node = newEditorNode(idx);
    if (!node) {
        Dpc::Gui::MsgBox::error(QString("Ошибка создания графического элемента %1").arg(idx));
        return;	}

    QSizeF size = node->size();
    QPointF position = QPointF(event->scenePos().x() - size.width() / 2, event->scenePos().y() - size.height() / 2);
    node->setPos(position);
    node->setSelected(false);
    addItem(node);

    return;
}

void CfcScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //  Вывод меню изменения связи
    QPointF position = QPointF(event->scenePos().x(), event->scenePos().y());
    CfcLink* link = dynamic_cast<CfcLink*>(itemAt(position, QTransform()));
    CfcTitleItem* title = dynamic_cast<CfcTitleItem*>(itemAt(position, QTransform()));
    if (link && event->button() == Qt::RightButton) {
        link->mousePressEvent(event);
        event->accept();
        return;
    }
    if (title && event->button() == Qt::RightButton) {
        title->mousePressEvent(event);
        event->accept();
        return;
    }

    //  Костыль для вывода контекстного меню
    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }

    //  Начало рисования нового соединения
    CfcSocket* socket = dynamic_cast<CfcSocket*>(itemAt(position, QTransform()));
    if ((event->buttons() & Qt::LeftButton) && socket) {
        if (socket->socketType() == CfcSocket::INPUT_SOCKET && socket->links().count() > 0) {
            Dpc::Gui::MsgBox::error(QString("Данный вход уже используется."));
            QGraphicsScene::mousePressEvent(event);
            return;
        }

        //  Добавление новой связи
        clearSelection();
        _new_link = new CfcNewLink(socket/*, position*/);
        addItem(_new_link);
        event->accept();

        return;
    }
    QGraphicsScene::mousePressEvent(event);
}

void CfcScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    //  Рисование нового соединения
    if (_new_link && (event->buttons() & Qt::LeftButton)) {
        _new_link->mouseMoveEvent(event);
        QGraphicsScene::mouseMoveEvent(event);
        return;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void CfcScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    //  Завершение отрисовки новой связи между элементами
    if (_new_link && event->button() == Qt::LeftButton) {
        _new_link->mouseReleaseEvent(event);

        if (_new_link->source() && _new_link->target()) {
            CfcLink* link = new CfcLink(QString(), _new_link->points());
            link->setSource(_new_link->source());
            link->setTarget(_new_link->target());
            link->source()->appendLink(link);
            link->target()->appendLink(link);
            addItem(link);
        }

        removeItem(_new_link);
        delete _new_link;
        _new_link = nullptr;
        event->accept();
    }

    QGraphicsScene::mouseReleaseEvent(event);

}

void CfcScene::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    //  Проверка наличия узла
    QPointF position = QPointF(event->scenePos().x(), event->scenePos().y());
    setMenuPoint(position);
    CfcNode* node = dynamic_cast<CfcNode*>(itemAt(position, QTransform()));

    //  Выделение узла
    if (node && !node->isSelected()) {
        clearSelection();
        node->setSelected(true);
    }

    //  Формирование нового меню
    QMenu menu;
    if (node) {
        if (node->name() == "BI") {
            menu.addAction(_action_bi);
            menu.addSeparator();
        }
        if (node->name() == "BO") {
            menu.addAction(_action_bo);
            menu.addSeparator();
        }

        if (node->name() != "BO" && node->name() != "BI" && node->paramsList().count() > 0) {
            menu.addAction(_action_param);
            menu.addSeparator();
        }
    }
    menu.addActions(contextMenu()->actions());
    menu.exec(event->screenPos());

    QGraphicsScene::contextMenuEvent(event);

    return;
}










