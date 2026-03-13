#pragma once

#include <QWidget>
#include "ui_ModbusWidgetSlaveSettings.h"
#include <qvector.h>
#include <qsharedpointer.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveDefinition.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusSlaveFileSaver.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusWidgetChannelsTable.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/CommandWidgets/ModbusWidgetCommonCommand.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusFunctionWidgets/IModbusWidgetFunction.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/ModbusWidgetInitCommands.h>
#include <gui/forms/interfaces_protocols/RS485/PortInfo.h>

class ModbusWidgetSlaveSettings : public QWidget
{
	Q_OBJECT

	enum {
		CREATION_TAB_INDEX,
		EDITING_TAB_INDEX,
		INITIALIZATION_TAB_INDEX
	};
	typedef ModbusCommand_ns::keyPairTypeAndIndex_t key_t;
	typedef ModbusMaster_ns::channelType_t channelType_t;
	typedef QSharedPointer<ModbusCommandInitialization> sp_InitCommand;
		
public:
    ModbusWidgetSlaveSettings(uint16_t portNumber, DcController* controller, uint8_t slaveCount, bool isModifyModbus = false, QWidget *parent = Q_NULLPTR);
    ModbusWidgetSlaveSettings(uint16_t portNumber, DcController* controller, QVector<spSlave_t> &slavesVector, bool isModifyModbus = false, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetSlaveSettings();

private:
	Ui::ModbusWidgetSlaveSettings ui;
    bool m_isModifyModbus;
	DcController *m_controller;
	uint16_t m_portNumber;

	QVector<spSlave_t> mv_slaves;	// Общее число настраиваемых слейвов в векторе
	spSlave_t m_currentSlave;	// Текущий настраиваемый слейв

	ModbusSlaveFileSaver m_slaveSaver;	// Модуль чтения/записи из файла

	ModbusWidgetChannelsTable *m_tableWidget;	// Дополнительный виджет для таблиц
	QSpacerItem *m_spaser;	// Спейсер для вставки/удаления

	spCommandDescr_t m_currentCommand;	// Текущая выбранная команда
	ModbusWidgetCommonCommand *m_currentCommandWidget;	// Специально чтобы легче было извлекать с формы

	spModbusFunc_t m_currentFunction;
	IModbusWidgetFunction *m_currentFunctionWidget;	// Базовый класс виджетов для функций
	ModbusWidgetInitCommands *m_initCommandWidget;	// Виждет для вкладки управления командами инициализации

	bool m_initFunction = true;	// Используется при первом отображении команды, чтобы загрузилась сохраненная в ней функция, а не новая

	PortInfo m_PortInfo;

	QWidget *m_connectionDiscret;

private:
	// Подготовка к заполнению формы
	void commonSetup();		// Вызывается из конструкторов
	void clearAllForms();	// Очистить все
	void clearCommandWidget();	// Очистить виджет команды
	bool fillFormFromCurrentSlave();	// Заполняет форму данными из слейва

	void createExistingCommand(spCommandDescr_t command, key_t key);
	
	// Работа с виджетом таблиц для создания команд по считыванию каналов
	void addChannelToTable(ModbusMaster_ns::channelType_t type);
	void removeAllChannels(ModbusMaster_ns::channelType_t type);
	void setCheckBoxTypeState(ModbusMaster_ns::channelType_t type, bool state);

	// Работа с командой
	bool fillCommand(spCommandDescr_t command, const ModbusMaster_ns::structCommand_t & info);
	int getIndexForNewCommand(key_t key);	// Возвращает индекс для вставки в комбобокс
	void createNewCommandWidget(spCommandDescr_t command);
	bool isNewTypeCountValid(const ModbusMaster_ns::structCommand_t &);

	// Создание 
	bool createFunctionForCurrentCommand(ModbusFunctions_ns::modbusFunctionType_t type);
	spModbusFunc_t createFunction(ModbusFunctions_ns::modbusFunctionType_t type, ModbusMaster_ns::startAddress_t startChannel, ModbusMaster_ns::count_t count);
	IModbusWidgetFunction * createNewFunctionWidget(spModbusFunc_t function);

	// Работа с виджетами
	void blockTypeCheckBoxes(bool needBlock);

	bool addToCommandCombobox(key_t key, const QString &name);
	bool removeFromCommandCombobox(key_t key);
	bool removeFromCommandCombobox(channelType_t type);
	
	void closeEvent(QCloseEvent *event);	// Ставим проверку на нажатие кнопки закрытия окна


signals:
	// При активации чекбоса создаются или удаляются ВСЕ команды указанного типа
	void signal_addChannelToTable(ModbusMaster_ns::channelType_t);
	void signal_removeAllChannelsFromTable(ModbusMaster_ns::channelType_t);

	// При заполнении формы от слейва, если есть команды, посылаем сигналы в таблицу для создания
	void signal_commandExist(const ModbusMaster_ns::structCommand_t &);
	void signal_removeCommand(const ModbusMaster_ns::structCommand_t &);

public slots:
	// Общее для всех вкладок
	void on_comBoxSlaveID_currentIndexChanged(int);	// Смена слейва
	void on_lEditFilename_textChanged(const QString &text);	// Редактирование имени слейва
	void on_butSave_clicked();
	void on_butCreateFromLibrary_clicked();
	void on_comBoxCRC_currentIndexChanged(int);
	void on_butLoadSlaveFromFile_clicked();	// Загрузить настройки слейва из файла
	void on_butSaveToFile_clicked();	// Сохранить настройки в файле

	// Взаимодействие с первой вкладкой
	void on_lEdTimeout_editingFinished();
	void on_lEdAddress_editingFinished();

	void slot_chBoxTypeChanged(int);	// Общий слот для чекбоксов, определяющих типы
	

	// Взаимодействие со второй вкладкой
	void on_comBoxCommand_currentIndexChanged(int);	// Выбираем разные команды
	void on_comBoxModbusFunction_currentIndexChanged(int);	// Выбираем разные функции для команды, доступные в команде
	

	// Принимающие слоты от таблицы формирования команд
	void slot_updateCommand(const ModbusMaster_ns::structCommand_t &);
	void slot_eraseCommand(const ModbusMaster_ns::structCommand_t &);

	// Взаимодействие с библиотекой команд
	void slot_addCommandsFromLibrary(QList<spCommandDescr_t>);
	void slot_addInitCommandsFromLibrary(QList<sp_InitCommand>);

};
