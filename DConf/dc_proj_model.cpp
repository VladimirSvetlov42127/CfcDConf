#include "dc_proj_model.h"

#include <QApplication>
#include <QMenu>
#include <QHeaderView>
#include <QFileDialog>
#include <QItemSelectionModel>
#include <QDirIterator>
#include <QStyledItemDelegate>
#include <QBoxLayout>

#include <dpc/helper.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "gui/dialogs/add_controller.h"
#include "utils/cfg_path.h"
#include "utils/qzipwriter_p.h"
#include "utils/qzipreader_p.h"
#include "project/dc_project.h"
#include "project/dc_device_node.h"

using namespace std;
using namespace Dpc::Gui;

namespace 
{
#ifdef QT_DEBUG
    constexpr const char*  DVIEW_PATH = "dview.exe";
#else
	const QString DVIEW_PATH =	"dview.exe";
#endif

    constexpr const char* TEXT_ALREADY_RUNING = "Программа уже запущена!";
    constexpr const char* default_filder_name = "Новая папка";

    enum {
        ProgressRole = Qt::UserRole + 2,
        ProgressHasValueRole,
        ProgressValueRole,
        NodeRole,
        TextBackup
    };

	class ProgressDelegate : public QStyledItemDelegate
	{
		//Q_OBJECT

	public:
		ProgressDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) { }

		void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override
		{
			if (!index.data(ProgressRole).toBool()) {
				QStyledItemDelegate::paint(painter, option, index);				
				return;
			}			

			QStyleOptionProgressBar progressBarOption;
			progressBarOption.state = QStyle::State_Enabled | QStyle::State_Children |
				QStyle::State_Horizontal | QStyle::State_Item;
			progressBarOption.direction = QApplication::layoutDirection();
			progressBarOption.rect = option.rect;
            progressBarOption.fontMetrics = QApplication::fontMetrics();
			progressBarOption.minimum = 0;
			progressBarOption.maximum = index.data(ProgressHasValueRole).toBool() ? 100 : 0;
			progressBarOption.progress = index.data(ProgressValueRole).toInt();
			progressBarOption.text = index.data(Qt::DisplayRole).toString();
			progressBarOption.textAlignment = Qt::AlignCenter;
			progressBarOption.textVisible = true;
			QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);
		}
	};

    DcProject* getProject(DcNode* node)
    {
        if (!node)
            return nullptr;

        if (node->type() == DcNode::ProjectType)
            return static_cast<DcProject*>(node);

        return getProject(node->parent());
    }
}

DcProjModel::DcProjModel(QWidget *parent)
    : QWidget{parent}
    , m_tree{new QTreeView(this) }
    , m_model{new QStandardItemModel(m_tree) }
    , m_processDView{ new QProcess(this) }
    , m_nodeToCopy{ nullptr }    
{
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(QMargins(0, 0, 0, 0));
    layout->addWidget(m_tree);

    connect(m_model, &QStandardItemModel::itemChanged, this, &DcProjModel::onItemChanged);

    m_tree->setModel(m_model);
    m_tree->setIconSize(QSize(24, 24));
	m_tree->setContextMenuPolicy(Qt::CustomContextMenu);
	m_tree->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tree->setEditTriggers(QAbstractItemView::EditKeyPressed);
    m_tree->setItemDelegateForColumn(0, new ProgressDelegate(this));
    m_tree->header()->hide();
    connect(m_tree, &QTreeView::doubleClicked, this, &DcProjModel::onTreeDoubleClicked);
    connect(m_tree, &QTreeView::customContextMenuRequested, this, &DcProjModel::onCustomContextMenu);
//    connect(m_tree->selectionModel(), &QItemSelectionModel::selectionChanged, this, &DcProjModel::onTreeSelection);

	// Определения действий
    m_settingsAction = new QAction("Свойства", this);
    connect(m_settingsAction, &QAction::triggered, this, &DcProjModel::onSettingsAction);
    m_renameItem = new QAction("Переименовать", this);
    connect(m_renameItem, SIGNAL(triggered()), this, SLOT(slotRenameItem()));
    m_removeItem = new QAction("Удалить", this);
    connect(m_removeItem, SIGNAL(triggered()), this, SLOT(slotRemoveItem()));
    m_addFolder = new QAction("Добавить папку", this);
    connect(m_addFolder, SIGNAL(triggered()), this, SLOT(slotAddFolder()));
    m_addController = new QAction("Добавить устройство", this);
    connect(m_addController, SIGNAL(triggered()), this, SLOT(slotAddController()));
    m_copyController = new QAction("Копировать", this);
    connect(m_copyController, SIGNAL(triggered()), this, SLOT(slotCopyController()));
    m_exportController = new QAction("Экспорт", this);
    connect(m_exportController, SIGNAL(triggered()), this, SLOT(slotExportController()));
    m_importController = new QAction("Импорт устройства", this);
    connect(m_importController, SIGNAL(triggered()), this, SLOT(slotImportController()));
    m_pasteController = new QAction("Вставить устройство", this);
    connect(m_pasteController, SIGNAL(triggered()), this, SLOT(slotPasteController()));
    m_pasteController->setEnabled(false);

    m_closeAction = new QAction("Закрыть проект", this);
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(slotCloseProject()));

    m_openDView = new QAction("Открыть DView", this);
    connect(m_openDView, SIGNAL(triggered()), this, SLOT(slotOpenDView()));
}

