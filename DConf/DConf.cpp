#include "DConf.h"

#include <QDebug>
#include <QFileDialog>
#include <QDirIterator>
#include <QSettings>
#include <QKeyEvent>
#include <QDesktopServices>
#include <QLabel>
#include <QStackedWidget>
#include <QThread>
#include <QAction>
#include <QTimer>
#include <QScreen>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <dpc/dep_about_box.h>

#include "version.h"
#include "utils/qzipreader_p.h"
#include "utils/qzipwriter_p.h"
#include "db/dc_db_manager.h"
#include "gui/dialogs/frmCreateProj.h"
#include "gui/dialogs/frmOpenProj.h"
#include "updater/updater.h"
#include "gui/dc_device_window.h"
#include "dc_proj_model.h"
#include "device_templates/dc_templates_manager.h"
#include "project/dc_config_loader.h"
#include "project/dc_device_node.h"

using namespace Dpc::Gui;

namespace {

#ifdef QT_DEBUG
constexpr const char* HELP_FILE = "../dconf_res/help/result/dconf.chm";
#else
constexpr const char* HELP_FILE = "dconf.chm";
#endif    

#define VER_PRE "pre.3"
#ifdef VER_PRE
constexpr const char* VERSION = DCONF_VERSION_STRING "-" VER_PRE;
#else
constexpr const char* VERSION = DCONF_VERSION_STRING;
#endif

constexpr const char* APPLICATION_NAME = "DConf";
constexpr const char* APPLICATION_COMPANY = "DEP";

constexpr const char* DEFAULT_IMPORT_DIR = "/home";
constexpr const char* SETTING_IMPORT_DIR = "importDir";
constexpr const char* SETTING_GEOMETRY = "importDir";

constexpr const char* disclaimer = "		ВНИМАНИЕ!!!\n"
		"	В связи с изменением структуры встроенных алгоритмов в устройствах (DEPROTEC, DEPRTU-LT-XX)"
		", конфигурации алгоритмов сделанные на Dconf версии 6.5.1 и ниже, не совместимы с текущей версией."
		" Для восстановления конфигурации встроенных алгоритмов, которые уже загружены в устройства,"
		" нужно обновить прошивки этих устройств и вычитать конфигурацию."
		" Для устройств DEPROTEC до версией ПО не ниже 1.70, для DEPRTU-LT-XX до версией ПО не ниже 2.57.\n\n"
		"	При открытии данного проекта конфигурация проекта будет безвозвратно изменена, рекомендуем сделать копию папки проекта(%1). Продолжить?";
}

DConf::DConf(QWidget *parent)
    : QMainWindow(parent)
    , m_projectsWidget{ new DcProjModel(this) }
    , m_updater{ new DcUpdater(this) }
    , m_statusBarTimer{new QTimer(this)}
    , m_configLoader{ new ConfigLoader(this) }
    , m_updaterSilent{ true }
    , m_lastOpenedWindow{ nullptr }
{
    ui.setupUi(this);

    setWindowIcon(QIcon(":/icons/dconf.svg"));
	QApplication::setWindowIcon(windowIcon());

    auto palette = qApp->palette();
    auto color = palette.color(QPalette::Highlight);
    color.setAlpha(80);
    palette.setColor(QPalette::Highlight, color);
    qApp->setPalette(palette);

    auto layout = new QVBoxLayout(centralWidget());
    layout->addWidget(m_projectsWidget);
    connect(m_projectsWidget, &DcProjModel::activate, this, &DConf::onNodeActivate);
    connect(m_projectsWidget, &DcProjModel::aboutToClose, this, &DConf::onNodeAboutToClose);

    m_statusBarTimer->setInterval(5000);
    m_statusBarTimer->setSingleShot(true);
    connect(m_statusBarTimer, &QTimer::timeout, this, &DConf::cleanStatusBar);

	ui.mainToolBar->setIconSize(QSize(32, 32));    
    auto createProjectAction = ui.mainToolBar->addAction(QIcon(":/icons/proj_add.svg"), "Cоздать проект", this, &DConf::onCreateProjectAction);
    auto openProjectAction = ui.mainToolBar->addAction(QIcon(":/icons/proj.svg"), "Открыть проект", this, &DConf::onOpenProjectAction);
    auto importProjectAction = ui.mainToolBar->addAction(QIcon(":/icons/bd_out.svg"), "Импорт", this, &DConf::onImportProjectAction);
    auto exportProjectAction = ui.mainToolBar->addAction(QIcon(":/icons/bd_in.svg"), "Экспорт", this, &DConf::onExportProjectAction);

    auto exitAction = new QAction("Выход", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &DConf::onAppExitAction);

    auto helpAction = new QAction("Помощь", this);
    helpAction->setShortcut(QKeySequence::HelpContents);
    connect(helpAction, &QAction::triggered, this, &DConf::onHelpAction);

    auto aboutAction = new QAction("О программе", this);
    connect(aboutAction, &QAction::triggered, this, &DConf::onAboutAction);

    auto updateAction = new QAction("Обновления", this);
    connect(updateAction, &QAction::triggered, this, &DConf::onCheckUpdatesAction);

    ui.menuProject->addAction(createProjectAction);
    ui.menuProject->addAction(openProjectAction);
    ui.menuProject->addAction(importProjectAction);
    ui.menuProject->addAction(exportProjectAction);
    ui.menuProject->addSeparator();
    ui.menuProject->addAction(exitAction);

    ui.menuHelp->addAction(helpAction);
    ui.menuHelp->addAction(aboutAction);
    ui.menuHelp->addAction(updateAction);

    connect(m_updater, &DcUpdater::error, this, &DConf::onUpdaterError);
    connect(m_updater, &DcUpdater::updatesAvailableState, this, &DConf::onUpdaterState);

    connect(m_configLoader, &ConfigLoader::error, this, &DConf::onConfigLoaderError);

    connect(&m_projectsManager, &DcProjectsManager::error, this, &DConf::showError);
    connect(&m_projectsManager, &DcProjectsManager::aboutToRemove, m_projectsWidget, &DcProjModel::closeProject);

    connect(&gTemplatesManager, &DcTemplatesManager::error, this, &DConf::showError);
    connect(&gDbManager, &DcDbManager::error, this, &DConf::onDbError);

    QCoreApplication::setOrganizationName(APPLICATION_COMPANY);
    QCoreApplication::setApplicationName(APPLICATION_NAME);
    loadSettings();

    gTemplatesManager.load();
    m_projectsManager.load();

    QMetaObject::invokeMethod(m_updater, &DcUpdater::checkUpdates, Qt::QueuedConnection);
}

