#include "filesystem_dialog.h"

#include <QDebug>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QFileDialog>
#include <QBoxLayout>
#include <QInputDialog>
#include <QSettings>
#include <QToolBar>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

#include "device_operations/filesystem_info_operation.h"
#include "device_operations/filesystem_list_operation.h"
#include "device_operations/filesystem_new_operation.h"
#include "device_operations/filesystem_upload_operation.h"
#include "device_operations/filesystem_download_operation.h"
#include "device_operations/filesystem_remove_operation.h"
#include "device_operations/filesystem_format_operation.h"

using namespace Dpc::Sybus;
using namespace Dpc::Gui;

namespace {

} // namespace

FilesystemDiaolog::FilesystemDiaolog(QWidget *parent)
    : QDialog{parent}
    , m_driveInfoLayout{new QGridLayout}
    , m_pathLineEdit{new QLineEdit(this)}
    , m_listWidget{new QListWidget(this)}
{
    setWindowTitle("Файловая система устройства");
    setWindowIcon(QIcon(":/icons/file_system.svg"));

    m_pathLineEdit->setReadOnly(true);

    auto toolBar = new QToolBar(this);
    toolBar->setMovable(false);
    toolBar->setFloatable(false);
//    toolBar->setIconSize(QSize(32, 32));
    toolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    toolBar->setAllowedAreas(Qt::TopToolBarArea);

    m_newAction = toolBar->addAction(QIcon(":/icons/folder_add.svg"), "Создать папку");
    connect(m_newAction, &QAction::triggered, this, &FilesystemDiaolog::onNewDirAction);

    toolBar->addSeparator();
    m_uploadAction = toolBar->addAction(QIcon(":/icons/in.svg"), "Записать в устройство");
    connect(m_uploadAction, &QAction::triggered, this, &FilesystemDiaolog::onUploadAction);

    m_downloadAction = toolBar->addAction(QIcon(":/icons/out.svg"), "Скачать из устройства");
    connect(m_downloadAction, &QAction::triggered, this, &FilesystemDiaolog::onDownloadAction);

    toolBar->addSeparator();
    m_removeAction = toolBar->addAction(QIcon(":/icons/del_1.svg"), "Удалить");
    connect(m_removeAction, &QAction::triggered, this, &FilesystemDiaolog::onRemoveAction);

    toolBar->addSeparator();
    m_formatAction = new QAction(QIcon(":/icons/formatting.svg"), "Форматировать", this);
    connect(m_formatAction, &QAction::triggered, this, &FilesystemDiaolog::onFormatAction);
    toolBar->addAction(m_formatAction);

    m_listWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &FilesystemDiaolog::onItemDoubleClicked);
    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &FilesystemDiaolog::onItemSelectionChanged);

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolBar);
    mainLayout->addLayout(m_driveInfoLayout);
    mainLayout->addWidget(m_pathLineEdit);
    mainLayout->addWidget(m_listWidget);
    resize(400, 500);

    setCurrentPath(QString());
}

FilesystemDiaolog::~FilesystemDiaolog()
{
}

void FilesystemDiaolog::start()
{
    getInfo();
}

