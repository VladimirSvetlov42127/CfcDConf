#include "TabSybusMasterChannels.h"

#include <qtableview.h>
#include <qsizepolicy.h>
#include <qsplitter.h>
#include <QLabel>
#include <QDebug>

#include <dpc/gui/widgets/CheckableHeaderView.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>					// Всплывающий окна с ошибками и предупреждениями


#include "data_model/dc_controller.h"


#include "gui/forms/interfaces_protocols/RS485/macrosCheckPointer.h"
#include "gui/forms/interfaces_protocols/RS485/Sybus/master/definesSlaveMode.h"
#include "gui/forms/interfaces_protocols/RS485/DcRs485Defines.h"

using namespace Dpc::Gui;


//static const QString STR_FLAG_READ_ANALOG_INPUT("Чтение аналоговых каналов ТИ");
//static const QString STR_FLAG_READ_DISCRETE_INPUT("Чтение дискретных каналов ТС");
//static const QString STR_FLAG_READ_COUNT_INPUT("Чтение счетных каналов ТИИ");
//static const QString STR_FLAG_READ_ANALOG_OUTPUT("Чтение состояния аналоговых выходов");
//static const QString STR_FLAG_READ_DISCRETE_OUTPUT("Чтение состояния дискретных выходов");
//static const QString STR_FLAG_WRITE_SET_OF_ANALOG_OUTPUT("Запись установок аналоговых выходов");
//static const QString STR_FLAG_WRITE_STATE_OF_DISCRETE_OUTPUT("Запись состояний управления ТУ дискретных выходов");
//static const QString STR_FLAG_SERVICE_MANADGMENT_AND_INDICATION("Обслуживание устройства отображения и индикации");
//static const QString STR_FLAG_TIME_SYNCHRONIZATION("Синхронизация времени");
//static const QString STR_FLAG_POLLON_THE_LINK("Опрос на наличие связи");


// !!! Для изменения размера маски, нужно отредактировать ф-ию getChannelAddress, где используется размер поля маски

//static const QString STR_COLOMN_NAME("Имя канала");				// Имена заголовков в таблицах
static const QString STR_COLOMN_NAME("");				// Имена заголовков в таблицах

static const QString STR_COLOMN_NUMBER("Номер");
static const QString STR_COLOMN_COUNT("Количество");

//static const QString STR_DEFAULT_CHANNEL_NAME("Канал ");		// Для формирования канала с номером
static const QString STR_DEFAULT_CHANNEL_NAME("");
static const QString STR_SLAVE("Ведомый ");

static const QString ERR_STR_NOT_IN_RANGE("Ошибка! Значение не входит в допустимый интервал!");

//#define MAX_CHANNELS_COUNT			32		// Максимум каналов 
//#define MASK_BYTES_COUNT				4		// Количество байт на каждую маску канала

#define SLAVE_COUNT_IN_1_BYTE		8		// Количество бит в 1 байте, характеризует макс кол-во каналов в байте

#define MIN_ROW_COUNT				1		// Кол-во строк при создании таблицы
#define COLOMN_MAX_COUNT			3		// Количество колонок в таблицах
#define COLOMN_NAME					0		// Номер колонки в таблице
#define COLOMN_CHANNEL				1
#define COLOMN_COUNT				2

#define	CH_BOX_CHANNEL_TYPES_COUNT				10		// Ключевой момент! Количество чекбоксов. Каждый чекбокс определяет свой тип таблицы. 
#define	CHANNEL_TYPES_COUNT						7		// Но из 10 чекбоксов тне все формируют таблицы!
#define MAX_CHANNEL_TYPE						CHANNEL_TYPES_COUNT - 1		// Максимально допустимый тип 
#define MIN_CHANNEL_TYPE						0

#define LINE_HIGHT					25

#define BUTTON_WIDHT				120

static const QVector<QString> vectorChBoxNames { "Чтение аналоговых каналов ТИ", 
												"Чтение дискретных каналов ТС",
												"Чтение счетных каналов ТИИ",
												"Чтение состояния аналоговых выходов",
												"Чтение состояния дискретных выходов",
												"Запись уставок аналоговых выходов",
												"Запись состояний управления ТУ дискретных выходов",
												"Обслуживание устройства отображения и индикации",
												"Синхронизация времени",
												"Опрос на наличие связи"
												};		// Вектор из допустимых скоростей

/*const*/ struct channelDataMin_s{
	channelType_t channelType;
	DefSignalDirection direction;
	DefSignalType type;

};


static const QVector<channelDataMin_s> vecChannelsInfo {	{MIN_CHANNEL_TYPE + 0, DEF_SIG_DIRECTION_INPUT, DEF_SIG_TYPE_ANALOG},
															{MIN_CHANNEL_TYPE + 1, DEF_SIG_DIRECTION_INPUT, DEF_SIG_TYPE_DISCRETE},
															{MIN_CHANNEL_TYPE + 2, DEF_SIG_DIRECTION_INPUT, DEF_SIG_TYPE_COUNTER},
															{MIN_CHANNEL_TYPE + 3, DEF_SIG_DIRECTION_OUTPUT, DEF_SIG_TYPE_ANALOG},
															{MIN_CHANNEL_TYPE + 4, DEF_SIG_DIRECTION_OUTPUT, DEF_SIG_TYPE_DISCRETE},
															{MIN_CHANNEL_TYPE + 5, DEF_SIG_DIRECTION_OUTPUT, DEF_SIG_TYPE_UNDEF},			// Не уверена какой тип! 
															{MIN_CHANNEL_TYPE + 6, DEF_SIG_DIRECTION_OUTPUT, DEF_SIG_TYPE_UNDEF},			// Не уверена какой тип! 	

};

TabSybusMasterChannels::TabSybusMasterChannels(DcController *controller, int portNumber, QObject *parent)
	: QObject(parent), m_portNumber(portNumber), m_maskBytesCount(0), m_maxChannelsCount(0)
{
	m_port_id = portNumber * 256;
    m_controller = controller;
	widChannels = new QWidget;

	comBoxChooseSlave = new QComboBox;
	buttonDeleteRow = new QPushButton;
	buttonAddRow = new QPushButton;
	buttonChooseAll = new QPushButton;

	toolBoxChannels = new QToolBox;
	tabViewChannels = new QTableView;
	modelAnalogInputs = new QStandardItemModel;

    auto psetting = m_controller->setting("MASK_BYTES_COUNT");
	if (psetting != nullptr) {
		m_maskBytesCount = psetting->value().toInt();
		m_maxChannelsCount = m_maskBytesCount * 8;
	}
}

TabSybusMasterChannels::~TabSybusMasterChannels()
{
}