DConf::~DConf()
{
    m_projectsWidget->closeAll();
}

void DConf::onCreateProjectAction()
{
    auto newProject = m_projectsManager.make();
    if (!newProject)
        return;

    if (!editProject(newProject)) {
        m_projectsManager.remove(newProject);
        return;
    }

    initProject(newProject);
}

void DConf::onOpenProjectAction()
{
    frmOpenProj dlg(&m_projectsManager, this);
    if (QDialog::Accepted != dlg.exec())
        return;

    auto project = dlg.project();
    if(m_projectsWidget->contains(project)) {
        MsgBox::info("Выбраный проект уже открыт");
        return;
    }

    initProject(dlg.project());
}

void DConf::onImportProjectAction()
{
    QString filename = QFileDialog::getOpenFileName(this, "Импорт проекта",	m_importDir, "Проекты (*.dproj);;");
    if (filename.isNull()) {
        return;
    }
    m_importDir = QDir(filename).absolutePath();

    auto newProject = m_projectsManager.make();
    if (!newProject)
        return;

    QString proj_path = newProject->path();
    QZipReader zip_reader(filename);
    if (zip_reader.exists()) {
        // распаковка архива по указанному пути
        if (!zip_reader.extractAll(proj_path)) {
            QDir projDir(proj_path);
            if (projDir.removeRecursively()) {
                MsgBox::error("Невозможно распаковать архив проекта! \nВозможно, не все временные файлы были удалены. Проверьте путь удаления: \n" + proj_path);
            }
            else
                MsgBox::error("Невозможно распаковать архив проекта!");
            return;
        }
    }

    if (!newProject->load()) {
        m_projectsManager.remove(newProject);
        return;
    }

    initProject(newProject);
}

void DConf::onExportProjectAction()
{
    auto node = m_projectsWidget->selectedNode();
    while(node && node->parent())
        node = node->parent();

    if (!node || node->type() != DcNode::ProjectType)
        return;

    auto project = static_cast<DcProject*>(node);
    QString filename = QFileDialog::getSaveFileName(this, "Экспорт проекта", m_importDir, "Проекты (*.dproj)");
    if (filename.isNull())
        return;

    m_importDir = QFileInfo(filename).dir().absolutePath();
    auto projPath = project->path();

    QZipWriter zip(filename);
    if (zip.status() != QZipWriter::NoError)
        return;

    zip.setCompressionPolicy(QZipWriter::AutoCompress);
    QDirIterator it(projPath, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString file_path = it.next();
//        qDebug() << file_path;
        if (it.fileInfo().isDir()) {
            zip.setCreationPermissions(QFile::permissions(file_path));
            zip.addDirectory(file_path.remove(projPath));
        }
        else if (it.fileInfo().isFile()) {
            QFile file(file_path);
            if (!file.open(QIODevice::ReadOnly))
                continue;

            zip.setCreationPermissions(QFile::permissions(file_path));
            QByteArray ba = file.readAll();
            zip.addFile(file_path.remove(projPath), ba); }
    }
    zip.close();
}

