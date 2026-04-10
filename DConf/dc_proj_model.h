#pragma once

#include <QWidget>

#include "project/dc_node.h"

class QTreeView;
class QStandardItemModel;
class QStandardItem;

class DcProject;

class DcProjModel : public QWidget
{
    Q_OBJECT

public:
    DcProjModel(QWidget *parent = nullptr);
    ~DcProjModel();

    bool contains(DcProject *project) const;
    const QList<DcProject*>& projects() const;
    DcNode* selectedNode() const;

signals:
    void appended(DcNode* node);
    void activate(DcNode* node);
    void aboutToClose(DcNode* node);

public slots:
    void openProject(DcProject *project);
    void closeProject(DcProject *project);
    void closeAll();

private slots:
    void onCustomContextMenu(const QPoint &point);
    void onItemChanged(QStandardItem* view_item);
//    void onTreeSelection(const QItemSelection &, const QItemSelection &);
    void onProjectInfoChanged();
    void onSettingsAction();
    void onTreeDoubleClicked(const QModelIndex& index);

    void slotAddFolder();
    void slotAddController();
    void slotRenameItem();
    void slotRemoveItem();
    void slotCopyController();
    void slotExportController();
    void slotImportController();
    void slotPasteController();
    void slotCloseProject();

private:
    QStandardItem* makeViewItem(DcNode* node, QStandardItem* parent_view_item);
    DcNode* makeNode(const QString &name, DcNode::Type type, DcNode* parent_node);
    DcNode* getNode(QStandardItem* view_item, bool checkParent = false) const;
    void fillProjTreeItem(DcNode* node, QStandardItem* parent_view_item);
    void addFolder(QStandardItem *parent_view_item);
    void addDevice(QStandardItem *parent_view_item);
    void append(DcNode* node, QStandardItem* parent_view_item);
    void close(QStandardItem *view_item, bool remove);

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

    QList<DcProject*> m_openedProjects;
    DcNode* m_nodeToCopy;   // контроллера для копирования
};
