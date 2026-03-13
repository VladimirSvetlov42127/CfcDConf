#pragma once

#include "ui_DConf.h"
#include <QtWidgets/QMainWindow>

#include "project/dc_projects_manager.h"

class QTimer;

class DcUpdater;
class DcProjModel;
class DcController;
class DcDeviceWindow;
class ConfigLoader;

class DConf : public QMainWindow
{
	Q_OBJECT

public:
    DConf(QWidget *parent = nullptr);
    ~DConf();

public slots:
    void onCreateProjectAction();
    void onOpenProjectAction();
    void onImportProjectAction();
    void onExportProjectAction();
    void onAppExitAction();
    void onAboutAction();
    void onHelpAction();
    void onCheckUpdatesAction();

protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onUpdaterError(const QString& errorMsg);
    void onUpdaterState(bool hasUpdates);
    void onNodeActivate(DcNode* node);
    void onNodeAboutToClose(DcNode* node);
    void onDbError(const QString &msg, const QString &filePath, DcController *device);
    void onConfigLoaderError(const QString &errorMsg);
    void showError(const QString& errorMsg);

private:
    bool editProject(DcProject* project);
    void initProject(DcProject* project);
    void checkForUpdates(bool silent = true);
    void openHelp(const QString& context = QString());
	void saveSettings() const;
	void loadSettings();
    void setStatusBarMsg(const QString& text, const QIcon& icon);
    void cleanStatusBar();
    void openNodeWindow(DcNode* node);
    void removeNodeWindow(DcNode* node);

private:
    Ui::dconfClass ui;

    DcProjModel *m_projectsWidget;
    DcUpdater* m_updater;
    QTimer* m_statusBarTimer;
    DcProjectsManager m_projectsManager;
    ConfigLoader* m_configLoader;
    bool m_updaterSilent;
    QString m_importDir;
    std::unordered_map<DcNode*, DcDeviceWindow*> m_deviceWindows;
    DcDeviceWindow* m_lastOpenedWindow;
};
