#pragma once

#include <QObject>
#include "QHBoxLayout"
#include "QLineEdit"
#include "qcheckbox.h"
#include "qcombobox.h"
#include "qvector.h"
#include "qtoolbox.h"
#include "qstandarditemmodel.h"
#include "qtableview.h"
#include <QToolBar>
#include "qpushbutton.h"

#include <data_model/dc_properties.h>

typedef unsigned int channelType_t;

//typedef enum {
//	START_CHANNEL_TYPE = 0,
//	READ_ANALOG_INPUT = 0,
//	READ_DISCRETE_INPUT = 1,
//	READ_COUNTER = 2,
//	READ_ANALOG_OUTPUT = 3,
//	READ_DISCRETE_OUTPUT = 4,
//	WRITE_SET_OF_ANALOG_OUTPUT = 5,
//	WRITE_STATE_OF_DISCRETE_OUTPUT = 6,
//	SERVICE_MANADGMENT_AND_INDICATION = 7,
//	TIME_SYNCHRONIZATION = 8,
//	POLLON_THE_LINK = 9,
//	CHANNEL_TYPES_COUNT = 10
//} channelType_t;


class DcController;

class TabSybusMasterChannels : public QObject
{
	Q_OBJECT

public:
    TabSybusMasterChannels(DcController* controller, int, QObject *parent = nullptr);
	~TabSybusMasterChannels();

	bool build(void);
	QWidget* formChannels(void);
	int32_t error();
	QString errorMessage();

	void updateSlaveList();				// Обновляет комбо бокс с именами ведомых
	bool isAllParametersValid();				// Проверки нет, не может считаться неправильно. Только флаги,

private:
	DcController	*m_controller;		// Указатель на модель контроллера	
	QString strErrorMessage;			// Сообщение с ошибкой для вывода
	int m_port_id = 0;
	int m_portNumber = 0;					// Номер порта
	int m_maxSlaveCount = 0;				// Максимальное кол-во слейвов на одном порте	
	QWidget *widChannels;				// Главный виджет
	int currentSlaveNumber = 0;			// Номер текущего настраиваемого слейва	
	bool settingsIsDone = false;

	int m_maskBytesCount;
	int m_maxChannelsCount;
	QComboBox *comBoxChooseSlave;		// Список с выбором слейва

	QVector<QCheckBox*> vecChBoxFlagsChannels;										// Вектор из чекбоксов, показывающих выбор использования типа каналов
	QVector<unsigned int> vecMaxChannelCountOfType;									// Хранит максимальное кол-во каналов для каждого типа
	QVector< QVector<channelType_t> > vecCurrentChannelCountOfTypeInSlave;			// Вектор из слейвов с текущим количеством выбранных каналов каждого типа
	QVector<QPair<channelType_t, QStandardItemModel*>> vectorChannelTableModels;	// Вектор из указателей на модели данных, содержащих инфо о включенных каналах

	QPushButton* buttonDeleteRow;
	QPushButton* buttonAddRow;
	QPushButton* buttonChooseAll;

	QToolBox *toolBoxChannels;							// Вкладки с таблицами
	QTableView *tabViewChannels;						// Табличное представление модели 
	QStandardItemModel *modelAnalogInputs;				// модель хранит состояния каналов аналоговых выходов

   
	
// Вспомогательные функции		
	// Формирование виджета с таблицами
	bool checkStateChBoxFlags(channelType_t channelType);
	void addTableToWidget(channelType_t channelType);							// Добавляет на новую вкладку новую таблицу
	void deleteTableFromWidget(channelType_t channelType);						// Удаляет таблицу, ищет по channelType
	int getModelIndexInToolBox(channelType_t channelType);						// Проверяет, есть ли такой тип таблицы уже, возвращает индекс модели в таблице
	int checkNewTablePosition(channelType_t channelType);						// Проверяет куда вставлять новую таблицу, чтобы было отсортировано
	
	// Действия с таблицами
	int getCurrentChannelType();												// Возвращает тип текущей открытой вкладки
	QStandardItemModel *getModelOfType(channelType_t channelType);				// Ищет нужную модель по типу
	void readFlagsFillChannelTable(QStandardItemModel* currentModel, int channelType);	// При создании таблицы вычитывает значения из шаблона контроллера и обновляет таблицу
	void addEmptyRowToModel(QStandardItemModel *);								// Добавляет пустую строку в модель
	void createRowInTable(channelType_t channelType, QStandardItemModel* currentModel, int startChannel, int channelCount);	// Создает табллицу с указанными параметрами
	void removeRowFromTable(QStandardItemModel* currentModel, int deleteRow);	// Удаляет указанную строчку из модели

	// Вычитывание данных из модели контроллера
	int getChannelAddress(int channelType);										// Возвращает адрес в шаблоне контроллера, где хранится выбранный тип канала
	void readFlagsFromTemplateAddTableToWidget();								// Считываются все типы каналов из шаблона МК, и если канал включен, добавляем таблицу на вкладки
	
	// Действия с флагами каналов
	bool getFlagFromChannelMask(int channelType, int channelNumber);			// Узнаем, включен ли нужный канал в таблице с типом channelType
	bool setFlagToChannelMask(int channelType, int channel, bool state);		// Устанавливаем флаг на выбранный канал в выбранный тип таблицы
	void clearFlagsInTemplate(int channelType);									// Обнуляет все флаги в выбранном типе таблицы
	void UpdateFlags(channelType_t channelType, QStandardItemModel* currentModel);

	// Проверки
	void fillVectorMaxChannelCountOfType();										// Заполняет вектор максимальным кол-вом каналов выбранного типа. ДОДЕЛАТЬ ДЛЯ ДРУГИХ ТИПОВ
	int getProfileCountOfAddress(unsigned int address);							// Считает размерность этого адреса
	bool isTotalAmountOfChannelsExceeded(channelType_t channelType);			// Проверяет сумму каналов со всех слейвов
	unsigned int getSlaveChannelCountOfType(channelType_t channelType);			// Подсчитывает число включенных каналов в текущем слейве
	bool checkAvailablilityOfChannelType();										// Проходит по вектору максимальных количеств каналов всех типов, и если оно равно 0, выключает доступность чекбокса

	// Ошибки
	void showErrorExceedMaxChannelCount(channelType_t);

	// Подсчеты
	unsigned int getMaxChannelCount(channelType_t);								// Получает максимум данного типа
	int getParameterCount(int address);
	int getIndexAtAddressForSlave(int parameterIndex);							// Облегчает получение смещения для адреса, если несколько слейвов (зависит от номера текущего выбранного)

public slots:

	void slotChangeComBoxCurrentSlaveIndex(int newSlaveIndex);	// Обработчик изменения текущего слейва!
	void slotCommonChBoxSlotForFlags(int);		// Обработчик сигналов со всех чекбоксов, добавляет/удаляет таблицы на вкладках
	void slotChangeInTable(QStandardItem *);	// Слот для изменений в любой таблице, обновляет данные	
	
	// Кнопки
	void slotClickedDeleteRowButton(bool);		// Кнопка Удалить (строку из таблицы)
	void slotClickedAddRowButton(bool);			// Кнопка добавить строку в таблицу
	void slotClickedChooseAllButton(bool);


};
