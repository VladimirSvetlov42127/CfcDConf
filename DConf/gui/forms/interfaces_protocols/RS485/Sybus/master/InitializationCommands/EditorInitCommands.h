#pragma once

#include <QDialog>
#include <qvector.h>
#include <qboxlayout.h>
#include <qcombobox.h>

#include "data_model/dc_controller.h"
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/InterfaseInitCommand.h>

class EditorInitCommands : public QDialog
{
	Q_OBJECT
		
	enum {
		COMMAND_TYPE_POS = 1
	};

public:
	EditorInitCommands(DcController *controller, uint16_t startSlaveInitIndex, QWidget *parent = Q_NULLPTR);
	virtual ~EditorInitCommands();

private:
	DcController *m_controller;
	uint16_t m_startInitFieldIndex;	// Начальный индекс в модели параметров, с которого начинается описание команд инициализации
	uint16_t m_maxCommandByteSize; // Максимальный размер под все команды, в байтах

	QVector<QByteArray> mv_initCommandArrays;	// Здесь хранятся команды уже как массив байт. Элементы вектора передаются в виджеты команд по указателю
	QVector<InterfaseInitCommand*> mv_initCommands;
	QWidget *m_currentCommandWidget;	// Указатель на текущий виджет команды, удаляется вместе с формой

	QComboBox *m_comBoxCreateType;
	QComboBox *m_comBoxCommands;
	QVBoxLayout *m_commandLayout;	// Слой, куда вставляются виджеты команд (и заменяются при смене)

private:
	bool parseModel();	// Читаем из модели команды, добавляем в вектор
	void updateCommandNamesInCombobox();	// При удалении команды переименовываем остальные
	void replaceWidgetInGroupBox(QWidget *wid);

private slots:
	void slot_comBoxCommand_itemChanged(int);
	void slot_butCreate_clicked();
	void slot_butErase_cliked();
	void slot_butSave_clicked();
	void slot_butCancel_clicked();
};
