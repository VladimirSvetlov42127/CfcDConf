#include "cfc_scene.h"
#include <cmath>

#include <QDebug>
#include <QColor>
#include <QMimeData>
#include <QByteArray>
#include <QSizeF>
#include <QGraphicsView>
#include <QCursor>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "service_manager/services/alg_cfc/cfc_service_input.h"
#include "service_manager/services/alg_cfc/cfc_service_output.h"
#include "gui/forms/algorithms/custom/cfc_nodes/cfc_bi.h"
#include "gui/forms/algorithms/custom/cfc_nodes/cfc_bo.h"
#include "gui/dialogs/params_dialog.h"
#include "gui/dialogs/binding_dialog.h"


//===================================================================================================================================================
//	список переменных
//===================================================================================================================================================
// namespace {
//     uint8_t grid_step = 50;
//     uint8_t grid_small_step = 10;
//     QColor grid_color = QColor(200, 200, 255, 125);
// }


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
CfcScene::CfcScene(CfcAlgService* service, SignalManager *signalManager, QGraphicsScene* parent)
    : CfcBasicScene(service, parent)
    , _signal_manager{signalManager}
    , _new_link { nullptr }
    , _last_socket { nullptr }
{
    //  Параметры сетки
    CfcNamespace::grid_enable = true;

    //  Параметры меню
    _action_bi = new QAction("Привязать сигнал", this);
    _action_bo = new QAction("Привязать сигнал", this);
    _action_remove_bi = new QAction("Отвязать сигнал", this);
    _action_remove_bo = new QAction("Отвязать сигнал", this);
    _action_param = new QAction("Параметры", this);

    connect(_action_bi, &QAction::triggered, this, &CfcScene::onActionBI);
    connect(_action_bo, &QAction::triggered, this, &CfcScene::onActionBO);
    connect(_action_remove_bi, &QAction::triggered, this, &CfcScene::onActionRemoveBI);
    connect(_action_remove_bo, &QAction::triggered, this, &CfcScene::onActionRemoveBO);
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

    BindingDialog dialog(BindingDialog::TYPE_INPUT, signalManager());
    if (dialog.exec() != QDialog::Accepted)
        return;

    auto input_signal = dialog.selectedSignal();
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

    BindingDialog dialog(BindingDialog::TYPE_OUTPUT, signalManager());
    if (dialog.exec() != QDialog::Accepted)
        return;

    auto output_signal = dynamic_cast<DinVirtualSignal*>(dialog.selectedSignal());
    if (!output_signal)
        return;

    if (!bo_node->cfcOutput())
        return;
    bo_node->cfcOutput()->setTarget(output_signal);
    update();

    return;
}

void CfcScene::onActionRemoveBI()
{
    CfcBI* bi_node = dynamic_cast<CfcBI*>(itemAt(menuPoint(), QTransform()));
    if (!bi_node)
        return;
    if (!bi_node->cfcInput())
        return;

    bi_node->cfcInput()->setSource(nullptr);
    update();

    return;
}

void CfcScene::onActionRemoveBO()
{
    CfcBO* bo_node = dynamic_cast<CfcBO*>(itemAt(menuPoint(), QTransform()));
    if (!bo_node)
        return;
    if (!bo_node->cfcOutput())
        return;

    bo_node->cfcOutput()->setTarget(nullptr);
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

    //  Крупная сетка
    QPen(QColor(CfcNamespace::grid_color), 2, Qt::SolidLine);
    painter->setPen(QPen(QColor(CfcNamespace::grid_color), 2, Qt::SolidLine));
    qreal left = std::floor(rect.left() / CfcNamespace::grid_step) * CfcNamespace::grid_step;
    qreal top = std::floor(rect.top() / CfcNamespace::grid_step) * CfcNamespace::grid_step;

    for (int x = left; x < rect.right(); x += CfcNamespace::grid_step)
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    for (int y = top; y < rect.bottom(); y += CfcNamespace::grid_step)
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));

    //  Мелкая сетка
    painter->setPen(QPen(QColor(CfcNamespace::grid_color), 1, Qt::SolidLine));
    left = std::floor(rect.left() / CfcNamespace::grid_small_step) * CfcNamespace::grid_small_step;
    top = std::floor(rect.top() / CfcNamespace::grid_small_step) * CfcNamespace::grid_small_step;
    for (int x = left; x < rect.right(); x += CfcNamespace::grid_small_step)
        painter->drawLine(QPointF(x, rect.top()), QPointF(x, rect.bottom()));
    for (int y = top; y < rect.bottom(); y += CfcNamespace::grid_small_step)
        painter->drawLine(QPointF(rect.left(), y), QPointF(rect.right(), y));
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
    if (CfcNamespace::grid_enable)
        position = CfcNamespace::gridPoint(position);
    node->setPos(position);
    node->setSelected(false);
    addItem(node);

    return;
}

void CfcScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    //  Костыль для вывода контекстного меню
    if (event->button() == Qt::RightButton) {
        event->accept();
        return;
    }

    //  Начало рисования нового соединения
    CfcSocket* socket = dynamic_cast<CfcSocket*>(itemAt(event->scenePos(), QTransform()));
    if ((event->buttons() & Qt::LeftButton) && socket) {
        if (socket->socketType() == CfcSocket::INPUT_SOCKET && socket->links().count() > 0) {
            Dpc::Gui::MsgBox::error(QString("Данный вход уже используется."));
            QGraphicsScene::mousePressEvent(event);
            return;
        }

        //  Добавление новой связи
        clearSelection();
        _new_link = new CfcNewLink(socket);
        _last_socket = socket;
        addItem(_new_link);
        _last_socket = socket;
        _last_socket->setHighLight(true);
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
        CfcSocket* socket = dynamic_cast<CfcSocket*>(itemAt(event->scenePos(), QTransform()));
        if (socket == nullptr && _last_socket != nullptr) {
            _last_socket->setHighLight(false);
            _last_socket = nullptr;
        }
        if (socket != nullptr && _last_socket == nullptr) {
            _last_socket = socket;
            _last_socket->setHighLight(true);
        }
        event->accept();
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
        if (_last_socket)
            _last_socket->setHighLight(false);
        _last_socket = nullptr;
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
            CfcBI* bi_node = dynamic_cast<CfcBI*>(itemAt(menuPoint(), QTransform()));
            if (!bi_node->cfcInput())
                return;
            if (bi_node->cfcInput()->source()) menu.addAction(_action_remove_bi);
            menu.addAction(_action_bi);
            menu.addSeparator();
        }
        if (node->name() == "BO") {
            CfcBO* bo_node = dynamic_cast<CfcBO*>(itemAt(menuPoint(), QTransform()));
            if (!bo_node->cfcOutput())
                return;
            if (bo_node->cfcOutput()->target()) menu.addAction(_action_remove_bo);
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