void FilesystemDiaolog::operationFinished(AbstractOperation *op)
{
    if ( !(op->flags() & AbstractOperation::Flag::Filesystem) )
        return;

    if (op->flags() & AbstractOperation::Flag::Write) {
        getInfo();
        return;
    }

    if (auto operation = dynamic_cast<FilesystemInfoOperation*>(op); operation) {
        while (auto item = m_driveInfoLayout->takeAt(0)) {
            if (auto widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }

        for(int i = 0; i < operation->drives().count(); ++i) {
            auto &driveInfo = operation->drives().at(i);
            size_t size = driveInfo.value;
            m_driveInfoLayout->addWidget(new QLabel(driveInfo.text), i, 0);
            m_driveInfoLayout->addWidget(new QLabel(QString::number(size)), i, 1);
            m_driveInfoLayout->setColumnStretch(2, 1);
        }

        changeDirTo(currentPath());
        return;
    }

    if (auto operation = dynamic_cast<FilesystemListOperation*>(op); operation) {
        m_listWidget->clear();
        auto list = operation->items();
        list.sort();

        std::map<NodeType, QStringList> allNodes;
        if (operation->dir().count('/'))
            allNodes[DotDotNode].push_back("..");

        for (auto& item: list) {
            if (item.endsWith(":/"))
                allNodes[DriveNode].push_back(item.chopped(2));
            else if (item.endsWith('/'))
                allNodes[DirNode].push_back(item.chopped(1));
            else
                allNodes[FileNode].push_back(item);
        }

        for(auto &[nodeType, nameList]: allNodes)
            for(auto &name: nameList)
                addNode(name, nodeType);

        setCurrentPath(operation->dir());
        return;
    }
}

void FilesystemDiaolog::onNewDirAction()
{
    if (currentPath().isEmpty())
        return;

    QString text = QInputDialog::getText(this, tr("Добавление каталога"),
        tr("Имя каталога:"), QLineEdit::Normal,
        "NewFolder");

    if (text.isEmpty())
        return;

    auto newDirPath = currentPath();
    if (newDirPath.back() != '/')
        newDirPath.append('/');
    newDirPath.append(text);

    emit operationRequest(std::make_shared<FilesystemNewOperation>(newDirPath));
}

void FilesystemDiaolog::onUploadAction()
{
    if (currentPath().isEmpty())
        return;

    QSettings settings;
    const QString last_path_tag = "filesystem/last_path";
    auto last_path = settings.value(last_path_tag, "/home").toString();
    auto pathList = QFileDialog::getOpenFileNames(this, "Выбрать файлы", last_path);
    if (pathList.isEmpty())
        return;

    settings.setValue(last_path_tag, QFileInfo(pathList.front()).absolutePath());

    QList<FilesystemUploadOperation::Item> writeList;
    for (auto &path: pathList) {
        FilesystemUploadOperation::Item item;
        item.localPath = path;
        item.devicePath = QString("%1/%2").arg(currentPath(), QFileInfo(path).fileName());
        writeList.append(item);
    }
    emit operationRequest(std::make_shared<FilesystemUploadOperation>(writeList));
}

void FilesystemDiaolog::onDownloadAction()
{
    QStringList pathList;
    for (auto& item : m_listWidget->selectedItems()) {
        auto nodeType = item->data(Qt::UserRole).value<NodeType>();
        if (FileNode != nodeType)
            return;

        pathList << makePath(item->text());
    }

    if (pathList.isEmpty())
        return;

    QSettings settings;
    const QString last_path_tag = "filesystem/last_path";
    auto last_path = settings.value(last_path_tag, "/home").toString();
    QString dir = QFileDialog::getExistingDirectory(this, "Выбрать папку", last_path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty())
        return;

    settings.setValue(last_path_tag, dir);

    QList<FilesystemDownloadOperation::Item> downloadList;
    for (auto &path: pathList) {
        FilesystemDownloadOperation::Item item;
        item.devicePath = path;
        item.localPath = QString("%1/%2").arg(dir, QFileInfo(path).fileName());
        downloadList.append(item);
    }
    emit operationRequest(std::make_shared<FilesystemDownloadOperation>(downloadList));
}

void FilesystemDiaolog::onRemoveAction()
{
    if (currentPath().isEmpty())
        return;

    QStringList removeList;
    for (auto item : m_listWidget->selectedItems()) {
        auto nodeType = item->data(Qt::UserRole).value<NodeType>();
        if (nodeType & (DriveNode | DotDotNode))
            continue;

        removeList << makePath(item->text());
    }

    if (removeList.isEmpty())
        return;

    if (!MsgBox::question("Вы уверены что хотите удалить выделенные объекты?"))
        return;

    emit operationRequest(std::make_shared<FilesystemRemoveOperation>(removeList));
}

void FilesystemDiaolog::onFormatAction()
{
    if (!currentPath().isEmpty())
        return;

    QStringList driveList;
    for (auto item : m_listWidget->selectedItems()) {
        auto nodeType = item->data(Qt::UserRole).value<NodeType>();
        if (nodeType != DriveNode)
            continue;

        driveList << makePath(item->text());
    }

    if (driveList.isEmpty())
        return;

    auto drive = driveList.at(0).toUInt();
    if (!MsgBox::question(QString("Вы уверены что хотите продолжить, все данные на томе %1 будут безвозвратно удалены?")
                          .arg(drive)))
        return;

    emit operationRequest(std::make_shared<FilesystemFormatOperation>(drive));
}

void FilesystemDiaolog::onItemDoubleClicked(QListWidgetItem* item)
{
    auto nodeType = item->data(Qt::UserRole).value<NodeType>();
    if (FileNode == nodeType)
        return;

    QString dir;
    QString text = item->text();
    QString curPath = currentPath();
    if (DriveNode == nodeType) {
        dir = QString("%1:/").arg(text);
    }
    else if (DotDotNode == nodeType) {
        if (curPath.back() != '/') {
            auto pos = curPath.lastIndexOf('/');
            dir = curPath.left(curPath.count('/') > 1 ? pos : pos + 1);
        }
    }
    else {
        dir = makePath(text);
    }

    changeDirTo(dir);
}

void FilesystemDiaolog::onItemSelectionChanged()
{
    NodeTypes selectedTypes = NoNode;
    for(auto item: m_listWidget->selectedItems()) {
        auto nodeType = item->data(Qt::UserRole).value<NodeType>();
        selectedTypes |= nodeType;
    }

    auto onlyDrivesSelected = selectedTypes == DriveNode;
    auto onlyFilesSelected = selectedTypes == FileNode;
    auto onlyDirFileSelected = (selectedTypes & (DirNode | FileNode))
            && !(selectedTypes & (DriveNode | DotDotNode));

    m_newAction->setEnabled(!currentPath().isEmpty());
    m_uploadAction->setEnabled(!currentPath().isEmpty());
    m_formatAction->setEnabled(onlyDrivesSelected);
    m_removeAction->setEnabled(onlyDirFileSelected);
    m_downloadAction->setEnabled(onlyFilesSelected);
}

void FilesystemDiaolog::setCurrentPath(const QString& path)
{
    m_currentPath = path;
    m_pathLineEdit->setText(path);
    onItemSelectionChanged();
}

void FilesystemDiaolog::getInfo()
{
    emit operationRequest(std::make_shared<FilesystemInfoOperation>());
}

void FilesystemDiaolog::changeDirTo(const QString& dir)
{
    emit operationRequest(std::make_shared<FilesystemListOperation>(dir));
}

void FilesystemDiaolog::addNode(const QString& text, NodeType type)
{
    auto item = new QListWidgetItem(text, m_listWidget);
    item->setData(Qt::UserRole, QVariant::fromValue<NodeType>(type));
    switch (type) {
    case DriveNode:
        item->setIcon(QIcon(":/icons/dsp.svg"));
        break;
    case DirNode:
    case DotDotNode:
        item->setIcon(QIcon(":/icons/folder.svg"));
        break;
    case FileNode:
        item->setIcon(QIcon(":/icons/file.svg"));
        break;
    default:
        break;
    }
    m_listWidget->addItem(item);
}

QString FilesystemDiaolog::makePath(const QString& text)
{
    auto path = currentPath();
    if (path.size() && path.back() != '/')
        path.append('/');
    path.append(text);

    return path;
}