void DConf::onAppExitAction()
{
    qApp->exit();
}

void DConf::onAboutAction()
{
    DepSettings settings;
    settings.insert(about_icon, QString(":/icons/dconf.svg"));
    settings.insert(about_name, QString("DConf (64bit)"));
    QString message = QString("Инструментально программное обеспечение DConf для ");
    message += QString("конфигурирования многофункциональных устройств depRTU, ") + '\n';
    message += QString("устройств релейной защиты и автоматики DeProtec, ") + '\n';
    message += QString("комплектов релейных защит и автоматики ExRZA.");
    settings.insert(about_version, VERSION);
    settings.insert(about_message, message);
    DepAboutBox about_box(settings);
    about_box.exec();
}

void DConf::onHelpAction()
{
    QString context;
    openHelp(context);
}

void DConf::onCheckUpdatesAction()
{
    checkForUpdates(false);
}

void DConf::closeEvent(QCloseEvent * event)
{
    saveSettings();
    qApp->closeAllWindows();
    QWidget::closeEvent(event);
}

void DConf::keyPressEvent(QKeyEvent* event)
{
    QWidget::keyPressEvent(event);
}

void DConf::onUpdaterError(const QString& errorMsg)
{
	auto msg = QString("Ошибка сервиса обновлений: %1").arg(errorMsg);
    setStatusBarMsg(msg, QIcon(":/icons/error.svg"));
	if (!m_updaterSilent)
		MsgBox::error(msg);
}

void DConf::onUpdaterState(bool hasUpdates)
{
	if (!hasUpdates) {
		auto msg = QString("Нет обновлений");
        setStatusBarMsg(msg, QIcon(":/icons/ok.svg"));
		if (!m_updaterSilent)
			MsgBox::info(msg);
		return;
	}

    setStatusBarMsg("Доступны обновления", QIcon(":/icons/info.svg"));
    if (!MsgBox::question("Доступно обновление приложения или его компонентов, "
                          "запустить мастер установки обновлений? Данное приложиение будет закрыто"))
		return;

	if (!m_updater->startUpdates())
		return;

	QThread::msleep(100);
    qApp->exit(0);
}

void DConf::onNodeActivate(DcNode *node)
{
    if (!node)
        return;

//    qDebug() << node->fullText();
    openNodeWindow(node);
}

void DConf::onNodeAboutToClose(DcNode *node)
{
    if (!node)
        return;

//    qDebug() << node->fullText();
    removeNodeWindow(node);
}

void DConf::onDbError(const QString &msg, const QString &filePath, DcController *device)
{
    showError(QString("%1: %2").arg(msg, filePath));
}

void DConf::onConfigLoaderError(const QString &errorMsg)
{
    showError(errorMsg);
}

void DConf::showError(const QString &errorMsg)
{
    Dpc::Gui::MsgBox::error(errorMsg);
}

bool DConf::editProject(DcProject *project)
{
    frmCreateProj dlg(project, this);
    return QDialog::Accepted == dlg.exec();
}

void DConf::initProject(DcProject *project)
{
    if (!project)
        return;

    // Отладка
//    std::function<void(DcNode*, int)> print = [&](DcNode* node, int indent) -> void {
//        qDebug().noquote() << QString("%1%2").arg(QString('\t', indent), node->name());
//        for(size_t i = 0; i < node->childsSize(); ++i)
//            print(node->child(i), indent + 1);
//    };
//    print(project, 0);

    // Актуализация версии проекта,
    auto version = project->version();
    if (!version) {
        if (!MsgBox::question(QString(disclaimer).arg(project->path())))
            return;

         project->setVersion(++version);
         project->save();
    }

    // Загрузка конфигураций устройств с обновлением из шаблона.
    std::function<void(DcNode*)> loadConfig = [&](DcNode* node) -> void {
        if (node->type() == DcNode::DeviceType) {
            auto deviceNode = static_cast<DcDeviceNode*>(node);
            deviceNode->setConfig(m_configLoader->load(deviceNode->configFilePath(), deviceNode->name(), true));
        }

        for(size_t i = 0; i < node->childsSize(); ++i)
            loadConfig(node->child(i));
    };

    QApplication::setOverrideCursor(Qt::WaitCursor);
    loadConfig(project);
    QApplication::restoreOverrideCursor();
    m_configLoader->freeCache();

    // Открытие проекта в виджете проектов
    m_projectsWidget->openProject(project);
}

