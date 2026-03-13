#ifndef DCDEVICEWINDOW_H
#define DCDEVICEWINDOW_H

#include <QMainWindow>

#include "device_operations/abstract_operation.h"

class QStackedWidget;
class QLabel;
class QProgressBar;

class DcDeviceNode;
class DcController;
class DcDeviceView;
class DcForm;
class MainForm;
class DcOperationHandler;
class FilesystemDiaolog;

namespace Dpc {
class Journal;
}

class DcDeviceWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit DcDeviceWindow(DcDeviceNode* device_node, QWidget *parent = nullptr);
    virtual ~DcDeviceWindow();

    DcDeviceNode* node() const;

signals:

private slots:
    void onNodeTextChanged();

    void onQuitAction();
    void onSaveAction();
    void onDefaultAction();
    void onUndoAction();
    void onRedoAction();
    void onConnectAction();
    void onDisconnectAction();
    void onReloadInfoAction();
    void onReadConfigAction();
    void onWriteConfigAction();
    void onRestartAction();
    void onResetAction();
    void onPassportAction();
    void onFilesystemAction();
    void onFirmwareUpdateAction();
    void onViewItemSelected(const QString &itemIdx, bool isConfigItem);

    void execOperation(AbstractOperation::SPtr operation);
    void onOHProgress(int value);
    void onOHFinished(AbstractOperation* operation);

    void addJournalInfo(const QString &msg);
    void addJournalDebug(const QString &msg, int level = 0);
    void addJournalWarning(const QString &msg);
    void addJournalError(const QString &msg);

private:
    DcController* config() const;

    void createActions();
    void setDeviceActionsEnabled(bool enabled);    
    void operationFinished(AbstractOperation* operation);
    void setCurrentForm(DcForm *form);
    void setProgressIndicator(bool busyIndicator);

private:
    DcDeviceNode* m_deviceNode;
    DcDeviceView* m_view;
    QStackedWidget* m_formsStackedWidget;
    MainForm* m_mainForm;
    DcOperationHandler* m_operationHeandler;
    Dpc::Journal* m_journal;
    QLabel* m_formTitleLabel;
    QProgressBar* m_progressBar;
    FilesystemDiaolog* m_filesystemDialog;

    QAction* m_quitAction;
    QAction* m_saveAction;
    QAction* m_defaultAction;
    QAction* m_undoAction;
    QAction* m_redoAction;
    QAction* m_connectAction;
    QAction* m_disconnectAction;
    QAction* m_reloadInfoAction;
    QAction* m_readConifAction;
    QAction* m_writeConfigAction;
    QAction* m_restartAction;
    QAction* m_resetAction;
    QAction* m_passportAction;
    QAction* m_filesystemAction;
    QAction* m_firmwareUpdateAction;

    std::unique_ptr<DcForm> m_currentConfigWidget;
};

#endif // DCDEVICEWINDOW_H