bool TabSybusMasterChannels::build(void) {

	// Получаем номер текущего слейва
	updateSlaveList();											// Обновляет КомбоБокс с доступными ведомыми
	fillVectorMaxChannelCountOfType();									// Вычитываем максимальные количества каналов всех типов

	QObject::connect(comBoxChooseSlave, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChangeComBoxCurrentSlaveIndex(int)));	// При смене ведомого обновляет все поля
		
	int maxSlaveCountOfAllPorts = getParameterCount(SP_USARTEXTNET_DEFINITION);
	int portCount = getParameterCount(SP_USARTPRTPAR_BYTE);

	m_maxSlaveCount = maxSlaveCountOfAllPorts / portCount;		// Рассчитываем кол-во слейвов на одном порте
	
	QLabel *labSlaveId = new QLabel;
	labSlaveId->setText("Выберите ведомого для настройки:");

	QHBoxLayout *mainHorBoxLayout = new QHBoxLayout;			// Основное размещение по стоблцам: чекбосы, правее таблицы
	QVBoxLayout* verFlagsChBoxLayout = new QVBoxLayout;			// Первый столбец с чекбоксами
	mainHorBoxLayout->addItem(verFlagsChBoxLayout);				// Вставляем первый столбец с чекбоксами
	verFlagsChBoxLayout->addWidget(labSlaveId);					// Добавляем текст в первый столбец
	verFlagsChBoxLayout->addWidget(comBoxChooseSlave);			// Добавляет выбор слейва	
	
	comBoxChooseSlave->setMinimumHeight(LINE_HIGHT);			// Устанавливаем фиксированную высоту (иначе мелко все)	
	labSlaveId->setMinimumHeight(LINE_HIGHT);

	verFlagsChBoxLayout->addWidget(new QLabel);					// Пустой лейбл будет разделителем

	for (int box = 0; box < CH_BOX_CHANNEL_TYPES_COUNT; box++) {	// Заполняем вектор чекбоксов!
		
		QCheckBox *newChBox = new QCheckBox;					// Создаем пустой новый чекбокс
		vecChBoxFlagsChannels.push_back(newChBox);				// Добавляем в вектор
		
		if (box < CHANNEL_TYPES_COUNT && vecMaxChannelCountOfType[box] == 0) {		// Проверяем, если общее количество каналов не указано, не отображаем чекбокс
			continue;
		}

		newChBox->setText(vectorChBoxNames[box]);		// Даем имя. Важен порядок следования имен!!!
		newChBox->setMinimumHeight(LINE_HIGHT);					// Устанавливаем фиксированную длину
		verFlagsChBoxLayout->addWidget(newChBox);				// Размещаем все чекбоксы в первом столбце
	}
	verFlagsChBoxLayout->addStretch(1);							// Добавляем пружинку снизу, чтобы не растягивался текст

	readFlagsFromTemplateAddTableToWidget();					// Читаем флаги, настраиваем чекбоксы и создаем таблицы
		
	// Добавляем таблицы на разные панели виджета
	QVBoxLayout* verButtonsAndToolBoxTables = new QVBoxLayout;			// Слой для кнопок и таблиц
	mainHorBoxLayout->addLayout(verButtonsAndToolBoxTables);			// Добавляем в основной
	mainHorBoxLayout->addStretch(1);
	QToolBar *toolBarTableButtons = new QToolBar;						// Тулбар для таблиц
	verButtonsAndToolBoxTables->addWidget(toolBarTableButtons);			// В слой добавляем панель кнопок и таблиц
	verButtonsAndToolBoxTables->addWidget(toolBoxChannels);
		
	// Настраиваем кнопки на панели над таблицей
	toolBarTableButtons->addWidget(buttonDeleteRow);
	toolBarTableButtons->addWidget(buttonAddRow);
	toolBarTableButtons->addWidget(buttonChooseAll);

	buttonChooseAll->setMinimumWidth(BUTTON_WIDHT);
	buttonDeleteRow->setText("Удалить выделенное");
	QObject::connect(buttonDeleteRow, SIGNAL(clicked(bool)), this, SLOT(slotClickedDeleteRowButton(bool)));
	
	buttonAddRow->setMinimumWidth(BUTTON_WIDHT);
	buttonAddRow->setText("Добавить строку");
	QObject::connect(buttonAddRow, SIGNAL(clicked(bool)), this, SLOT(slotClickedAddRowButton(bool)));
	
	buttonChooseAll->setMinimumWidth(BUTTON_WIDHT);
	buttonChooseAll->setText("Выбрать все");
	buttonChooseAll->setCheckable(true);
	QObject::connect(buttonChooseAll, SIGNAL(toggled(bool)), this, SLOT(slotClickedChooseAllButton(bool)));

	widChannels->setLayout(mainHorBoxLayout);							// Устанавливаем в виждет главный слой
	
																		// Начинаем проверку сумму всех каналов

	// Проходим по предустановленным данным, чтобы проверить правильность данных
	int oldSlave = currentSlaveNumber;									// Сохраняем старый номер
	int slave = 0;
	do {
		comBoxChooseSlave->setCurrentIndex(slave);
		checkAvailablilityOfChannelType();									// Проверяем, все ли значения максимума каналов не равны нулю и поддерживаются
		slave++;
	} while (slave < comBoxChooseSlave->count());

	comBoxChooseSlave->setCurrentIndex(oldSlave);		// Возвращаемся на первый слейв
	
	// Проверяем и выдаем ошибки
	for (channelType_t type = 0; type < CHANNEL_TYPES_COUNT; type++) {		// Проходим по всем типам каналов
		bool notValid;
		notValid = isTotalAmountOfChannelsExceeded(type);				// Проверяем сумму значений по всем слейвам
		if (notValid) {
			//showErrorExceedMaxChannelCount(type);
		}
	}
	settingsIsDone = true;

	for (int box = 0; box < CH_BOX_CHANNEL_TYPES_COUNT; box++) {
		if (box < CHANNEL_TYPES_COUNT && vecMaxChannelCountOfType[box] == 0) {		// Проверяем, если общее количество каналов не указано, не отображаем чекбокс
			continue;
		}
		QObject::connect(vecChBoxFlagsChannels[box], SIGNAL(stateChanged(int)), this, SLOT(slotCommonChBoxSlotForFlags(int)));
	}


	return true;
}

QWidget* TabSybusMasterChannels::formChannels(void) {
	return widChannels;
}

int32_t TabSybusMasterChannels::error() {
	return 666;
}

QString TabSybusMasterChannels::errorMessage() {
	return strErrorMessage;
}