DcProjModel::~DcProjModel()
{
    closeAll();
}

bool DcProjModel::contains(DcProject *project) const
{
    return m_openedProjects.contains(project);
}

DcNode *DcProjModel::selectedNode() const
{
    return getNode(m_model->itemFromIndex(m_tree->currentIndex()));
}

void DcProjModel::openProject(DcProject *project)
{
    if (!project || contains(project))
        return;

    m_openedProjects.append(project);
    connect(project, &DcNode::nameChanged, this, &DcProjModel::onProjectNameChanged);

    auto project_view_item = makeViewItem(project, nullptr);
    for (size_t i = 0; i < project->childsSize(); ++i)
        fillProjTreeItem(project->child(i), project_view_item);

    auto index = project_view_item->index();
    m_tree->expand(index);
    m_tree->scrollTo(index, QAbstractItemView::PositionAtCenter);
    m_tree->setCurrentIndex(index);
}

void DcProjModel::closeProject(DcProject *project)
{
    if (!contains(project))
        return;

    disconnect(project, &DcNode::nameChanged, this, &DcProjModel::onProjectNameChanged);
    for(int i = 0; i < m_model->rowCount(); ++i) {
        auto view_item = m_model->item(i, 0);
        if (getNode(view_item) == project)
            close(view_item, false);
    }

    m_openedProjects.removeAll(project);
}

void DcProjModel::closeAll()
{
    while(m_openedProjects.size())
        closeProject(m_openedProjects.first());
}

void DcProjModel::onCustomContextMenu(const QPoint &point)
{
    auto node = getNode(m_model->itemFromIndex(m_tree->indexAt(point)));
    if (!node)
        return;

    auto menu = new QMenu(this);

    if (DcNode::ProjectType == node->type()) {
        menu->addAction(m_settingsAction);
        menu->addSeparator();
        menu->addAction(m_addFolder);
        menu->addAction(m_addController);
        menu->addAction(m_importController);
        menu->addAction(m_pasteController);
        menu->addSeparator();
        menu->addAction(m_closeAction);
    }
    if (DcNode::DirType == node->type()) {
        menu->addAction(m_renameItem);
        menu->addSeparator();
        menu->addAction(m_addFolder);
        menu->addAction(m_addController);
        menu->addAction(m_importController);
        menu->addAction(m_pasteController);
        menu->addSeparator();
        menu->addAction(m_removeItem);
    }
    if (DcNode::DeviceType == node->type()) {
        menu->addAction(m_renameItem);
        menu->addSeparator();
        menu->addAction(m_copyController);
        menu->addAction(m_exportController);
        menu->addSeparator();
        menu->addAction(m_openDView);
        menu->addSeparator();
        menu->addAction(m_removeItem);
    }

    menu->popup(m_tree->viewport()->mapToGlobal(point));
}

