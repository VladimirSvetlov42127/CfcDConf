#include "flex_editor_form.h"


//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QApplication>
#include <QIcon>
#include <QFile>
#include <QSplitter>
#include <QToolBar>
#include <QLabel>
#include <QDebug>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <file_managers/DcFlexLogicFileManager.h>
#include <gui/forms/algorithms/custom/flexlogic_namespace.h>
#include <gui/forms/algorithms/custom/Editor/editor_model.h>
#include "gui/forms/algorithms/custom/Nodes/bo_node.h"
#include "gui/forms/algorithms/custom/Nodes/bi_node.h"
#include "gui/forms/algorithms/custom/Editor/editor_node.h"


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
flexEditorForm::flexEditorForm(DcController* device, CfcAlgService *cfcAlg, QWidget* parent)
    : QMainWindow(parent)
    , _cfcAlg{cfcAlg}
    , _hasError{false}
    , _scale_factor{1.0}
{
	//	Установка цветовой палитры
	QPalette palette = qApp->palette();
	QColor color = palette.color(QPalette::Highlight);
	color.setAlpha(80);
	palette.setColor(QPalette::Highlight, color);
	qApp->setPalette(palette);

	//	Настройка главного окна
	setWindowIcon(QIcon(":/icons/algorithm_custom.svg"));
	setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	QApplication::setWindowIcon(windowIcon());
    setWindowTitle(QString("Контроллер: %1, Алгоритм: %2").arg(device->name(), cfcAlg->name()));

	//	Формирование графической области
    _scene = new EditorScene(this->cfcAlg(), device->serviceManager());
    _graph_view = new CfcView(_scene);

	//	Привязка к сигналам сцены
    // connect(Scene(), &EditorScene::DataChanged, this, &flexEditorForm::DataChange);
    // connect(Scene(), &EditorScene::BufferChanged, this, &flexEditorForm::ButtonsEnableChange);
    // connect(Scene(), &EditorScene::selectionChanged, this, &flexEditorForm::ButtonsEnableChange);
 //    connect(Scene(), &EditorScene::sceneRectChanged, _graph_view, &CfcView::SceneChanged);

	//	Формирование виджетов окна
    // CreateTreeView();
    // CreateToolbar();

	//	Формирование центрального виджета
    // QSplitter* splitter = new QSplitter(Qt::Horizontal);
    // splitter->setContentsMargins(5, 5, 5, 5);
    // splitter->setStretchFactor(1, 1);
    // splitter->addWidget(_tree_menu);
    // splitter->addWidget(_graph_view);
    // setCentralWidget(splitter);

	//	Масштабирование формы
    //QSize size = QDesktopWidget().availableGeometry(this).size(); // use QWidget::screen()
    //resize(size * 0.85);
    // resize(1200, 800);

    //	Вывод элементов сцену
    // auto parser = cfcAlg->parser();
    // for (int i = 0; i < parser->editorNodes().count(); i++) {
    //     auto node = parser->editorNodes().at(i);
    //     if ( node->name() == "BO") {
    //         auto boNode =static_cast<BO*>(node);
    //         if (!boNode->cfcOutput())
    //             continue;
    //     }

    //     if ( node->name() == "BI") {
    //         auto biNode = static_cast<BI*>(node);
    //         if (!biNode->cfcInput())
    //             continue;
    //     }

    //     Scene()->addItem(node);
    // }

    // for (int i = 0; i < parser->editorLinks().count(); i++)
    //     Scene()->addItem(parser->editorLinks().at(i));
}

flexEditorForm::~flexEditorForm()
{
	if (!_tree_menu) delete _tree_menu;
	if (!_scene) delete _scene;
	if (!_graph_view) delete _graph_view;
}

void flexEditorForm::ZoomChange(int flag)
{
	//	Сброс масштабирования
    // if (flag == 0) {
    // 	GraphView()->resetTransform();
    // 	_scale_factor = 1.0;
    // 	_zoom_edit->setText(QString::number(_scale_factor * 100) + "%");
    // 	return; }

    // //	Изменение масштабирования
    // double factor = (_scale_factor - 0.2 * flag / abs(flag)) / _scale_factor;
    // if (factor * _scale_factor < 0.1 || factor * _scale_factor > 100) return;
    // _scale_factor *= factor;
    // _zoom_edit->setText(QString::number(_scale_factor * 100) + "%");
    // GraphView()->scale(factor, factor);

	return;
}