void TabSybusMasterChannels::updateSlaveList() {

    auto slaveCount = m_controller->paramsRegistry().element(SP_USARTPRTPAR_BYTE, m_port_id + Rs485::Indexs::Byte::SlaveCount);
	CHECK_ADDRESS(slaveCount);

	currentSlaveNumber = 0;			// Переходим на первого слейва

	comBoxChooseSlave->clear();															// Очищаем предыдущий список
	int newSlaveCount = slaveCount->value().toInt();

	QString strSlaveName;
	if (newSlaveCount != 0) {			// Если есть слейвы		
		for (int i = 0; i < newSlaveCount; i++) {							// Заполняем список из номеров ведомых
			strSlaveName = STR_SLAVE + QString::number(i + 1);							// Слейвы начинаются с 1! Для удобства пользователя
			comBoxChooseSlave->addItem(strSlaveName);
		}
	}
	else {	
		newSlaveCount = 1;
		comBoxChooseSlave->addItem("Комбокс не должен быть пустым"); // Чтобы сформировать полный виджет. При нуле он просто неативен. 
	}

	vecCurrentChannelCountOfTypeInSlave.resize(newSlaveCount);
	for (int i = 0; i < newSlaveCount; i++) {
		vecCurrentChannelCountOfTypeInSlave[i].resize(CHANNEL_TYPES_COUNT);
	}
}

bool TabSybusMasterChannels::isAllParametersValid() {
	bool allIsValid = true;

	for (int channelType = 0; channelType < vecMaxChannelCountOfType.size(); channelType++) {
		bool maxCountExceed = isTotalAmountOfChannelsExceeded(channelType);

		if (maxCountExceed) {
			if (settingsIsDone) {
				showErrorExceedMaxChannelCount(channelType);			// Показывает сообщение об ошибке
				allIsValid = false;
			}
		}
	}


	return allIsValid;
}

void TabSybusMasterChannels::fillVectorMaxChannelCountOfType() {

	vecMaxChannelCountOfType.resize(CHANNEL_TYPES_COUNT);			// Устанавливаем размер вектора
	int position = 0;

	// Аналоговые входы
	//channelType_t analogInputSize = getProfileCountOfAddress(ns_slaveModeDefines::ADDRESS_ANALOG_INPUT_NAMES);		// Может вернуть 0, если нет такого 
	//vecMaxChannelCountOfType[position++] = analogInputSize;
	//
	//channelType_t discreteInputSize = getProfileCountOfAddress(ns_slaveModeDefines::ADDRESS_DISCRETE_INPUT_NAMES);
	//vecMaxChannelCountOfType[position++] = discreteInputSize;
	//
	//channelType_t counterInputSize = getProfileCountOfAddress(ns_slaveModeDefines::ADDRESS_COUNTER_INPUT_NAMES);
	//vecMaxChannelCountOfType[position++] = counterInputSize;


	//unsigned int discreteOutputSize = getProfileCountOfAddress(ns_slaveModeDefines::ADDRESS_DISCRETE_OUTPUT_NAMES);
	//vecMaxChannelCountOfType[4] = discreteOutputSize;
	
	//for (int i = 0; i < vecMaxChannelCountOfType.count(); i++) {
	//	qDebug() << i << " = " << vecMaxChannelCountOfType[i];
	//}

	for (channelType_t channeltype = 0; channeltype < CHANNEL_TYPES_COUNT; channeltype++ ) {

		vecMaxChannelCountOfType[position++] = getMaxChannelCount(channeltype);
		//qDebug() << "max = " << vecMaxChannelCountOfType[position - 1];
	}

	// Доделать остальные
		
	return;
}

int TabSybusMasterChannels::getProfileCountOfAddress(unsigned int address)
{
	int index = 0;		// Индекс для чтения
    while(m_controller->paramsRegistry().element(address, index++)) {
    };

	return index - 1;		// Даже если размерность ноль, индекс был увеличен, поэтому -1
}

void TabSybusMasterChannels::readFlagsFromTemplateAddTableToWidget() {

	for (int channelType = 0; channelType < vecChBoxFlagsChannels.size(); channelType++) {		// Все чекбоксы хранятся в векторе

		QCheckBox *box = vecChBoxFlagsChannels[channelType];		// Берем чекбокс из вектора

		if (checkStateChBoxFlags(channelType) == true) {		// Проверяем значение флага в модели контроллера
			box->setChecked(true);								// И устанавливаем флаг, если канал используется

			if (channelType < CHANNEL_TYPES_COUNT) {			// Количество чекбосов больше, чем реализованных таблиц, поэтому проверяем
				addTableToWidget(channelType);					// Добавляем таблицу на вкладку
			}
		}
		else {
			box->setChecked(false);								// И снимаем флаг, если канал не используется
		}
	}
}

bool TabSybusMasterChannels::checkStateChBoxFlags(channelType_t channelType) {
	   	
	bool channelIsOn = false; 

	if (channelType < 8) {
        auto currentSlaveOptions1 = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_1));
		CHECK_ADDRESS_RETURN_BOOL(currentSlaveOptions1);
		int currentSlaveMode1 = currentSlaveOptions1->value().toInt();		// Флаги в итовом формате

		if (currentSlaveMode1 & (1 << channelType)) {
			channelIsOn = true; 
		}

	}
	else if (channelType < 16) {
        auto currentSlaveOptions2 = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_2));
		CHECK_ADDRESS_RETURN_BOOL(currentSlaveOptions2);
		int currentSlaveMode2 = currentSlaveOptions2->value().toInt();		// Флаги в итовом формате

		int bit;

		if (channelType == 8) {
			bit = 14 % 8;
		}
		else if (channelType == 9) {
			bit = 15 % 8;
		}

		if (currentSlaveMode2 & (1 << bit)) {
			channelIsOn = true;
		}
	}

	return channelIsOn;
}

int TabSybusMasterChannels::getCurrentChannelType() {

	int channelType = -1;			// Если такого типа нет

	if (toolBoxChannels->count() > 0) {
		
		QTableView *tabViewChannels;		// Промежуточный варинт. В нем хранится модель
		tabViewChannels = qobject_cast<QTableView *> (toolBoxChannels->currentWidget());	// Преобразуем в тип

		QStandardItemModel* model = qobject_cast<QStandardItemModel *>(tabViewChannels->model());

		for (int i = 0; i < vectorChannelTableModels.size(); i++) {			// Ищем какой тип у данной модели
			if (vectorChannelTableModels[i].second == model) {
				channelType = vectorChannelTableModels[i].first;
				break;
			}
		}

	}

	return channelType;
}