void DcProjModel::fillProjTreeItem(DcNode *node, QStandardItem* parent_view_item)
{
    if (!node)
        return;

    auto view_item = makeViewItem(node, parent_view_item);
    for (size_t i = 0; i < node->childsSize(); ++i)
        fillProjTreeItem(node->child(i), view_item);
}

//void DcProjModel::onTreeSelection(const QItemSelection &sel, const QItemSelection &desel)
//{
//}

void DcProjModel::onProjectNameChanged(const QString &name)
{
    auto node = dynamic_cast<DcNode*>(sender());
    if (!node)
        return;

    for(int i = 0; i < m_model->rowCount(); ++i) {
        auto item = m_model->item(i, 0);
        if (getNode(item) == node)
            item->setText(name);
    }
}

void DcProjModel::close(QStandardItem *view_item, bool remove)
{
    if (!view_item)
        return;

    for(int i = view_item->rowCount() - 1; 0 <= i ; --i)
        close(view_item->child(i), remove);

    auto node = getNode(view_item);
    if (auto parent_view_item = view_item->parent(); parent_view_item)
        parent_view_item->removeRow(view_item->row());
    else
        m_model->removeRow(view_item->row());

    if (!node)
        return;

    if (node == m_nodeToCopy) {
        m_nodeToCopy = nullptr;
        m_pasteController->setEnabled(false);
    }
    emit aboutToClose(node);

    if (remove && node->parent()) {        
        if (auto project = getProject(node); project)
            project->removeNodeData(node);

        node->parent()->removeChild(node->position());
    }
}

DcNode *DcProjModel::createDeviceNode(const QString &nameTemplate, DcNode *parent_node)
{
    auto project_node = getProject(parent_node);
    if (!project_node)
        return nullptr;

    auto node = project_node->createNode(nameTemplate, DcNode::DeviceType, parent_node);
    if (!node)
        return nullptr;

    node->setName(QString("%1 (%2)").arg(nameTemplate).arg(node->id()));
    project_node->save();
    return node;
}

void DcProjModel::slotAddFolder()
{
    addFolder(m_model->itemFromIndex(m_tree->currentIndex()));
}

void DcProjModel::slotAddController()
{
    addDevice(m_model->itemFromIndex(m_tree->currentIndex()));
}

void DcProjModel::slotRenameItem()
{
    QStandardItem* pitem = m_model->itemFromIndex(m_tree->currentIndex());
    if (!pitem)
        return;

    pitem->setData(pitem->text(), TextBackup);
    m_tree->edit(pitem->index());
}

void DcProjModel::slotRemoveItem()
{
    QStandardItem* pitem = m_model->itemFromIndex(m_tree->currentIndex());
    if (!pitem)
        return;

    if (!MsgBox::question(tr("Вы уверены, что хотите удалить этот элемент?")))
        return;

    auto project_node = getProject(getNode(pitem));
    close(pitem, true);    
    if (!project_node)
        return;

    project_node->save();
}

void DcProjModel::slotCopyController()
{
    auto node = getNode(m_model->itemFromIndex(m_tree->currentIndex()));
    if (!node || node->type() != DcNode::DeviceType)
		return;
	
    m_nodeToCopy = node;
    m_pasteController->setEnabled(true);
}

