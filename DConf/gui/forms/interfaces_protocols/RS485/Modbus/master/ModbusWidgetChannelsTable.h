#pragma once

#include <QWidget>
#include "ui_ModbusWidgetChannelsTable.h"
#include <qstandarditemmodel.h>
#include <qtableview.h>
#include <qvector.h>
#include <qpair.h>
#include <qtoolbox.h>
#include <qsharedpointer.h>

#include "Common/common_ModbusCommandDefines.h"


class ModbusWidgetChannelsTable : public QWidget
{
	Q_OBJECT		

public:
	ModbusWidgetChannelsTable(bool isModifyModbus, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetChannelsTable();

	void clearAllForms();

private:

	typedef QSharedPointer<QStandardItemModel> spModel_t;
	typedef QPair<ModbusMaster_ns::channelType_t, spModel_t> pairNameAndModel_t;

	Ui::ModbusWidgetChannelsTable ui;	// Основной виджет сделан в дизайнере
	QVector<pairNameAndModel_t> mv_modelVector;	// Вектор содержит пары "Имя типа канала" и "Модель данных", содержащую таблицу с значениями стартового канала и кол-ва
	spModel_t m_currentModel;	// Указатель на текущую видимую модель, определяется при смене вкладок тулбокса
	uint16_t m_maxChannelsCount;	// ДОЛЖНО РАССЧИТЫВАТЬСЯ! Пока ставится по умолчанию 64

	QToolBox *m_toolBox;	// Тулбокс добавляем и заполняем вручную
	bool m_isModifyModbus;

private:

	// Построение виждетов
	bool buildToolBoxTable();
	void fillModelWithHeaders(spModel_t model);
	void addEmptyRowToModel(spModel_t model);

	// Расчеты
	int checkNewTablePosition(ModbusMaster_ns::channelType_t type);
	ModbusMaster_ns::channelType_t getCurrentChannelType();
	int getTableIndexForType(ModbusMaster_ns::channelType_t type);

signals:
	// При заполнение необходимых полей посылает посылку с данными для создания новой команды в слейве
	void signal_updateChannelRequest(const ModbusMaster_ns::structCommand_t &);
	void signal_eraseChannelRequest(const ModbusMaster_ns::structCommand_t &);	

public slots:

	// Принимает от главного виджета таблицу какого типа создать или удалить
	void slot_addTable(ModbusMaster_ns::channelType_t);
	void slot_removeTable(ModbusMaster_ns::channelType_t);

	// Принимает от главного виджета сигнал о добавлении уже существующей команды 
	void slot_addExistingCommand(const ModbusMaster_ns::structCommand_t &);
	void slot_removeCommand(const ModbusMaster_ns::structCommand_t &);

	// Управление таблицей с каналами
	void slot_changeInTable(QStandardItem *);	// Вводимые данные в таблицу
	void slot_tableChanged(int);	// Изменение вкладки с типов таблицы(каналов)

	void on_butEraseChecked_clicked();
	void on_butAddRow_clicked();
	void on_butCheckAll_clicked();

};

Q_DECLARE_METATYPE(ModbusMaster_ns::structCommand_t)