void TabSybusMasterChannels::addTableToWidget(channelType_t channelType) {

	if (getModelIndexInToolBox(channelType) >= 0 || channelType > MAX_CHANNEL_TYPE) {		// Проверяем, есть ли такой тип уже в таблице, если да, ничего не делаем
		return;
	}

	QTableView *tabViewChannelsNew = new QTableView;						// Табличное представление модели 
	auto myHeader = new CheckableHeaderView(Qt::Horizontal, tabViewChannelsNew);	// Создается пользовательский хедер с чекбоксом
	myHeader->setEnabledChecking(false);
	tabViewChannelsNew->setHorizontalHeader(myHeader);

	QStandardItemModel *newModel = new QStandardItemModel;

	int newPosition = checkNewTablePosition(channelType);
	vectorChannelTableModels.insert(newPosition, qMakePair(channelType, newModel));
	toolBoxChannels->insertItem(newPosition, tabViewChannelsNew, vectorChBoxNames[channelType]);
    toolBoxChannels->setItemIcon(newPosition, QIcon(":/icons/proj.svg"));

	QStandardItem* itemName = new QStandardItem();		// Устанавливаем имена заголовков
	itemName->setCheckable(false);
	itemName->setCheckState(Qt::Unchecked);
	//itemName->setText(STR_COLOMN_NAME);
	newModel->setHorizontalHeaderItem(COLOMN_NAME, itemName);

	QStandardItem* itemSlaveChannel = new QStandardItem();			// Устанавливаем имена заголовков
	itemSlaveChannel->setCheckable(false);
	itemSlaveChannel->setText(STR_COLOMN_NUMBER);
	newModel->setHorizontalHeaderItem(COLOMN_CHANNEL, itemSlaveChannel);

	QStandardItem* itemCount = new QStandardItem();					// Устанавливаем имена заголовков
	itemCount->setCheckable(false);
	itemCount->setText(STR_COLOMN_COUNT);
	newModel->setHorizontalHeaderItem(COLOMN_COUNT, itemCount);

	readFlagsFillChannelTable(newModel, channelType);								// Читаются флаги из модели контроллера, заполняется modelAnalogInputs
	addEmptyRowToModel(newModel);							// Добавляем пустую строку

	tabViewChannelsNew->setSelectionBehavior(QAbstractItemView::SelectRows);
	tabViewChannelsNew->setSelectionMode(QAbstractItemView::SingleSelection);
	//tabViewChannelsNew->resizeRowsToContents();
	//tabViewChannelsNew->resizeColumnsToContents(); 
	//tabViewChannelsNew->setFi(COLOMN_NAME);

	tabViewChannelsNew->setModel(newModel);

	tabViewChannelsNew->setColumnWidth(COLOMN_NAME, 10);

	QObject::connect(newModel, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(slotChangeInTable(QStandardItem *)));

	return;
}


void TabSybusMasterChannels::deleteTableFromWidget(channelType_t channelType) {

	int modelIndexInToolBox = getModelIndexInToolBox(channelType);

	if (modelIndexInToolBox < 0 || channelType > MAX_CHANNEL_TYPE) {		// Если нет такого в списке, значит не можем удалить
		return;
	}

	int remInd = -1;		// Ищем модель в векторе, чтобы из вектора удалить
	for (int tableIndex = 0; tableIndex < vectorChannelTableModels.size(); tableIndex++) {	// Проходим по вектору моделей

		if (vectorChannelTableModels[tableIndex].first == channelType) {		// Ищем, есть ли модель с таким типов
			remInd = tableIndex;		// Если есть, сохраняем индекс
			break;
		}
	}
	
	if (remInd < 0) {
		qDebug() << "Нет такого типа в векторе! тип = " << channelType;
		return;
	}

	QWidget *removeWidget = toolBoxChannels->widget(modelIndexInToolBox);	// Получаем виджет
	toolBoxChannels->removeItem(modelIndexInToolBox);						// Удаляем виджет из тулбокса
	delete removeWidget;													// Высвобождаем память
	delete vectorChannelTableModels[remInd].second;							// Высвобождаем память из вектора
	vectorChannelTableModels.remove(remInd);								// Удаляем элемент из вектора
	
	vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] = 0;	// Обнуляем кол-во включенных каналов данного типа в этом слейве

}

int TabSybusMasterChannels::getModelIndexInToolBox(channelType_t channelType) {			// Проверяет, есть ли такой тип таблицы уже, возвращает индекс модели в таблице

	int index = - 1;					// Сохраняем индекс в векторе при поиске соответствующего типа 

	QStandardItemModel *foundModel = getModelOfType(channelType);

	if (foundModel == nullptr) {					// Если ничего не нашли в векторе, значит таблицы нет
		return index;
	}

	QTableView *tabViewChannels;		// Промежуточный варинт. В нем хранится модель
	for (int i = 0; i < toolBoxChannels->count(); i++) {		// Проходим по всем вкладкам, ищем модель

		tabViewChannels = qobject_cast<QTableView *> (toolBoxChannels->widget(i));	// Преобразуем в тип
		
		if (tabViewChannels->model() == foundModel) {	// Если модель совпадает в найденой в векторе
			index = i;
			break;
		}
	}

	return index;
}

int TabSybusMasterChannels::checkNewTablePosition(channelType_t channelType) {
	int position = vectorChannelTableModels.size();

	for (int i = 0; i < vectorChannelTableModels.size(); i++) {
		if (channelType < vectorChannelTableModels[i].first) {
			position = i;
			break;
		}
	}
	
	return position;
}

QStandardItemModel *TabSybusMasterChannels::getModelOfType(channelType_t channelType) {

	QStandardItemModel *foundModel = nullptr;

	for (int tableIndex = 0; tableIndex < vectorChannelTableModels.size(); tableIndex++) {	// Проходим по вектору, ищем, есть ли уже такая модель

		if (vectorChannelTableModels[tableIndex].first == channelType) {	// Если модель с таким же типом нашлась
			foundModel = vectorChannelTableModels[tableIndex].second;
			break;
		}
	}

	return foundModel;	
}

void TabSybusMasterChannels::readFlagsFillChannelTable(QStandardItemModel* currentModel, int channelType) {
	
	int channelBit;					// Расчитывыем в какой бит должен быть записан данный канал
	int channelMaskByte;			// Смотрим, в какой из 4х байт надо записать значение	
	bool flagInOn = false;			// Стоит ли флаг на текущем канале
	bool nextFlagIsOn = false;		// Смотрим на следующий канал
	int OnChannelsCounter = 0;		// Если подряд идет больше двух, будем записывать их подряд в одной строке
	int startChannel = 0;			// Первый включенный канал

	vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] = 0;			// Обнуляем кол-во включенных каналов в текущем слейве в текущем типе

	for (int checkChannel = 0; checkChannel < m_maxChannelsCount; checkChannel++) {		// Проходим по всем каналам
		flagInOn = getFlagFromChannelMask(channelType, checkChannel);					// Узнаем, включен ли этот бит
		
		if (flagInOn) {						// Если текущий флаг включен
			if (startChannel < 0) {
				startChannel = checkChannel;
			}

			OnChannelsCounter++;			// Увеличиваем число включенных подряд

			nextFlagIsOn = getFlagFromChannelMask(channelType, checkChannel + 1);			// Проверяем следующий флаг, здесь выход за допустимый массив, проверка в функции осуществляется

			if (!nextFlagIsOn) {							// Если выключен, последовательность прервалась
				createRowInTable(channelType, currentModel, startChannel + 1, OnChannelsCounter);		// В таблице каналы начинаются с 1!
				vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] += OnChannelsCounter;	// Сохраняем в текущий слейв в текущий тип
			}
		}
		else {
			OnChannelsCounter = 0;
			startChannel = -1;
		}
	}
	 
	bool maxCountExceed = isTotalAmountOfChannelsExceeded(channelType);

	if (maxCountExceed) {
		if (settingsIsDone) {
			showErrorExceedMaxChannelCount(channelType);
		}		
	}

	/*qDebug() << "readFlagsFillChannelTable";
	qDebug() << "slave = " << currentSlaveNumber << ", type = " << channelType << "current count = " << vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType];*/
}

