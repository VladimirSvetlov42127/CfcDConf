#pragma once

#include <QDialog>
#include <qlist.h>
#include "ui_CmdLibWidget.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/DeviceCommandLibrary/CmdLibTreeItem.h>

class CmdLibWidget : public QDialog
{
	Q_OBJECT

	typedef QSharedPointer<ModbusCommandInitialization> sp_InitCommand;

public:
	CmdLibWidget(QWidget *parent = Q_NULLPTR);
	~CmdLibWidget();

private:
	Ui::CmdLibWidget ui;
	QList<spCommandDescr_t> ml_periodicCommands;
	QList<sp_InitCommand> ml_initCommands;

private:
	void addAllChildrenCommand(CmdLibTreeItem *parent);

signals:
	void signal_addCommandsFromLibrary(QList<spCommandDescr_t>);
	void signal_addInitCommandsFromLibrary(QList<sp_InitCommand>);

public slots:
	void on_butOk_clicked();
	void on_butCancel_clicked();
};
