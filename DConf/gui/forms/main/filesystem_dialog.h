#ifndef FILESYSTEMDIALOG_H
#define FILESYSTEMDIALOG_H

#include <QDialog>

#include "device_operations/abstract_operation.h"

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QProgressBar;
class QGridLayout;

class FilesystemDiaolog : public QDialog
{
    Q_OBJECT

public:
    enum NodeType {
        NoNode = 0x0,
        DriveNode = 0x1,
        DotDotNode = 0x2,
        DirNode = 0x4,
        FileNode = 0x8
    };
    Q_ENUM(NodeType);
    Q_DECLARE_FLAGS(NodeTypes, NodeType);

    FilesystemDiaolog(QWidget *parent = nullptr);
    ~FilesystemDiaolog();

    void start();

signals:
    void operationRequest(AbstractOperation::SPtr);

public slots:
    void operationFinished(AbstractOperation* op);

private slots:
    void onNewDirAction();
    void onUploadAction();
    void onDownloadAction();
    void onRemoveAction();
    void onFormatAction();

    void onItemDoubleClicked(QListWidgetItem* item);
    void onItemSelectionChanged();

private:
    void setCurrentPath(const QString& path);
    void getInfo();
    void changeDirTo(const QString&dir);
    void addNode(const QString& text, NodeType type);
    QString makePath(const QString& text);

    QString currentPath() const { return m_currentPath; }

private:
    QString m_currentPath;
    QGridLayout* m_driveInfoLayout;
    QLineEdit* m_pathLineEdit;
    QListWidget* m_listWidget;

    QAction* m_newAction;
    QAction* m_uploadAction;
    QAction* m_downloadAction;
    QAction* m_removeAction;
    QAction* m_formatAction;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(FilesystemDiaolog::NodeTypes)

#endif // FILESYSTEMDIALOG_H