void TabSybusMasterChannels::addEmptyRowToModel(QStandardItemModel * model) {
	
	CHECK_ADDRESS(model);
	
	QList<QStandardItem*> row;
	for (int colomn = 0; colomn < model->columnCount(); colomn++)
	{
		QStandardItem* item = new QStandardItem();
		item->setTextAlignment(Qt::AlignCenter);

		if (colomn == COLOMN_NAME) {
			item->setCheckable(true);
			item->setEditable(false);
			//item->setText(STR_DEFAULT_CHANNEL_NAME + QString::number(model->rowCount() + 1));
		}
		row.append(item);
	}
	model->appendRow(row);
}

bool TabSybusMasterChannels::getFlagFromChannelMask(int channelType, int channelNumber) {

	if (channelNumber >= m_maxChannelsCount || channelNumber < 0) return false;		// Проверка значения

	int channelBit = channelNumber % SLAVE_COUNT_IN_1_BYTE;			// расчитывыаем в какой бит должен быть записан данный канал
	int channelMaskByte = channelNumber / SLAVE_COUNT_IN_1_BYTE;	// Смотрим, в какой из 4х байт надо записать значение	
	bool flagIsOn = false;

	int channelAddress = getChannelAddress(channelType);			// Получаем смещение по адресу, относительно первой маски

    auto channelMask = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(channelAddress + channelMaskByte));
	CHECK_ADDRESS_RETURN_BOOL(channelMask);
	int currentMask = channelMask->value().toInt();		// Текущая маска

	if (currentMask & (1 << channelBit)) {		// Если нужный каналу бит включен
		flagIsOn = true;				// Указываем это
	}

	return flagIsOn;
}

int TabSybusMasterChannels::getChannelAddress(int channelType) {
	if (channelType > MAX_CHANNEL_TYPE) {
		return -1;
	}
	return (ns_slaveModeDefines::MASK_READ_ANALOG_INPUT + m_maskBytesCount * channelType);
}

bool TabSybusMasterChannels::setFlagToChannelMask(int channelType, int channelNumber, bool state) {

	if (channelNumber > m_maxChannelsCount || channelNumber < 0) return false;		// Проверка значения

	int channelBit = channelNumber % SLAVE_COUNT_IN_1_BYTE;			// расчитывыаем в какой бит должен быть записан данный канал
	int channelMaskByte = channelNumber / SLAVE_COUNT_IN_1_BYTE;	// Смотрим, в какой из 4х байт надо записать значение	

	int channelAddress = getChannelAddress(channelType);
	if (channelAddress < 0) return false;

    auto channelMask = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(channelAddress + channelMaskByte));
	CHECK_ADDRESS_RETURN_BOOL(channelMask);
	int currentMask = channelMask->value().toInt();		// Текущая маска
	int newMask = currentMask;							// Для новой макси

	if (state) {										// Если канал выбрали
		newMask |= (1 << channelBit);					// Устанавливаем бит
	}
	else {
		newMask &= ~(1 << channelBit);						// Выключаем бит
	}

	channelMask->updateValue(QString::number(newMask));	// Обновляем маску

	return true;
}


void TabSybusMasterChannels::createRowInTable(channelType_t channelType, QStandardItemModel* currentModel, int startChannel, int channelCount) {

	if (startChannel + channelCount > (m_maxChannelsCount + 1)) return;				// Пока кол-во строк умещается в допустимый максимум (а начало каналов будет не с нуля)

	QList<QStandardItem*> row;										// Создаем списко из итемов по количеству стоблцов
	for (int colomn = 0; colomn < currentModel->columnCount(); colomn++)	// Для каждого столбца свой итем
	{
		QStandardItem* item = new QStandardItem;
		item->setTextAlignment(Qt::AlignCenter);

		switch (colomn) {
		case COLOMN_NAME: {
			//item->setText(STR_DEFAULT_CHANNEL_NAME + QString::number(currentModel->rowCount() + 1));	// Делаем имена каналов 
			item->setCheckable(true);										// И чекбокс
			item->setEditable(false);
			break;
		}
		case COLOMN_CHANNEL: {
			item->setText(QString::number(startChannel));
			break;
		}
		case COLOMN_COUNT: {
			item->setText(QString::number(channelCount));
			break;
		}
		}
		row.append(item);											// Добавляем в список
	}
	currentModel->appendRow(row);								// Добавляем в модель

	//vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] += channelCount;	// Обнуляем кол-во включенных каналов данного типа в этом слейве
	//qDebug() << "slave = " << currentSlaveNumber  << ", type = " << channelType << "current count = " << vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType];
}

void TabSybusMasterChannels::removeRowFromTable(QStandardItemModel* currentModel, int deleteRow) {

	CHECK_ADDRESS(currentModel);

	int startChannel = currentModel->item(deleteRow, COLOMN_CHANNEL)->text().toInt();		// Считываем номер канала
	int channelCount = currentModel->item(deleteRow, COLOMN_COUNT)->text().toInt();		// Считываем номер канала
	
	if (startChannel < 0 || channelCount < 0 ) return;

	int channelType;

	for (int i = 0; i < vectorChannelTableModels.size(); i++) {			// Ищем какой тип у данной модели
		if (vectorChannelTableModels[i].second == currentModel) {
			channelType = vectorChannelTableModels[i].first;
			break;
		}
	}

	for (int channel = startChannel; channel < (startChannel + channelCount); channel++) {
		setFlagToChannelMask(channelType, channel, false);
	}

	currentModel->removeRow(deleteRow);
}

void TabSybusMasterChannels::clearFlagsInTemplate(int channelType) {

	int address = getChannelAddress(channelType);			// Получаем начальный адрес поля, где хранится выбранный тип канала

	for (int byte = 0; byte < m_maskBytesCount; byte++) {	// Проходим по всем байтам, выделенным на маски каналов

        auto choosenFlagByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(address + byte));
		CHECK_ADDRESS(choosenFlagByte);

		choosenFlagByte->updateValue(QString::number(NULL));
	}

}

