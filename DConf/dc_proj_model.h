#pragma once

#include <map>
#include <list>

#include <QObject>
#include <QTreeView>
#include <QProcess>
#include <QStandardItemModel>

class DcNode;
class DcProject;
class DcMenu;

class DcController;

class DcProjModel : public QWidget
{
    Q_OBJECT

public:
    DcProjModel(QWidget *parent = nullptr);
    ~DcProjModel();

    bool contains(DcProject *project) const;
    DcNode* selectedNode() const;

signals:
    void activate(DcNode* node);
    void aboutToClose(DcNode* node);

public slots:
    void openProject(DcProject *project);
    void closeProject(DcProject *project);
    void closeAll();

private slots:
    void onItemChanged(QStandardItem* view_item);
    void onCustomContextMenu(const QPoint &point);
//    void onTreeSelection(const QItemSelection &, const QItemSelection &);
    void onProjectNameChanged(const QString &name);

    void slotAddFolder();
    void slotAddController();
    void slotRenameItem();
    void slotRemoveItem();
    void slotCopyController();
    void slotExportController();
    void slotImportController();
    void slotPasteController();
    void slotOpenDView();
    void slotCloseProject();
    void onSettingsAction();

private slots:
    void onTreeDoubleClicked(const QModelIndex& index);

private:
    QStandardItem* makeViewItem(DcNode* node, QStandardItem* parent_view_item);
    DcNode* getNode(QStandardItem* view_item, bool checkParent = false) const;
    void fillProjTreeItem(DcNode* node, QStandardItem* parent_view_item);
    void addFolder(QStandardItem *parent_view_item);
    void addDevice(QStandardItem *parent_view_item);
    void close(QStandardItem *view_item, bool remove);
    DcNode* createDeviceNode(const QString &nameTemplate, DcNode *parent_node);

private:
    QTreeView* m_tree;
    QStandardItemModel* m_model;

    QAction* m_settingsAction;
    QAction* m_renameItem;
    QAction* m_removeItem;
    QAction* m_addFolder;
    QAction* m_addController;
    QAction* m_copyController;
    QAction* m_exportController;
    QAction* m_importController;
    QAction* m_pasteController;
    QAction* m_closeAction;
    QAction* m_openDView;

    QProcess* m_processDView;

    QList<DcProject*> m_openedProjects;
    DcNode* m_nodeToCopy;   // контроллера для копирования
};