void DcProjModel::slotExportController()
{
    auto node = getNode(m_model->itemFromIndex(m_tree->currentIndex()));
    if (!node || DcNode::DeviceType != node->type()) {
        MsgBox::error(QString("Возможно экспортировать только контроллеры"));
        return;
    }

	QString filename = QFileDialog::getSaveFileName(nullptr, "Экспорт Устройства",	QDir::currentPath(), "Проекты (*.dunit)");
	if (filename.isNull())
		return;

	QZipWriter zip(filename);
	if (zip.status() != QZipWriter::NoError)
		return;
	zip.setCompressionPolicy(QZipWriter::AutoCompress);

    auto device_node = static_cast<DcDeviceNode*>(node);
    QString unitpath = device_node->path();
	QDirIterator it(unitpath, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
	while (it.hasNext()) {
		QString file_path = it.next();

		QString projDirModify = unitpath;	// Проблема в том, что путь может быть либо с "\", либо с "/". И тогда путь не удалится в file_path.remove(projDirModify) 
		if (projDirModify.contains('/') && projDirModify.contains('\\')) {
			while (projDirModify.contains('\\')) {
				projDirModify[projDirModify.indexOf('\\')] = '/';
			}
		}

		if (it.fileInfo().isDir()) {
			zip.setCreationPermissions(QFile::permissions(file_path));
			zip.addDirectory(file_path.remove(projDirModify));
		}
		else if (it.fileInfo().isFile()) {
			QFile file(file_path);
			if (!file.open(QIODevice::ReadOnly))
				continue;

			zip.setCreationPermissions(QFile::permissions(file_path));
			QByteArray ba = file.readAll();
			zip.addFile(file_path.remove(projDirModify), ba);
		}
	}
	zip.close();
}

void DcProjModel::slotImportController()
{
    QStandardItem* parent_view_item = m_model->itemFromIndex(m_tree->currentIndex());
    auto parent_node = getNode(parent_view_item);
    if (!parent_node)
        return;

    if (!(DcNode::DirType == parent_node->type() || DcNode::ProjectType == parent_node->type())) {
        MsgBox::error(QString("Некорректный тип узла для импорта"));
        return;
    }

    QString filename = QFileDialog::getOpenFileName(nullptr, "Импорт проекта", QDir::currentPath(),	"Проекты (*.dunit)");
    if (filename.isNull())
        return;

    QZipReader unzip(filename, QIODevice::ReadOnly);
    if (!unzip.exists()) {
        MsgBox::error(QString("Некорректный путь к архиву устройства"));
        return;
    }

    auto node = createDeviceNode(QFileInfo(filename).baseName(), parent_node);
    if (!node)
        return;

    auto device_node = static_cast<DcDeviceNode*>(node);
    QString controllerpath = device_node->path();
    QList<QZipReader::FileInfo> allFiles = unzip.fileInfoList();
    QZipReader::FileInfo fi;
    foreach(QZipReader::FileInfo fi, allFiles) {
        const QString absPath = controllerpath + fi.filePath;
        if (fi.isDir) {
            if (!QDir().mkdir(controllerpath + fi.filePath)) {
                MsgBox::error(QString("Ошибка распаковки архива устройства 1"));
                return;
            }
            if (!QFile::setPermissions(absPath, fi.permissions)) {
                MsgBox::error(QString("Ошибка распаковки архива устройства 2"));
                return;
            }
        }
    }

    foreach(QZipReader::FileInfo fi, allFiles) {
        const QString absPath = controllerpath + fi.filePath;
        if (fi.isFile) {
            QFile file(absPath);
            if (file.open(QFile::WriteOnly)){
                QApplication::setOverrideCursor(Qt::WaitCursor);
                file.write(unzip.fileData(fi.filePath), unzip.fileData(fi.filePath).size());
                file.setPermissions(fi.permissions);
                QApplication::restoreOverrideCursor();
                file.close();
            }
        }
    }
    unzip.close();

    emit activate(node);
    makeViewItem(node, parent_view_item);
}

void DcProjModel::slotPasteController()
{
    if (!m_nodeToCopy)
        return;

    QStandardItem* parent_view_item = m_model->itemFromIndex(m_tree->currentIndex());
    auto parent_node = getNode(parent_view_item);
    if (!parent_node)
        return;

    if ( DcNode::DeviceType == parent_node->type() )
        return;

    auto node = createDeviceNode(m_nodeToCopy->name(), parent_node);
    if (!node)
        return;

    auto device_node = static_cast<DcDeviceNode*>(node);
    auto source_device_node = static_cast<DcDeviceNode*>(m_nodeToCopy);
    QString contrdstpath = device_node->path();
    QString contrsrcpath = source_device_node->path();
    if (!cfg::path::copy(contrsrcpath, contrdstpath)) {
        MsgBox::error(QString("Ошибка копирования устройства. Не удалось скопировать файлы конфигурации"));
        return;
    }

    emit activate(device_node);
    makeViewItem(node, parent_view_item);    
}

void DcProjModel::slotOpenDView()
{
    if (m_processDView->state() == QProcess::Running) {
		MsgBox::warning(TEXT_ALREADY_RUNING);
		return;
	}

    m_processDView->start(DVIEW_PATH, QStringList());
}

void DcProjModel::slotCloseProject()
{
    QStandardItem* pitem = m_model->itemFromIndex(m_tree->currentIndex());
    if (!pitem)
        return;

    auto project = getProject(getNode(pitem));
    if (!project)
        return;

    closeProject(project);
}

void DcProjModel::onSettingsAction()
{
    auto node = getNode(m_model->itemFromIndex(m_tree->currentIndex()));
    if (!node || node->type() != DcNode::ProjectType)
        return;

    emit activate(node);
}

void DcProjModel::onTreeDoubleClicked(const QModelIndex &index)
{
    auto node = getNode(m_model->itemFromIndex(index));
    if (!node || node->type() != DcNode::DeviceType)
        return;

    emit activate(node);
}

QStandardItem *DcProjModel::makeViewItem(DcNode *node, QStandardItem *parent_view_item)
{
    if (!node)
        return nullptr;

    auto view_item = new QStandardItem{ node->name() };
    view_item->setIcon(node->icon());
    view_item->setFlags(view_item->flags() | Qt::ItemIsEditable);    
    view_item->setData(Dpc::fromPtr(node), NodeRole);

    if (!parent_view_item)
        m_model->appendRow(view_item);
    else
        parent_view_item->appendRow(view_item);

    return view_item;
}

DcNode *DcProjModel::getNode(QStandardItem *view_item, bool checkParent) const
{
    if (view_item) {
        if (auto node = Dpc::toPtr<DcNode>(view_item->data(NodeRole)); node)
            return node;

        if (checkParent)
            return getNode(view_item->parent(), checkParent);
    }

    return nullptr;
}

void DcProjModel::onItemChanged(QStandardItem* view_item)
{
    if (!view_item)
        return;

    auto newText = view_item->text().trimmed();
    if (newText.isEmpty()) {
        auto textBackup = view_item->data(TextBackup).toString();
        view_item->setText(textBackup);
        return;
    }

    auto node = getNode(view_item);
    if (!node || node->name() == newText) // если имя не менялось - ничего не делаем
        return;

    node->setName(newText);

    auto project_node = getProject(node);
    if (!project_node)
        return;

    project_node->save();
}

void DcProjModel::addFolder(QStandardItem *parent_view_item)
{
    auto parent_node = getNode(parent_view_item);
    auto project_node = getProject(parent_node);
    if (!project_node)
        return;

    auto node = project_node->createNode(default_filder_name, DcNode::DirType, parent_node);
    if (!node)
        return;

    project_node->save();
    makeViewItem(node, parent_view_item);
    m_tree->expand(m_model->indexFromItem(parent_view_item));
}

void DcProjModel::addDevice(QStandardItem *parent_view_item)
{
    auto parent_node = getNode(parent_view_item);
    if (!parent_node)
        return;

    // Выбор шаблона конфигурации устройтсва
    AddController dlgcontroller;
    if (dlgcontroller.exec() != QDialog::Accepted)
        return;

    auto node = createDeviceNode(dlgcontroller.deviceType(), parent_node);
    if (!node)
        return;

    // Копирование шаблона конфигурации устройства в папку конфгурации устройства
    auto device_node = static_cast<DcDeviceNode*>(node);
    if (!QFile::copy(dlgcontroller.deviceTemplateFilePath(), device_node->configFilePath())) {
        Dpc::Gui::MsgBox::error(QString("Ошибка создания файла(%1) конфигурации устройства").arg(device_node->configFilePath()));
        return;
    }

    emit activate(device_node);
    makeViewItem(node, parent_view_item);    
}