void TabSybusMasterChannels::UpdateFlags(channelType_t channelType, QStandardItemModel* model) {

	CHECK_ADDRESS_SHOW_MESSAGE(model, "Нет данных");
	
	clearFlagsInTemplate(channelType);												// Очищаем все флаги в модели контроллера
	vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] = 0;		// Обнуляем кол-во каналов данного типа для текущего слейва

	int startChannel = 0;
	int channelCount = 0;
	bool isIntStartChannel, isIntCount;
	
	for (int row = 0; row < model->rowCount(); row++) {
		
		startChannel = model->item(row, COLOMN_CHANNEL)->text().toInt(&isIntStartChannel) - 1;		// Потому что нумерация для пользователя не с нуля!
		channelCount = model->item(row, COLOMN_COUNT)->text().toInt(&isIntCount);

		if (!isIntStartChannel || !isIntCount) {		// Если хотя бы одно из полей не число
			continue;									// Переходим на следующую строку
		}
		
		for (int channel = startChannel; channel < (startChannel + channelCount); channel++) {	// По всему кол-ву каналов проходим
			setFlagToChannelMask(channelType, channel, true);					// Записываем канал в шаблон МК
		}		

		// Начинаем считать кол-во включенных каналов, чтобы сохранить в вектор
		int sumOfOnChannels = 0;
		for (int channel = 0; channel < m_maxChannelsCount; channel++) {		// Проходим по всем каналам
			bool channelIsOn = getFlagFromChannelMask(channelType, channel);	// Проверяем, включен ли канал
			if (channelIsOn) {													// Если включен
				sumOfOnChannels++;												// Увеличиваем кол-во включенных каналов
			}
		}
		vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] = sumOfOnChannels;	// Сохраняем в текущий слейв в текущий тип		
	}

	/*qDebug() << "UpdateFlags";
	qDebug() << "slave = " << currentSlaveNumber << ", type = " << channelType << "current count = " << vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType];*/

}

bool TabSybusMasterChannels::isTotalAmountOfChannelsExceeded(channelType_t channelType) {

	if (channelType > MAX_CHANNEL_TYPE || vecMaxChannelCountOfType.count() == 0) {		// Если вне диапазона
		MsgBox::error("Ошибка проверки валидности суммы каналов");
		return true;
	}
	
	if (channelType > vecMaxChannelCountOfType.count() - 1) {		// Если вне поддерживаемого диапазона (не для всех таблиц реализованы ограничения)
	//	MsgBox::warning("У данного типа не реализовано ограничение по количеству каналов");
		return false;
	}

	int sumOfChannels = 0;		// Суммируем все включенные каналы во всех слейвах

	for (int slave = 0; slave < comBoxChooseSlave->count(); slave++) {		// Проходим по каждому слейву
		sumOfChannels += vecCurrentChannelCountOfTypeInSlave[slave][channelType];
	}

	bool result = true;	// Превышено
	if (sumOfChannels <= vecMaxChannelCountOfType[channelType]) {		// Проверяем, меньше ли сумма максимума
		result = false;
	}

	return result;
}

unsigned int TabSybusMasterChannels::getSlaveChannelCountOfType(channelType_t channelType) {

	unsigned int sum = 0;											// Храним сумму

	if (vecChBoxFlagsChannels[channelType]->isChecked() == false) {	// Смотрим на чекбок, если он выключен, то каналы не используется и кол-во равно 0
		return sum;													// Сумма тогда ноль, никакие не включены (таблицы нет на виджете)
	}

	QStandardItemModel *model = getModelOfType(channelType);		// Получаем модель, отвечающую за этот тип
	if (model==nullptr) {											// Если модели нет, значит каналы все выключены, таблицы нет
		return sum;
	}
	
	for (int row = 0; row < model->rowCount(); row++) {				// Проходим по всем строкам таблицы
		sum += model->item(row, COLOMN_COUNT)->text().toInt();		// Складываем количества каналов
	}

	return sum;
}

bool TabSybusMasterChannels::checkAvailablilityOfChannelType() {

	int maxCount = 0;
	QCheckBox *box = nullptr;
	bool noError = true;

	for (int channelType = 0; channelType < vecMaxChannelCountOfType.size(); channelType++) {	// Проходим по всем типам каналов

		maxCount = vecMaxChannelCountOfType[channelType];										// Сохраняем максимальное кол-во каналов для данного типа

		if (maxCount <= 0) {																	// Если оно ноль или меньше, т.е. нет таких каналов в устройстве
			box = vecChBoxFlagsChannels[channelType];											// Получаем соответствующий этому типу чекбокс

			//if (box->checkState() == Qt::Checked) {
			//	noError = false;
			//	MsgBox::error("Обнаружена ошибка в конфигурации!\nИспользование каналов типа \"" + vectorChBoxNames[channelType] + 
			//	"\" недопустимо. \nПараметр исправлен автоматически.");
			//	box->setChecked(false);																// Снимаем галочку (если она была поставлена)
			//}
			
			box->setEnabled(false);																// Делаем недоступным
			vecCurrentChannelCountOfTypeInSlave[currentSlaveNumber][channelType] = 0;			// Обнуляем в расчете

		}

	}

	slotCommonChBoxSlotForFlags(0);		// Просто вызывается ф-ия для считывания полей всех чекбоксов и занесения данных в модель контроллера
	return noError;
}


void TabSybusMasterChannels::showErrorExceedMaxChannelCount(channelType_t type) {

	QString strMax = "Не определено";
	QString stSlavesCount;

	for (int slave = 0; slave < comBoxChooseSlave->count(); slave++) {
		stSlavesCount += "\nВключенных каналов в устройстве " + comBoxChooseSlave->itemText(slave) + " = " + QString::number(vecCurrentChannelCountOfTypeInSlave[slave][type]);
	}

	if (type < vecMaxChannelCountOfType.count()) {
		strMax = QString::number(vecMaxChannelCountOfType[type]);
	}
	MsgBox::error("Обнаружена ошибка в конфигурации мастера Sybus! \nПревышено допустимое количество каналов. \nПроверьте параметр \"" + vectorChBoxNames[type] + "\"" + "\nМаксимальное количество суммы всех каналов: " + strMax + stSlavesCount);
}


unsigned int TabSybusMasterChannels::getMaxChannelCount(channelType_t channelType) {
	
	unsigned int max = 0;
	DefSignalDirection direction = vecChannelsInfo[channelType].direction;
	DefSignalType signalType = vecChannelsInfo[channelType].type;
	
	return m_controller->getSignalList(signalType, DEF_SIG_SUBTYPE_UNDEF, direction).size();
}

int TabSybusMasterChannels::getParameterCount(int address) {

	int maxCount = 0;

	// Начинаем пробавать вычитывать значения по 0 порту, полагая, что кол-во слейвов для всех портов одинаковое
	int index = 0;
    auto checkSlave = m_controller->paramsRegistry().element(address, index);

	while (checkSlave != nullptr) {		// Пока считывается нормально
		maxCount++;						// Увеличиваем число доступных элементов
		index += 256;					// Для одного элемента максимум отводится 256 байт (0..255)
        checkSlave = m_controller->paramsRegistry().element(address, index);	// Считываем с новой позиции
	}

	return maxCount;

}


