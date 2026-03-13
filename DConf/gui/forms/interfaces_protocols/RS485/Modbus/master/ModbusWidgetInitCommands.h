#pragma once

#include <QWidget>
#include "ui_ModbusWidgetInitCommands.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandInitialization.h>

class ModbusWidgetInitCommands : public QWidget
{
	Q_OBJECT
		typedef QSharedPointer<ModbusCommandInitialization> spInitCommand;

public:
	ModbusWidgetInitCommands(QVector<spInitCommand>* vector, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetInitCommands();

	bool setCommandVector(QVector<spInitCommand>* vector);

private:
	Ui::ModbusWidgetInitCommands ui;
	spInitCommand m_curInitCommand;
	QVector<spInitCommand> *mv_commands;

private:
	void blockFields(bool block);

public slots:
	void slot_comBoxInitCommand_itemChanged(int);

	void slot_butAdd_clicked();
	void slot_butErase_clicked();

	void slot_spBox_valueChanged(int);
	void slot_lEdExpectedFirstRspByte_editingFinished();
	void slot_lEdInitCmdBuf_editingFinished();
};