void DConf::checkForUpdates(bool silent)
{
    m_updaterSilent = silent;
    m_updater->checkUpdates();
}

void DConf::openHelp([[maybe_unused]] const QString &context)
{
    auto helpFilePath = QFileInfo(HELP_FILE).absoluteFilePath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(helpFilePath));
}

void DConf::saveSettings() const
{
    QSettings settings;
    settings.setValue(SETTING_IMPORT_DIR, m_importDir);
    settings.setValue(SETTING_GEOMETRY, geometry());
}

void DConf::loadSettings()
{
    QSettings settings;
    m_importDir = settings.value(SETTING_IMPORT_DIR, DEFAULT_IMPORT_DIR).toString();

    auto screenGeometry = QApplication::primaryScreen()->availableGeometry();
    QSizeF windowSize = { screenGeometry.width() * 0.2, screenGeometry.height() * 0.8 };
    int x = screenGeometry.x() + (screenGeometry.width() - windowSize.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - windowSize.height()) / 2;
    auto defaultGeometry = QRect(QPoint(x, y), windowSize.toSize());
    auto geometry = settings.value(SETTING_GEOMETRY, defaultGeometry);
    setGeometry(geometry.value<QRect>());
}

void DConf::setStatusBarMsg(const QString& text, const QIcon& icon)
{
    cleanStatusBar();

    auto iconLabel = new QLabel(ui.statusBar);
    iconLabel->setPixmap(icon.pixmap(16, 16));
    ui.statusBar->addWidget(iconLabel);
    ui.statusBar->addWidget(new QLabel(text, ui.statusBar));
    m_statusBarTimer->start();
}

void DConf::cleanStatusBar()
{
    for (auto ch : ui.statusBar->findChildren<QLabel*>()) {
        ui.statusBar->removeWidget(ch);
        ch->deleteLater();
    }

    m_statusBarTimer->stop();
}

void DConf::openNodeWindow(DcNode *node)
{
    if (node->type() == DcNode::ProjectType) {
        editProject(static_cast<DcProject*>(node));
        return;
    }

    if (node->type() == DcNode::DeviceType) {
        auto device_node = static_cast<DcDeviceNode*>(node);
        DcDeviceWindow* deviceWindow = nullptr;
        if (auto deviceWindowIt = m_deviceWindows.find(node); deviceWindowIt != m_deviceWindows.end()) {
            deviceWindow = deviceWindowIt->second;
        }
        else {
            // Загрузка конфигурации при необходимости
            if (!device_node->config()) {
                auto config = m_configLoader->load(device_node->configFilePath(), device_node->name(), true);
                if (!config)
                    return;

                device_node->setConfig(std::move(config));
            }

            // создание окна конфигурации
            deviceWindow = m_deviceWindows.emplace(node, new DcDeviceWindow(device_node)).first->second;

            // установка размера окна
            auto screenGeometry = QApplication::primaryScreen()->availableGeometry();
            auto windowSize = QSize( screenGeometry.width() * 0.8, screenGeometry.height() * 0.8);
            deviceWindow->resize(windowSize);

            // установка позиции окна
            int x = m_lastOpenedWindow ? m_lastOpenedWindow->x() + 40 : (screenGeometry.width() - windowSize.width() ) / 2;
            int y = m_lastOpenedWindow ? m_lastOpenedWindow->y() + 40 : (screenGeometry.height() - windowSize.height() ) / 2;
            deviceWindow->move(x, y);
        }

        deviceWindow->showNormal();
        deviceWindow->activateWindow();
        m_lastOpenedWindow = deviceWindow;
    }
}

void DConf::removeNodeWindow(DcNode *node)
{
    if (node->type() == DcNode::ProjectType) {
        return;
    }

    if (node->type() == DcNode::DeviceType) {
        auto deviceWindowIt = m_deviceWindows.find(node);
        if (deviceWindowIt == m_deviceWindows.end())
            return;

        auto deviceWindow = deviceWindowIt->second;
        auto deviceNode = deviceWindow->node();

        // Закрытие и удаление окна конфигурации
        if (deviceWindow == m_lastOpenedWindow)
            m_lastOpenedWindow = nullptr;
        deviceWindow->close();
        deviceWindow->deleteLater();
        m_deviceWindows.erase(deviceWindowIt);

        // Удаление конфигурации
        deviceNode->setConfig(DcController::UPtr());
    }
}