int TabSybusMasterChannels::getIndexAtAddressForSlave(int parameterIndex) {

	if (currentSlaveNumber < 0) {
		MsgBox::warning("Текущий индекс слейва отрицательный, нельзя сформировать адрес!");
		return -1;
	}

	int slaveIndex = m_portNumber * m_maxSlaveCount * 256;

	return (slaveIndex + currentSlaveNumber * 256 + parameterIndex);		// На каждом порту несколько слейвов может быть
}





void  TabSybusMasterChannels::slotChangeComBoxCurrentSlaveIndex(int newSlaveIndex) {
	
	for (int tab = toolBoxChannels->count() - 1; tab >= 0; tab-- ) {	// Удаляем все вкладки
		delete toolBoxChannels->widget(tab);							// Удаляем виджет
		toolBoxChannels->removeItem(tab);								// Убираем вкладку
	}

	for(QPair<int, QStandardItemModel*> pair: vectorChannelTableModels) {
		delete pair.second;												// Высвобождаем память
	}
	vectorChannelTableModels.clear();									// Очищаем вектор полностью

	currentSlaveNumber = newSlaveIndex;
	if (newSlaveIndex < 0) currentSlaveNumber = 0;						// Если ошибка или нет индекса, будет -1

	for(QCheckBox* box: vecChBoxFlagsChannels) {
		box->blockSignals(true);
	}
	
	readFlagsFromTemplateAddTableToWidget();						// Считываем заново уже с другого слейва, т.к. currentSlaveNumber участвует в формировании адреса запроса!
		
	for(QCheckBox* box: vecChBoxFlagsChannels) {
		box->blockSignals(false);
	}
}

void TabSybusMasterChannels::slotCommonChBoxSlotForFlags(int)
{
	int intChoosenFlagByte;						// Будем высчитывать в какой байт записывать бит
	int channelBit;								// Рассчитываем бит для записи
    ParameterElement* choosenFlagByte;				// Один из двух вариантов, первый или второй байт

	for (int channelType = 0; channelType < vecChBoxFlagsChannels.size(); channelType++) {	// Проходим по всем чекбосам
		QCheckBox * currChBox = vecChBoxFlagsChannels[channelType];							// Получаем текущий чекбокс, отвечающий за данный тип
		
		if (currChBox->isChecked()) {														// Проверяем, выбран ли чекбокс

            if (channelType < 8) {   										// Если номер меньше, значит он относится к 1-му байту хранения, иначе - ко второму
                auto currentSlaveFlags1stByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_1));
				CHECK_ADDRESS(currentSlaveFlags1stByte);
				int currFlags1stByte = currentSlaveFlags1stByte->value().toInt();			// Текущие выставленные флаги в первом байте

				intChoosenFlagByte = currFlags1stByte;										// Выставляем значения, что выбран 1 байт
				channelBit = channelType;													// channelType < 8, поэтому это и есть бит
				choosenFlagByte = currentSlaveFlags1stByte;									// Сохраняем адрес куда записывать будем
			}
			else if (channelType < 16) {

                auto currentSlaveFlags2ndByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_2));
				CHECK_ADDRESS(currentSlaveFlags2ndByte);
				int currFlags2ndByte = currentSlaveFlags2ndByte->value().toInt();			// Текущие выставленные флаги
				
				if (channelType == 8) {
					channelBit = 14 % 8;		// Так прописано в описании
				}
				else if (channelType == 9) {
					channelBit = 15 % 8;
				}
				intChoosenFlagByte = currFlags2ndByte;										// Сохраняем для второго байта
				choosenFlagByte = currentSlaveFlags2ndByte;				
			}	

			int newFlags = intChoosenFlagByte | (1 << channelBit);							// Включаем соответствующий бит
			choosenFlagByte->updateValue(QString::number(newFlags));						// Обновляем значение
			addTableToWidget(channelType);													// При включении создается таблица
		}
		else {	// Если чекбокс не выбран, аналогично всему выше, только выключаем бит и удаляем таблицу
			if (channelType < 8) {															// Если номер меньше, значит он относится к 1-му байту хранения, иначе - ко второму
                auto currentSlaveFlags1stByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_1));
				CHECK_ADDRESS(currentSlaveFlags1stByte);
				int currFlags1stByte = currentSlaveFlags1stByte->value().toInt();			// Текущие выставленные флаги 
				
				intChoosenFlagByte = currFlags1stByte;
				choosenFlagByte = currentSlaveFlags1stByte;
				channelBit = channelType;
			}
			else if (channelType < 16) {

                auto currentSlaveFlags2ndByte = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, getIndexAtAddressForSlave(ns_slaveModeDefines::FLAGS_CHANNELS_2));
				CHECK_ADDRESS(currentSlaveFlags2ndByte);
				int currFlags2ndByte = currentSlaveFlags2ndByte->value().toInt();			// Текущие выставленные флаги

				if (channelType == 8) {
					channelBit = 14 % 8;		// Так прописано в описании
				}
				else if (channelType == 9) {
					channelBit = 15 % 8;
				}

				intChoosenFlagByte = currFlags2ndByte;
				choosenFlagByte = currentSlaveFlags2ndByte;

			}
			int newFlags = intChoosenFlagByte & (~(1 << channelBit));		// Выключаем соответствующий бит
			choosenFlagByte->updateValue(QString::number(newFlags));		// Обновляем значение
			deleteTableFromWidget(channelType);								// Удаляем таблицу
		}
	}
}