//===================================================================================================================================================
//	Методы работы с формой
//===================================================================================================================================================
void flexEditorForm::CreateTreeView()
{
    // _tree_menu = new QTreeView;
    // int width = 250;
    // _tree_menu->setMinimumWidth(width);
    // _tree_menu->setMaximumWidth(width);
    // _tree_menu->setDragEnabled(true);
    // _tree_menu->setDragDropMode(QAbstractItemView::DragOnly);
    // _tree_menu->setDefaultDropAction(Qt::CopyAction);
    // _tree_menu->setSelectionMode(QAbstractItemView::SingleSelection);
    // _tree_menu->setSelectionBehavior(QAbstractItemView::SelectItems);
    // _tree_menu->setIconSize(QSize(24, 24));
    // _tree_menu->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    // _tree_menu->setHeaderHidden(true);
    // _tree_menu->setModel(new EditorModel(this));
    // _tree_menu->expandAll();
    // connect(_tree_menu, &QTreeView::doubleClicked, this, &flexEditorForm::TreeDoubleClicked);

	return;
}

void flexEditorForm::CreateToolbar()
{
	//	Создание тулбара
    // int icon_size = 24;
 //    QToolBar* tool_bar = new QToolBar(this);
    // tool_bar->setMinimumSize(QSize(0, 0));
    // tool_bar->setBaseSize(QSize(0, icon_size + 5));
    // tool_bar->setMovable(false);
    // tool_bar->setAllowedAreas(Qt::TopToolBarArea);
    // tool_bar->setIconSize(QSize(icon_size, icon_size));
    // this->addToolBar(Qt::TopToolBarArea, tool_bar);

    // //	Кнопки панели инструментов
    // tool_bar->addSeparator();
    // QAction* grid_action = new QAction(QIcon(":/icons/grid.svg"), "Сетка", tool_bar);
    // grid_action->setCheckable(true);
    // tool_bar->addAction(grid_action);
    // tool_bar->addSeparator();
    // tool_bar->addWidget(new QLabel(""));

    // QAction* zoom_out_action = new QAction(QIcon(":/icons/zoom_minus.svg"), "Уменьшить", tool_bar);
    // tool_bar->addAction(zoom_out_action);

    // _zoom_edit = new QLineEdit(tool_bar);
    // _zoom_edit->setMaximumWidth(50);
    // _zoom_edit->setMinimumHeight(20);
    // _zoom_edit->setReadOnly(true);
    // _zoom_edit->setAlignment(Qt::AlignCenter);
    // _zoom_edit->setText(QString::number(_scale_factor * 100) + "%");
    // tool_bar->addWidget(_zoom_edit);

    // QAction* zoom_in_action = new QAction(QIcon(":/icons/zoom_plus.svg"), "Увеличить", tool_bar);
    // tool_bar->addAction(zoom_in_action);
    // QAction* zoom_default_action = new QAction(QIcon(":/icons/zoom_def.svg"), "По умолчанию", tool_bar);
    // tool_bar->addAction(zoom_default_action);
    // tool_bar->addSeparator();
    // tool_bar->addWidget(new QLabel(""));

 //    _append_connector_action = new QAction(QIcon(":/icons/function_add.svg"), "Добавить конектор", tool_bar);
    // _append_connector_action->setEnabled(false);
    // tool_bar->addAction(_append_connector_action);
 //    _remove_connector_action = new QAction(QIcon(":/icons/function_del.svg"), "Удалить конектор", tool_bar);
    // _remove_connector_action->setEnabled(false);
    // tool_bar->addAction(_remove_connector_action);
    // tool_bar->addSeparator();
    // tool_bar->addWidget(new QLabel(""));

 //    _delete_action = new QAction(QIcon(":/icons/del_1.svg"), "Удалить", tool_bar);
    // _delete_action->setShortcut(QKeySequence::Delete);
    // _delete_action->setEnabled(false);
    // tool_bar->addAction(_delete_action);
    // tool_bar->addSeparator();
    // tool_bar->addWidget(new QLabel(""));

    // _copy_action = new QAction(QIcon(":/icons/copy.svg"), "Копировать", tool_bar);
    // _copy_action->setShortcut(QKeySequence::Copy);
    // _copy_action->setEnabled(false);
    // tool_bar->addAction(_copy_action);

    // _cut_action = new QAction(QIcon(":/icons/cut.svg"), "Вырезать", tool_bar);
    // _cut_action->setShortcut(QKeySequence::Cut);
    // _cut_action->setEnabled(false);
    // tool_bar->addAction(_cut_action);

    // _paste_action = new QAction(QIcon(":/icons/paste.svg"), "Вставить", tool_bar);
    // _paste_action->setShortcut(QKeySequence::Paste);
    // _paste_action->setEnabled(false);
    // tool_bar->addAction(_paste_action);
    // tool_bar->addSeparator();

    // //	Привязка сигналов кнопок тулбара
    // connect(grid_action, &QAction::toggled, this, &flexEditorForm::SetGridEnable);
    // connect(zoom_in_action, &QAction::triggered, this, &flexEditorForm::ZoomInClicked);
    // connect(zoom_out_action, &QAction::triggered, this, &flexEditorForm::ZoomOutClicked);
    // connect(zoom_default_action, &QAction::triggered, this, &flexEditorForm::ZoomDefaultClicked);
    // connect(_append_connector_action, &QAction::triggered, Scene(), &EditorScene::AddInputs);
    // connect(_remove_connector_action, &QAction::triggered, Scene(), &EditorScene::RemoveInputs);

    // connect(_copy_action, &QAction::triggered, Scene(), &EditorScene::Copy);
    // connect(_cut_action, &QAction::triggered, Scene(), &EditorScene::Cut);
    // connect(_paste_action, &QAction::triggered, Scene(), &EditorScene::Paste);
    // connect(_delete_action, &QAction::triggered, Scene(), &EditorScene::Delete);

    // //	Создание меню
    // QMenu* contens_menu = new QMenu(this);
    // contens_menu->addAction(_delete_action);
    // contens_menu->addSeparator();
    // contens_menu->addAction(_copy_action);
    // contens_menu->addAction(_cut_action);
    // contens_menu->addAction(_paste_action);
    // Scene()->SetMenu(contens_menu);

	return;
}


