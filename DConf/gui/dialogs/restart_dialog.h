#ifndef RESTARTDIALOG_H
#define RESTARTDIALOG_H

#include <QDialog>

#include "device_operations/restart_operation.h"

class QCheckBox;
class QComboBox;

class RestartDialog : public QDialog
{
public:
    RestartDialog(bool hasT2compatibilityMode, QWidget* parent = nullptr);

    RestartOperation::RestartMode mode() const;
    RestartOperation::WorkMode workMode() const;
    RestartOperation::DecontMode decontMode() const;

private slots:
    QComboBox* m_restartModeComboBox;
    QComboBox* m_workModeComboBox;
    QComboBox* m_T2ModeComboBox;
    QCheckBox* m_workModeCheckBox;
    QCheckBox* m_T2ModeCheckBox;
};

#endif // RESTARTDIALOG_H