void TabSybusMasterChannels::slotChangeInTable(QStandardItem * item) {

	bool wasChanges = false;
	int channelType = getCurrentChannelType();			// Тип текущей таблицы
	
	if (channelType < 0) {
		qDebug() << "Невозможно получить тип данных из виджета";
		return;
	}

	QStandardItemModel* currModel = item->model();		// Текущая модель таблицы	
	Q_ASSERT(currModel != nullptr);		// Если такой модели с измененным итемом не нашлось, такого быть не может!

	currModel->blockSignals(true);		// Блокируем сигналы!!! Нельзя выходить досрочно из функции, в конце их включение!

	int currentRow = item->row();			// Строка текущего измененного итема
	int currentColom = item->column();		// Столбец

	switch (currentColom) {
	case COLOMN_NAME: {

		break;
	}

	case COLOMN_CHANNEL: {
		bool isInt;												// Для проверки, является ли значение числом
		int newStartChannel = item->text().toInt(&isInt) - 1;	// Сохраняем числовое значение начального канала
		
		if (!isInt) {											// Если не число
			if (!item->text().isEmpty()) {						// Если ячейка не пустая
				item->setText(ERR_STR_NOT_IN_RANGE);			// Выводим сообщение об ошибке
			}
			break;												// Выходим из case
		}

		if (newStartChannel >= m_maxChannelsCount || newStartChannel < 0) {		// Если указанный номер канала больше допустимого или меньше 0. Каланы начинаются с 1 для пользователя!
			item->setText(ERR_STR_NOT_IN_RANGE);				// Выводим сообщение об ошибке
			break;
		}

		// Проверяем значение количества каналов
		QStandardItem* itemCount = currModel->item(currentRow, COLOMN_COUNT);		// Получаем ячейку с количеством каналов
				
		if (!itemCount->text().isEmpty() && itemCount->text() != ERR_STR_NOT_IN_RANGE) {	// Если в ячейке с количеством каналов не пусто и не ошибка
			int channelCount = itemCount->text().toInt();									// Получаем кол-во каналов из соседней ячейки
			
			if (newStartChannel + channelCount > m_maxChannelsCount) {						// Проверяем с максимумом для текущего типа
				item->setText(ERR_STR_NOT_IN_RANGE);										// Выводим сообщение об ошибке
				MsgBox::warning("Суммарное количество каналов в данном ведомом превышено!\nМаксимум каналов: " + QString::number(m_maxChannelsCount));
			}
			else {	// Если суммарное количество каналов не превышено, ставим флаг, что есть изменения и дальшезаписываем					
				wasChanges = true;														// Если прошли все проверки, значит изменения внеслись и в ячейке валидное число					
			}
		}

		break;
	}

	case COLOMN_COUNT: {
		bool isInt;											// Для проверки, является ли значение числом
		int newChannelCount = item->text().toInt(&isInt);			// Сохраняем числовое значение
		
		if (!isInt) {											// Если не число
			if (!item->text().isEmpty()) {
				item->setText(ERR_STR_NOT_IN_RANGE);			// Выводим сообщение об ошибке
			}
		}

		if (newChannelCount > m_maxChannelsCount) {		// Если указанный номер канала больше допустимого
			item->setText(ERR_STR_NOT_IN_RANGE);		// Выводим сообщение об ошибке
			break;
		}		

		// Проверяем значение количества
		QStandardItem* itemNumber = currModel->item(currentRow, COLOMN_CHANNEL);
		if (!itemNumber->text().isEmpty() && itemNumber->text() != ERR_STR_NOT_IN_RANGE) {	// Если в ячейке с количеством каналов не пусто и не ошибка
			int startChannel = itemNumber->text().toInt() - 1;
			
			if (startChannel + newChannelCount > m_maxChannelsCount) {
				item->setText(ERR_STR_NOT_IN_RANGE);		// Выводим сообщение об ошибке
				MsgBox::warning("Суммарное количество каналов превышено!\nМаксимум каналов: " + QString::number(m_maxChannelsCount));
			}
			else {
				wasChanges = true;		// Если прошли все проверки, значит изменения внеслись и в ячейке валидное число
			}
		}

		break;
	}
	}	// Конец switch


	if (wasChanges) {
		UpdateFlags(channelType, currModel);										// Обновим все данные в шаблоне МК. Обнулим и перепишем заново из таблицы
		
		bool totalAmountNotExceeded = isTotalAmountOfChannelsExceeded(channelType);	// Проверяем по всем слейвам общее кол-во каналов

		if (totalAmountNotExceeded) {												// Если превышена сумма по все каналам
			// Если превышено мак кол-во канало данного типа
			item->setText(ERR_STR_NOT_IN_RANGE);										// Выводим сообщение об ошибке
			MsgBox::warning("Сумма каналов со всех ведомых превышена!\nМаксимум каналов: " + QString::number(vecMaxChannelCountOfType[channelType]));
			UpdateFlags(channelType, currModel);										// Обновим все данные в шаблоне МК. Обнулим и перепишем заново из таблицы
		}
	}


	currModel->blockSignals(false);		// Обязательно не забыть! Но это должно быть перед редактированием модели, иначе строки не добавятся!

	// Добавление новой строки
	int currentRowCount = currModel->rowCount();
	bool isLastRow = (currentRow == (currentRowCount - 1));
	bool newValueNotEmpty = !item->text().isEmpty();
	bool valueIsValid = (currentColom != COLOMN_NAME) && (item->text() != ERR_STR_NOT_IN_RANGE);

	if (isLastRow && valueIsValid && newValueNotEmpty) {		// Если изменяемая строчка строчка последняя, не имя и не пуста

		if (currentRowCount <= m_maxChannelsCount) {				// Пока кол-во строк умещается в допустимый максимум 			
			addEmptyRowToModel(currModel);							// Добавляем пустую строчку
		}
	}

}


void TabSybusMasterChannels::slotClickedDeleteRowButton(bool) {
	
	QTableView *tabViewChannels;		// Промежуточный варинт. В нем хранится модель
	tabViewChannels = qobject_cast<QTableView *> (toolBoxChannels->currentWidget());	// Преобразуем в тип

	CHECK_ADDRESS_SHOW_MESSAGE(tabViewChannels, "Нет данных");

	QStandardItemModel* model = qobject_cast<QStandardItemModel *>(tabViewChannels->model());
	   
	if (model->rowCount() <= 1) {
		return;
	}

	for (int row = model->rowCount() - 1; row >= 0; row--) {

		if (model->item(row, COLOMN_NAME)->checkState() == Qt::Checked) {
			removeRowFromTable(model, row);
		}
	}

	UpdateFlags(getCurrentChannelType(), model);
	buttonChooseAll->setChecked(false);			// Снимаем кнопку

}

void TabSybusMasterChannels::slotClickedAddRowButton(bool) {
	
	QTableView *tabViewChannels;		// Промежуточный варинт. В нем хранится модель
	tabViewChannels = qobject_cast<QTableView *> (toolBoxChannels->currentWidget());	// Преобразуем в тип

	CHECK_ADDRESS_SHOW_MESSAGE(tabViewChannels, "Нет данных");

	QStandardItemModel* model = qobject_cast<QStandardItemModel *>(tabViewChannels->model());
	addEmptyRowToModel(model);

}

void TabSybusMasterChannels::slotClickedChooseAllButton(bool state) {

	QTableView *tabViewChannels = nullptr;		// Промежуточный варинт. В нем хранится модель
	tabViewChannels = qobject_cast<QTableView *> (toolBoxChannels->currentWidget());	// Преобразуем в тип

	CHECK_ADDRESS_SHOW_MESSAGE(tabViewChannels, "Нет данных");

	QStandardItemModel* model = qobject_cast<QStandardItemModel *>(tabViewChannels->model());

	CHECK_ADDRESS_SHOW_MESSAGE(model, "Невозможно получить модель данных из таблицы");
	
	for (int row = 0; row < model->rowCount(); row++) {

		if (state == true) {
			model->item(row, COLOMN_NAME)->setCheckState(Qt::Checked);
		}
		else {
			model->item(row, COLOMN_NAME)->setCheckState(Qt::Unchecked);
		}
	}
}