//===================================================================================================================================================
//	Методы обработки сигналов формы
//===================================================================================================================================================
void flexEditorForm::onSaveAction()
{
    // auto parser = cfcAlg()->parser();
    // parser->setData(Scene()->Nodes(), Scene()->Links());
    // if (!cfcAlg()->save()) {
    //     _hasError = true;
    //     _errorString = parser->errors().join("\n");
    // }
}

void flexEditorForm::ButtonsEnableChange()
{
    // int links = Scene()->SelectedLinks().count();
    // int nodes = Scene()->SelectedNodes().count();
    // nodes > 0 ? _append_connector_action->setEnabled(true) : _append_connector_action->setEnabled(false);
    // nodes > 0 ? _remove_connector_action->setEnabled(true) : _remove_connector_action->setEnabled(false);
    // nodes > 0 ? _copy_action->setEnabled(true) : _copy_action->setEnabled(false);
    // nodes > 0 ? _cut_action->setEnabled(true) : _cut_action->setEnabled(false);
    // links > 0 || nodes > 0 ? _delete_action->setEnabled(true) : _delete_action->setEnabled(false);
    // Scene()->BufferCount() > 0 ? _paste_action->setEnabled(true) : _paste_action->setEnabled(false);

	return;
}

void flexEditorForm::TreeDoubleClicked(const QModelIndex& index)
{
	//	Проверка входных данных
    // QString data = index.data(Qt::UserRole).toString();
    // if (data == QString()) return;

    // //	Создание элемента и вывод его на экран
    // Scene()->clearSelection();
    // EditorNode* node = Scene()->NewEditorNode(data);
    // if (!node) {
    // 	Dpc::Gui::MsgBox::error(QString("Ошибка создания графического элемента %1").arg(data));
    // 	delete node;
    // 	return;	}

    // QPointF center = GraphView()->mapToScene(GraphView()->viewport()->rect().center());
    // node->setPos(center);
    // node->setSelected(true);
    // Scene()->addItem(node);
    // _data_changed = true;

	return;
}


//===================================================================================================================================================
//	Перегружаемые методы класса
//===================================================================================================================================================
void flexEditorForm::closeEvent(QCloseEvent* event)
{
    // onSaveAction();
    // emit aboutToClose();
}

void flexEditorForm::keyPressEvent(QKeyEvent* event)
{
    // if (event->key() == Qt::Key_G && (event->modifiers() & Qt::ControlModifier)) {
    // 	FlexLogic::grid_enable = !FlexLogic::grid_enable;
    // 	Scene()->update(); }

    // //	Ctrl +/- Увеличение/уменьшение масштаба
    // if (event->key() == Qt::Key_Plus && (event->modifiers() & Qt::ControlModifier)) ZoomChange(-1);
    // if (event->key() == Qt::Key_Minus && (event->modifiers() & Qt::ControlModifier)) ZoomChange(1);

    // //	Ctrl A Выделить все
    // if (event->key() == Qt::Key_A && (event->modifiers() & Qt::ControlModifier)) {
    // 	QList<EditorNode*> nodes = Scene()->Nodes();
    // 	for (int i = 0; i < nodes.count(); i++) nodes.at(i)->setSelected(true);
    // 	QList<EditorLink*> links = Scene()->Links();
    // 	for (int i = 0; i < links.count(); i++) links.at(i)->setSelected(true);	}

    QMainWindow::keyPressEvent(event);

	return;
}

void flexEditorForm::wheelEvent(QWheelEvent* event)
{
    // if (event->modifiers() & Qt::ControlModifier) {
    // 	int delta = event->angleDelta().y();
    // 	ZoomChange(-delta);	}

    QWidget::wheelEvent(event);
}
