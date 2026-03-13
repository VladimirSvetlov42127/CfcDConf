#include "ModbusWidgetSlaveSettings.h"

#include <qdebug.h>
#include <qcoreapplication.h>
#include <qfiledialog.h>
#include <qevent.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandWidgetHeaders.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ModbusFunctionsHeaders.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_FunctionWidgetHeaders.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/DeviceCommandLibrary/CmdLibWidget.h>
#include <gui/forms/interfaces_protocols/ConnectionDiscretWidget.h>

using namespace Dpc::Gui;

static const QString gErrorValue("Неверное значение");

ModbusWidgetSlaveSettings::ModbusWidgetSlaveSettings(uint16_t portNumber, DcController* controller, uint8_t slaveCount, bool isModifyModbus, QWidget *parent)
	: QWidget(parent),
    m_isModifyModbus(isModifyModbus),
    m_controller(controller),
    m_portNumber(portNumber),
    m_slaveSaver(controller, portNumber, this),
    m_tableWidget(nullptr),
    m_currentCommand(nullptr),
    m_currentCommandWidget(nullptr),
    m_currentFunction(nullptr),
    m_currentFunctionWidget(nullptr),
    m_initCommandWidget(nullptr),
    m_PortInfo(controller),
	m_connectionDiscret(new QWidget)
{
	for (int i = 0; i < slaveCount; i++) {
		mv_slaves.append(spSlave_t(new ModbusSlaveDefinition(isModifyModbus)));	// Создаем заново
	}

	commonSetup();
}


ModbusWidgetSlaveSettings::ModbusWidgetSlaveSettings(uint16_t portNumber, DcController* controller, QVector<spSlave_t> &slavesVector, bool isModifyModbus, QWidget *parent)
    : QWidget(parent),
      m_isModifyModbus(isModifyModbus),
      m_portNumber(portNumber),
      mv_slaves(slavesVector),
      m_slaveSaver(controller, portNumber, this),
      m_tableWidget(nullptr),
      m_currentCommand(nullptr),
      m_currentCommandWidget(nullptr),
      m_currentFunction(nullptr),
      m_currentFunctionWidget(nullptr),
      m_initCommandWidget(nullptr),
      m_PortInfo(controller),
      m_connectionDiscret(new QWidget)
{
    commonSetup();
}

ModbusWidgetSlaveSettings::~ModbusWidgetSlaveSettings()
{
}

void ModbusWidgetSlaveSettings::commonSetup()
{
	ui.setupUi(this);
	m_currentSlave = nullptr;
	
	ui.comBoxSlaveID->blockSignals(true);	// Заполняем комбобокс с отклченными сигналами, чтобы не работал слот
	for (int i = 0; i < mv_slaves.size(); i++) {
		QString slaveName = "Ведомый " + QString::number(i + 1);
		ui.comBoxSlaveID->addItem(slaveName);
	}
	ui.comBoxSlaveID->blockSignals(false);

	m_tableWidget = new ModbusWidgetChannelsTable(m_isModifyModbus);	// Вспомогательный виджет с таблицами
	ui.hLayoutForTable->addWidget(m_tableWidget);
	m_tableWidget->setVisible(false);	// Изначально не виден, т.к. пустой

	m_spaser = new QSpacerItem(1, 1, QSizePolicy::Expanding);	// Спейсер будет добавлять/удалять для позиционирования постоянной формы слева
	ui.hLayoutForTable->addSpacerItem(m_spaser);
	
	// Связи от текущего виджета к таблицам (включает/выклачает вкладки)
	bool ok = connect(this, SIGNAL(signal_addChannelToTable(ModbusMaster_ns::channelType_t)), m_tableWidget, SLOT(slot_addTable(ModbusMaster_ns::channelType_t)));
	Q_ASSERT(ok);
	ok = connect(this, SIGNAL(signal_removeAllChannelsFromTable(ModbusMaster_ns::channelType_t)), m_tableWidget, SLOT(slot_removeTable(ModbusMaster_ns::channelType_t)));
	Q_ASSERT(ok);

	// Связи от таблицы к созданию команды
	ok = connect(m_tableWidget, SIGNAL(signal_updateChannelRequest(const ModbusMaster_ns::structCommand_t &)), SLOT(slot_updateCommand(const ModbusMaster_ns::structCommand_t &)));
	Q_ASSERT(ok);
	ok = connect(m_tableWidget, SIGNAL(signal_eraseChannelRequest(const ModbusMaster_ns::structCommand_t &)), SLOT(slot_eraseCommand(const ModbusMaster_ns::structCommand_t &)));
	Q_ASSERT(ok);
	ok = connect(this, &ModbusWidgetSlaveSettings::signal_removeCommand, m_tableWidget, &ModbusWidgetChannelsTable::slot_removeCommand);
	Q_ASSERT(ok);

	// Все чекбоксы коннектим к одному слоту
	ok = connect(ui.cBoxAins, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxDins, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxCins, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxRAouts, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxRDouts, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxWAouts, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.cBoxWDouts, SIGNAL(stateChanged(int)), SLOT(slot_chBoxTypeChanged(int))); 
	Q_ASSERT(ok);

	ok = connect(this, SIGNAL(signal_commandExist(const ModbusMaster_ns::structCommand_t &)), m_tableWidget, SLOT(slot_addExistingCommand(const ModbusMaster_ns::structCommand_t &)));
	Q_ASSERT(ok);

	ui.formLayout->insertRow(0, "Дискрет связи", m_connectionDiscret);	

	on_comBoxSlaveID_currentIndexChanged(0);	// Устанавливаем первый слейв

	if (m_currentSlave) {
		m_initCommandWidget = new ModbusWidgetInitCommands(m_currentSlave->getInitCommandsVector());
		ui.tabSlaveDescription->addTab(m_initCommandWidget, "Команды инициализации");	// Добавляем новую вкладку
	}
		
}

void ModbusWidgetSlaveSettings::clearAllForms()
{
	ui.lEdAddress->clear();
	ui.lEdTimeout->clear();

	blockTypeCheckBoxes(true);	// Блокируем, чтобы не удалить в последнем слейве
	ui.cBoxAins->setChecked(false); 
	ui.cBoxDins->setChecked(false);
	ui.cBoxCins->setChecked(false);
	ui.cBoxRAouts->setChecked(false);
	ui.cBoxRDouts->setChecked(false);
	ui.cBoxWAouts->setChecked(false);
	ui.cBoxWDouts->setChecked(false);
	blockTypeCheckBoxes(false);

	if (m_tableWidget) {
		m_tableWidget->clearAllForms();
		if (ui.hLayoutForTable->count() == 1) {	// Если стала пустой. isEmpty() здесь всегда false!			
			ui.hLayoutForTable->addSpacerItem(m_spaser);
		}
	}
	
	ui.comBoxCommand->clear();	// Удаляем здесь, а не в clearCommandWidget

	clearCommandWidget();
}

void ModbusWidgetSlaveSettings::clearCommandWidget()
{
	if (m_currentCommandWidget) {	// Если был установлен какой-то
		ui.hLayoutFromGroupBoxCommand->removeWidget(m_currentCommandWidget);
		delete m_currentCommandWidget;
		m_currentCommandWidget = nullptr;
	}

	ui.comBoxModbusFunction->clear();

	if (m_currentFunctionWidget) {	// Удаляет виджет из описания модбас команды
		ui.vLayoutModbusFunction->removeWidget(m_currentFunctionWidget);
		delete m_currentFunctionWidget;
		m_currentFunctionWidget = nullptr;
	}	
}

bool ModbusWidgetSlaveSettings::fillFormFromCurrentSlave()
{
	if (m_currentSlave == nullptr)
		return false;

	ui.lEdAddress->setText(QString::number(m_currentSlave->getAddress()));
	ui.lEdTimeout->setText(QString::number(m_currentSlave->getTimeout()));
	QString filename = m_currentSlave->getFileName();
	QString fileNameWithoulMSL = filename.left(filename.lastIndexOf('.'));
	ui.lEditFilename->setText(fileNameWithoulMSL);

	ui.comBoxCRC->blockSignals(true);
	if (m_currentSlave->isCRCBitSet())
		ui.comBoxCRC->setCurrentIndex(1);	// По умолчанию ставится ноль
	else
		ui.comBoxCRC->setCurrentIndex(0);
	ui.comBoxCRC->blockSignals(false);

	const QHash<key_t, spCommandDescr_t> commandsHash(m_currentSlave->getCommandsHash());
	QHash<key_t, spCommandDescr_t>::const_iterator iter;

	for(ModbusMaster_ns::channelType_t type: ModbusMaster_ns::qv_channelTypes)
	{
		ModbusMaster_ns::channelType_t searhType;
		switch (type)
		{
		case ModbusMaster_ns::AinsInd: 
			searhType = ModbusMaster_ns::AinsInd; 
			break;				
		case ModbusMaster_ns::DinsInd: 
			searhType = ModbusMaster_ns::DinsInd;
			break;
		case ModbusMaster_ns::CinsInd:
			searhType = ModbusMaster_ns::CinsInd;
			break;
		case ModbusMaster_ns::RAoutsInd:
			searhType = ModbusMaster_ns::RAoutsInd;
			break;
		case ModbusMaster_ns::RDoutsInd:
			searhType = ModbusMaster_ns::RDoutsInd;
			break;
		case ModbusMaster_ns::WAoutsInd:
			searhType = ModbusMaster_ns::WAoutsInd;
			break;
		case ModbusMaster_ns::WDoutsInd:
			searhType = ModbusMaster_ns::WDoutsInd;
			break;
		default:
			break;
		}

		key_t key(searhType, 0);	// Создаем ключ с выбранным типом
		iter = commandsHash.find(key);	// Ищем 
		while (iter != commandsHash.end()) {
			createExistingCommand(iter.value(), key);	// Создаем существующую команду
			key.second++;
			iter = commandsHash.find(key);
		}
	}
	
	return true;
}

void ModbusWidgetSlaveSettings::createExistingCommand(spCommandDescr_t command, key_t key) {
	if (!command)
		return;

	setCheckBoxTypeState(command->getCommandType(), true);	// Включаем checkbox; // Он должен по слоту добавить новую таблицу

	ModbusMaster_ns::structCommand_t structForSend;
	structForSend.type = command->getCommandType();
	structForSend.index = key.second;
	structForSend.startChannel = command->getStartAddress();
	structForSend.count = command->getCount();

	QString commandName(command->getCommandName());
	if (!commandName.isEmpty())
		structForSend.name = commandName;
	else {
		structForSend.name = makeDefaultCommandName(key.second + 1, structForSend.type, structForSend.count, structForSend.startChannel, m_isModifyModbus);
		command->setCommandName(structForSend.name);
	}
	
	emit signal_commandExist(structForSend);	// Посылаем сигнал к таблице, чтобы добавить

	addToCommandCombobox(key, structForSend.name);
}

// Работа с виджетом таблиц для создания команд по считыванию каналов
void ModbusWidgetSlaveSettings::addChannelToTable(ModbusMaster_ns::channelType_t type)
{
	ui.hLayoutForTable->removeItem(m_spaser);	// Удаляем спейсер
	emit signal_addChannelToTable(type);
}

void ModbusWidgetSlaveSettings::removeAllChannels(ModbusMaster_ns::channelType_t type)
{
	// Последовательность важна!
	emit signal_removeAllChannelsFromTable(type);	// Удаляем из таблицы
	if (ui.hLayoutForTable->isEmpty()) {	// Если стала пустой
		ui.hLayoutForTable->addSpacerItem(m_spaser);
	}

	removeFromCommandCombobox(type);

	m_currentSlave->removeAllTypeCommands(type);
	m_currentSlave->setTypeCount(type, 0);	// Обнуляем количество каналов
	
}

void ModbusWidgetSlaveSettings::setCheckBoxTypeState(ModbusMaster_ns::channelType_t type, bool state)
{
	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		ui.cBoxAins->setChecked(state);
		break;
	case ModbusMaster_ns::DinsInd:
		ui.cBoxDins->setChecked(state);
		break;
	case ModbusMaster_ns::CinsInd:
		ui.cBoxCins->setChecked(state);
		break;
	case ModbusMaster_ns::RAoutsInd:
		ui.cBoxRAouts->setChecked(state);
		break;
	case ModbusMaster_ns::RDoutsInd:
		ui.cBoxRDouts->setChecked(state);
		break;
	case ModbusMaster_ns::WAoutsInd:
		ui.cBoxWAouts->setChecked(state);
		break;
	case ModbusMaster_ns::WDoutsInd:
		ui.cBoxWDouts->setChecked(state);
		break;
	default:
		break;
	}
}

// Работа с командой
bool ModbusWidgetSlaveSettings::fillCommand(spCommandDescr_t command, const ModbusMaster_ns::structCommand_t & info)
{
	if(!command) {
		MsgBox::error("Нет команды для заполнения!");
		return false;
	}

	if (!command->setStartAddress(info.startChannel)) {
		MsgBox::error("Невозможно установить новый адрес в слейве!");
		return false;
	}

	if (!command->setCount(info.count)) {
		MsgBox::error("Невозможно установить новое количество каналов в слейве!");
		return false;
	}

	if (!command->setCommandName(info.name)) {
		MsgBox::error("Невозможно установить новое имя команды в слейве!");
		return false;
	}

	return true;
}

int ModbusWidgetSlaveSettings::getIndexForNewCommand(key_t key)
{
	int pos = ui.comBoxCommand->count();

	for (int i = 0; i < ui.comBoxCommand->count(); i++) { // Проходим по всем элементам комбобокса
		key_t commandKey((ui.comBoxCommand->itemData(i).value<key_t>()));	// Берем из элемента данные, хранимые в QVariant, преобразуем к ключу
		if (key.first < commandKey.first) {	// Проверяем типы
			pos = i;
			break;
		}
		if (key.first == commandKey.first) {
			if (key.second < commandKey.second) {	// Проверяем индексы в конкретном типе
				pos = i;
				break;
			}
		}
	}

	return pos;
}

void ModbusWidgetSlaveSettings::createNewCommandWidget(spCommandDescr_t command)
{
	if (!command)
		return;

	if (m_currentCommandWidget) {
		ui.hLayoutFromGroupBoxCommand->removeWidget(m_currentCommandWidget);
		delete m_currentCommandWidget;
		m_currentCommandWidget = nullptr;
	}

	ModbusMaster_ns::channelType_t type(command->getCommandType());

	switch (type)
	{
	case ModbusMaster_ns::AinsInd:
		m_currentCommandWidget = new ModbusWidgetCommandAins(command, m_isModifyModbus);
		break;
	case ModbusMaster_ns::DinsInd: 
		m_currentCommandWidget = new ModbusWidgetCommandDins(command, m_isModifyModbus);		
		break;	
	case ModbusMaster_ns::CinsInd:
		m_currentCommandWidget = new ModbusWidgetCommandCins(command, m_isModifyModbus);
		break;

	case ModbusMaster_ns::RAoutsInd:
		break;
	case ModbusMaster_ns::RDoutsInd:
		break;
	case ModbusMaster_ns::WAoutsInd:
		break;
	case ModbusMaster_ns::WDoutsInd:
		break;
	default:

		break;
	}

	if(m_currentCommandWidget)
		ui.hLayoutFromGroupBoxCommand->insertWidget(0, m_currentCommandWidget);
}

bool ModbusWidgetSlaveSettings::isNewTypeCountValid(const ModbusMaster_ns::structCommand_t &command)
{
	//int64_t currCount = m_PortInfo.getCurrentTypeCountForAllPorts(command.type);
	//int64_t maxCout = m_PortInfo.getMaxTypeCountForAllPorts(command.type);
	//if (currCount >= 0 && maxCout >= 0) {
	//	if (currCount + command.count > maxCout) {
	//		uint64_t curCount = m_currentSlave->getTypeCount(command.type);
	//		MsgBox::error("Превышено допустимое количество удаленных переменных суммарно по всем слейвам. Команда не сохранена.\nМаксимальное количество удаленных сигналов: " + QString::number(maxCout)
	//			+ "\nОбщее количество сигналов с добавлением последней команды: " + QString::number(currCount + command.count) +
	//			"\nКоличество сигналов в текущем слейве с новой командой: " + QString::number(curCount + command.count));
	//		//emit signal_removeCommand(command);
	//		return false;
	//	}
	//}
	//else {
	//	static bool dontShow = false;	// Показывать или нет
	//	if(!dontShow)
	//		dontShow = MsgBox::question("ВНИМАНИЕ!\nНевозможно рассчитать максимальное количество внешних сигналов. Проверка не работает. Обновите устройство.\nСкрывать данное сообщение в рамках текущей сессии?");
	//}

	return true;
}

bool ModbusWidgetSlaveSettings::createFunctionForCurrentCommand(ModbusFunctions_ns::modbusFunctionType_t type)
{
	if (!m_currentCommand)
		return false;

	if (m_initFunction && m_currentCommand->getModbusFunction()) {// Проверяем, была ли настроена функция в команде уже
		m_currentFunction = m_currentCommand->getModbusFunction();	// Загружаем старую
		m_initFunction = false;
	}
	else {	// Или создаем пустую
		m_currentFunction = createFunction(type, m_currentCommand->getStartAddress(), m_currentCommand->getCount());
		if (!m_currentFunction)
			return false;
		m_currentCommand->setModbusFunction(m_currentFunction);	// Устанавливаем функцию в слейв
	}

	m_currentCommandWidget->updateFromFunctionInfo(m_currentFunction);	// Обновляем виджет для команды

	if (m_currentFunctionWidget) {
		ui.vLayoutModbusFunction->removeWidget(m_currentFunctionWidget);
		delete m_currentFunctionWidget;
		m_currentFunctionWidget = nullptr;
	}

	m_currentFunctionWidget = createNewFunctionWidget(m_currentFunction);	// Фабрика дает виджет по типу
	if (!m_currentFunctionWidget)
		return false;
	
	ui.vLayoutModbusFunction->addWidget(m_currentFunctionWidget);
	return true;
}

spModbusFunc_t ModbusWidgetSlaveSettings::createFunction(ModbusFunctions_ns::modbusFunctionType_t type, ModbusMaster_ns::startAddress_t startChannel, ModbusMaster_ns::count_t count)
{
	spModbusFunc_t function(nullptr);

	switch (type)
	{
	case ModbusFunctions_ns::Unknown:
		break;
	case ModbusFunctions_ns::ReadDiscreteInputs:
		function = spModbusFunc_t(new ModbusFunction02_ReadDiscreteInputs(startChannel, count));
		break;
	case ModbusFunctions_ns::ReadInputRegisters:
		function = spModbusFunc_t(new ModbusFunction04_ReadInputRegisters(startChannel, count));
		break;
		
	default:
		function = spModbusFunc_t(new ModbusFunctionCustom(startChannel, count));
		break;
	}

	return function;
}

IModbusWidgetFunction * ModbusWidgetSlaveSettings::createNewFunctionWidget(spModbusFunc_t function)
{
	IModbusWidgetFunction *functionWidget(nullptr);
	ModbusFunctions_ns::modbusFunctionType_t code(function->getFunctionType());	// Получаем код функции.
	ModbusFunctions_ns::modbusFunctionType_t notStandartCode(code); // Сохраем код, если она не является стандартной
	if (!m_currentCommand->isSupportedFunctionCode(code)) {	// Если нестандартная меняем код для выбора виджета
		code = ModbusFunctions_ns::Custom;
	}

	switch (code)
	{
	case ModbusFunctions_ns::Unknown:
		break;
	case ModbusFunctions_ns::ReadDiscreteInputs: {
		ModbusFunction02_ReadDiscreteInputs *func = dynamic_cast<ModbusFunction02_ReadDiscreteInputs*>(function.data());
		if (!func) {
			MsgBox::error("Невозможно преобразовать функцию к нужному типу для получения виджета");
			break;
		}
		functionWidget = new ModbusWidgetFunction02_ReadDiscreteInputs(func);
		break;
	}
	case ModbusFunctions_ns::ReadInputRegisters: {
		ModbusFunction04_ReadInputRegisters *func = dynamic_cast<ModbusFunction04_ReadInputRegisters*>(function.data());
		if (!func) {
			MsgBox::error("Невозможно преобразовать функцию к нужному типу для получения виджета");
			break;
		}
		functionWidget = new ModbusWidgetFunction04_ReadInputRegisters(func);
		break;
	}

	case ModbusFunctions_ns::Custom: {
		ModbusFunctionCustom* func = dynamic_cast<ModbusFunctionCustom*>(function.data());
		if (!func) {
			MsgBox::error("Невозможно преобразовать функцию к нужному типу для получения виджета");
			break;
		}
		functionWidget = new ModbusWidgetFunction_Custom(func);
		break;
	}
	}

	return functionWidget;
}

// Работа с виджетами
void ModbusWidgetSlaveSettings::blockTypeCheckBoxes(bool needBlock)
{
	ui.cBoxAins->blockSignals(needBlock);
	ui.cBoxDins->blockSignals(needBlock);
	ui.cBoxCins->blockSignals(needBlock);
	ui.cBoxRAouts->blockSignals(needBlock);
	ui.cBoxRDouts->blockSignals(needBlock);
	ui.cBoxWAouts->blockSignals(needBlock);
	ui.cBoxWDouts->blockSignals(needBlock);
}

bool ModbusWidgetSlaveSettings::addToCommandCombobox(key_t key, const QString & name)
{
	if (name.isEmpty())
		return false;
	
	bool firstEntry = ui.comBoxCommand->count() == 0;	// Проверяем. Если список команд пустой, то вызывает слот обновления команды, иначе блокируем сигналы
	if (!firstEntry) 
		ui.comBoxCommand->blockSignals(true);	// Текущий индекс не изменяется

	int pos(getIndexForNewCommand(key));	// Заполняем комбобокс
	QVariant variant = QVariant::fromValue(key);
	ui.comBoxCommand->insertItem(pos, name, variant);	// Вставляем на нужную позицию
	
	if (!firstEntry)
		ui.comBoxCommand->blockSignals(false);

	return true;
}

bool ModbusWidgetSlaveSettings::removeFromCommandCombobox(key_t key)
{
	int size = ui.comBoxCommand->count();
	for (int i = 0; i < size; i++) {
		key_t k = ui.comBoxCommand->itemData(i).value<key_t>(); 

		if (k == key) {
			ui.comBoxCommand->removeItem(i);
			return true;
		}			
	}

	/*QVariant variant;
	variant.setValue<key_t>(key);
	int index = ui.comBoxCommand->findData(variant);		// Почему-то не работает поиск с командами из библиотеки!!!
	if (index < 0) {
		return false;
	}

	ui.comBoxCommand->removeItem(index);*/
	return false;
}

bool ModbusWidgetSlaveSettings::removeFromCommandCombobox(channelType_t type)
{
	clearCommandWidget();	

	// Удаляем из комбобокса все команды данного типа
	for (int i = ui.comBoxCommand->count(); i >= 0; i--) {
		QVariant var(ui.comBoxCommand->itemData(i));
		if (var.value<key_t>().first == type)
			ui.comBoxCommand->removeItem(i);
	}

	return true;
}

void ModbusWidgetSlaveSettings::closeEvent(QCloseEvent * event)
{
	if (MsgBox::question("Вы действительно хотите выйти? \nНесохраненные настройки слейва будут утеряны!")) {
		QWidget::closeEvent(event);
	}
	else
		event->ignore();
}


// Slots
void ModbusWidgetSlaveSettings::on_comBoxSlaveID_currentIndexChanged(int index) {
	if (index >= mv_slaves.size() || index < 0)
		return;

	m_currentSlave = mv_slaves[index];		

	clearAllForms();
	fillFormFromCurrentSlave();
	
	QWidget *newConnectionDiscretWidget = new ConnectionDiscretWidget(m_controller, m_portNumber, index, QString("Модбас Слейв(%1:%2)").arg(m_portNumber).arg(index), this);
	ui.formLayout->replaceWidget(m_connectionDiscret, newConnectionDiscretWidget);
	m_connectionDiscret->deleteLater();
	m_connectionDiscret = newConnectionDiscretWidget;

	int oldIndex = ui.tabSlaveDescription->currentIndex();

	QWidget *delWid = ui.tabSlaveDescription->widget(INITIALIZATION_TAB_INDEX);
	if (delWid) {
		delWid->setParent(nullptr);
		delete delWid;	// Это m_initCommandWidget
		ui.tabSlaveDescription->removeTab(INITIALIZATION_TAB_INDEX);

		m_initCommandWidget = new ModbusWidgetInitCommands(m_currentSlave->getInitCommandsVector());
		ui.tabSlaveDescription->addTab(m_initCommandWidget, "Команды инициализации");	// Добавляем новую вкладку		
	}

	if (oldIndex <= ui.tabSlaveDescription->count())
		ui.tabSlaveDescription->setCurrentIndex(oldIndex);
	
}

void ModbusWidgetSlaveSettings::on_lEditFilename_textChanged(const QString &text)
{
	QString newName = text;
	QString nameInSlave = m_currentSlave->getFileName();

	if (newName.isEmpty()) {	// Если стерли имя, показывается старое записанное
		ui.lEditFilename->setText(nameInSlave.left(nameInSlave.lastIndexOf('.')));
		return;
	}		

	if (newName.contains(' ')) {
		MsgBox::warning("Проблемы в имени файла не допустимы\nПробелы были заменены на нижнее подчеркивание");
		ui.lEditFilename->setText(newName.replace(' ', '_'));
	}

	if(newName.contains('.')) {
		MsgBox::warning("Использование точек недопустимо. Формат прописывается автоматически.");
		newName.chop(1);
		ui.lEditFilename->setText(newName);
	}
	//QString slaveName (createSuffixName(newName, m_portNumber, ui.comBoxSlaveID->currentIndex()));

	//if (!checkFileNameSize(slaveName)) {
	//	MsgBox::error("Слишком длинное имя! Не вводите рисширение файла и принадлежность к порту или слейву, это добавится автоматически");
	//	return;
	//}

	newName = newName + ".msl";	// Добавляем расширение
	if (!checkFileNameSize(newName)) {
		MsgBox::error("Слишком длинное имя файла!");
		return;
	}


	if (!m_currentSlave->setCustomFileName(newName)) {	// Устанавливаем
		MsgBox::error("Невозможно сохранить имя в слейве");
	}
}


void ModbusWidgetSlaveSettings::on_butSave_clicked() {
	
	QString err;
	if(m_slaveSaver.saveAllSlaveConfig(mv_slaves, err) == ModbusSlaveFileSaver::ALL_IS_OK)	// При удачном сохранении закрываем окно
		this->hide();
	else {
		if (!err.isEmpty())
			MsgBox::error(err);
		else
			MsgBox::error("Настройки не сохранены");
	}
}

void ModbusWidgetSlaveSettings::on_butCreateFromLibrary_clicked()
{
	static CmdLibWidget lib;

	// Сигналы с дополнительным флагом, повторно не соединяются
	connect(&lib, SIGNAL(signal_addCommandsFromLibrary(QList<spCommandDescr_t>)), this, SLOT(slot_addCommandsFromLibrary(QList<spCommandDescr_t>)), Qt::UniqueConnection);
	connect(&lib, SIGNAL(signal_addInitCommandsFromLibrary(QList<sp_InitCommand>)), this, SLOT(slot_addInitCommandsFromLibrary(QList<sp_InitCommand>)), Qt::UniqueConnection);
		
	lib.exec();
}

void ModbusWidgetSlaveSettings::on_comBoxCRC_currentIndexChanged(int index)
{
	m_currentSlave->setModbusModeCRCBit(index);	// Преобразование типа в bool
}

void ModbusWidgetSlaveSettings::on_butLoadSlaveFromFile_clicked()
{
	if (!MsgBox::question("Вы уверены, что хотите загрузить в текущий слейв новые параметры? \nСтарые файлы будут утеряны!"))
		return;
	   
	static QString openDir = QDir::currentPath();
	QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл с настройками слейва", openDir, "Бинарные (*.msl)");
	if (filePath.isNull() || filePath.isEmpty())
		return;

	int lastSlashPosition = filePath.lastIndexOf('/');
	openDir = filePath.left(lastSlashPosition);	// Сохраняем выбранную директорию
	
	
	// Удаляем старый файл
	if (m_currentSlave->isModbusModeSaveInFile()) {	// Если был записан в файле
		QString filename = m_currentSlave->getFileName();
		QString filePath = getFilePathForLocalSave(m_controller, filename);	// Получаем путь к старому файлу,
		QFile file(filePath);
		if (file.exists())
			if (!file.remove()) {
				MsgBox::error("Ошибка удаления старого файла настроек слейва");
				return;
			}
	}

	int slaveIndex = ui.comBoxSlaveID->currentIndex();
	spSlave_t newSlave = m_slaveSaver.readSlaveConfigFromFile(filePath, slaveIndex);
	if (!newSlave) {
		MsgBox::error("Невозможно распознать информацию из файла");
		return;
	}

	//if (fileName.contains(' ')) {
	//	MsgBox::warning("Использование пробелов в имени файла конфигурации недопустимо. \nПробелы будут заменены на нижнее подчеркивание");

	//	fileName = fileName.replace((' '), '_');		
	//	newSlave->setFileName(fileName);
	//}

	QString fileName;
	//fileName = filePath.right(filePath.size() - lastSlashPosition - 1);	// Получаем только имя файла
	//if (fileName.size() > getMaxFileNameByteSize()) {	// Есть 
	//	MsgBox::error("Невозможно сохранить имя файла. Слишком большое");
	//	return;
	//}
	fileName = "Slave_" + QString::number(m_portNumber * m_PortInfo.getSlaveCountForEachPort() + ui.comBoxSlaveID->currentIndex()) + ".msl";

	newSlave->setFileName(fileName);	// Записываем имя файла

	m_currentSlave = newSlave;	// Обновляем слейв
	mv_slaves[slaveIndex] = m_currentSlave;
	on_comBoxSlaveID_currentIndexChanged(slaveIndex);	// Обновляем информацию в виджете
}

void ModbusWidgetSlaveSettings::on_butSaveToFile_clicked()
{
	static QString saveDir = QDir::currentPath();

	QString filename = QFileDialog::getSaveFileName(nullptr, "Выберите файл с настройками слейва", saveDir, "Бинарные (*.msl)");
	if (filename.isNull() || filename.isEmpty())
		return;

	saveDir = filename.left(filename.lastIndexOf('/'));	// Сохраняем последнюю директорию

	if (!m_slaveSaver.writeSlaveConfigToFile(m_currentSlave, filename)) {
		MsgBox::error("Оибка сохранения конфигурации в файл");
		return;
	}
}

void ModbusWidgetSlaveSettings::on_lEdTimeout_editingFinished() {
	bool ok;
	uint16_t val = ui.lEdTimeout->text().toUShort(&ok);
	if (ok && (val < 0xff)) {
		m_currentSlave->setTimeout(val);
	}
	else {
		ui.lEdTimeout->setText(gErrorValue);
	}
}

void ModbusWidgetSlaveSettings::on_lEdAddress_editingFinished() {
	bool ok;
	uint16_t val = ui.lEdAddress->text().toUShort(&ok);
	if (ok && (val < 0xff)) {
		m_currentSlave->setAddress(val);
	}
	else {
		ui.lEdAddress->setText(gErrorValue);
	}
}

void ModbusWidgetSlaveSettings::slot_chBoxTypeChanged(int state) {
	ModbusMaster_ns::channelType_t type;

	if (sender() == ui.cBoxAins)
		type = ModbusMaster_ns::AinsInd;
	else if (sender() == ui.cBoxDins)
		type = ModbusMaster_ns::DinsInd;
	else if (sender() == ui.cBoxCins)
		type = ModbusMaster_ns::CinsInd;
	else if (sender() == ui.cBoxRAouts)
		type = ModbusMaster_ns::RAoutsInd;
	else if (sender() == ui.cBoxRDouts)
		type = ModbusMaster_ns::RDoutsInd;
	else if (sender() == ui.cBoxWAouts)
		type = ModbusMaster_ns::WAoutsInd;
	else if (sender() == ui.cBoxWDouts)
		type = ModbusMaster_ns::WDoutsInd;
	else
		return;

	if (state == Qt::Checked) {	// Если поставили галочку
		addChannelToTable(type);
	}
	else {
		removeAllChannels(type);
	}
}

void ModbusWidgetSlaveSettings::on_comBoxCommand_currentIndexChanged(int index) {
	clearCommandWidget();
	m_initFunction = true;

	if (ui.comBoxCommand->count() <= 0 || index < 0)	// Если комбобокс пустой, то форма пустая остается
		return;

	QVariant data(ui.comBoxCommand->itemData(index));	// Получаем из комбобокса QVariant
	key_t key(data.value<key_t>());	 // Получаем ключ

	m_currentCommand = m_currentSlave->getCommand(key);	// Получаем команду
	if (!m_currentCommand) {
		MsgBox::error("Невозможно получить команду из слейва!");
		return;
	}

	createNewCommandWidget(m_currentCommand);	// Размещает виджет в поле "Заголовок описания команды"

	// Получаем список доступных Модбас функций от типа команды
	QVector<ModbusFunctions_ns::modbusFunctionType_t> availableFunc(m_currentCommand->getAvailableModbusFunctionTypes());
	if (availableFunc.size() != 0) {	// Если есть достпупные функции
	
		ui.comBoxModbusFunction->blockSignals(true);
		for (ModbusFunctions_ns::modbusFunctionType_t type : availableFunc) {	// Формируем список доступных функций
			QString funcName = ModbusFunctions_ns::HASH_FUNCTION_NAME_BY_TYPE.value(type);
			ui.comBoxModbusFunction->addItem(funcName, type);	// Добавляем все доступные функции в комбобокс			
		}
		
		m_currentFunction = m_currentCommand->getModbusFunction();	// Получаем функцию из команды

		ModbusFunctions_ns::modbusFunctionType_t type = ModbusFunctions_ns::Unknown;
		if (m_currentFunction) {	// Если есть, ставит комбобокс на это значение
			type = m_currentFunction->getFunctionType();
			if (!m_currentCommand->isSupportedFunctionCode(type))	// Если код не поддерживаем, меняем на пользовательский, чтобы написать правильное имя
				type = ModbusFunctions_ns::Custom;
			ui.comBoxModbusFunction->setCurrentText(ModbusFunctions_ns::HASH_FUNCTION_NAME_BY_TYPE.value(type));
		}
		else {	// Если текущей функции нет, Создаем функцию, по умолчанию на 0-ой позиции			
			type = ui.comBoxModbusFunction->itemData(0).value<ModbusFunctions_ns::modbusFunctionType_t>();	// Преобразуем из QVariant
		}
		
		ui.comBoxModbusFunction->blockSignals(false);

		if (type == ModbusFunctions_ns::Unknown) {
			MsgBox::error("Неопознанный код команды модбаса!");
			return;
		}

		if (!createFunctionForCurrentCommand(type)) {
			MsgBox::error("Невозможно создать новую команду после смены команды");
			return;
		}
	}
}

void ModbusWidgetSlaveSettings::on_comBoxModbusFunction_currentIndexChanged(int index) {	// Выбираем разные функции для команды, доступные в команде

	if (index < 0)
		return;	// Скорее всего, происходит очищение списка

	ModbusFunctions_ns::modbusFunctionType_t type = ui.comBoxModbusFunction->itemData(index).value<ModbusFunctions_ns::modbusFunctionType_t>();
	if (!createFunctionForCurrentCommand(type)) {
		MsgBox::error("Невозможно создать функцию");
	}

}
// Принимающие слоты от таблицы формирования команд
void ModbusWidgetSlaveSettings::slot_updateCommand(const ModbusMaster_ns::structCommand_t &command) {
	
	if (!m_currentSlave) {
		MsgBox::error("Невозможно добавить к команду! Нет текущего слейва!");
		return;
	}



	//// Обновляем поля комадны
	key_t key(command.type, command.index);	// Создаем ключ для поиска по ХЭШу
	spCommandDescr_t commandFromSlave(m_currentSlave->getCommand(key));	// Получаем команду из слейва (если есть)
	   
	if (!commandFromSlave) {	//	Если такой команды еще нет, создаем	
		
		if (!isNewTypeCountValid(command)) {
			emit signal_removeCommand(command);
			return;
		}
			

		spCommandDescr_t newCommand(m_currentSlave->createCommandByType(command.type));	// Создаем новую команду (выбираем потомка от базового класса)
		if (!newCommand) {
			MsgBox::error("Невозможно создать новую команду!");
			return;
		}

		if (!fillCommand(newCommand, command))	// Заполняем команду информацией 
			return;
				
		newCommand->createDefaultModbusFunction();	// Устанавливаем модбас-функцию по умолчанию для конкретной команды свою

		uint16_t typeCount(m_currentSlave->getTypeCount(command.type));	// Узнаем текущее кол-во каналов
		uint16_t newTypeCount = typeCount + command.count;
		if (!m_currentSlave->setTypeCount(command.type, newTypeCount)) {	// Добавляем обновленное количество в данный тип
			MsgBox::error("Ошибка записи количества запрашиваемых сигналов");
			return;
		}

		if (!m_currentSlave->addCommand(key, newCommand)) {	// Добавляем к имеющимся
			MsgBox::error("Невозможно добавить новую команду в слейв!");
			return;
		}

		if (!addToCommandCombobox(key, command.name)) {	// Добавляем в комбобокс команд, ПОСЛЕ добавления в слейв!
			MsgBox::error("Ошибка добавления в комбобокс команд");
			return;
		}

	}
	else {	// Если команда уже в слейве есть, редактируем ее

		uint16_t oldTypeCount = commandFromSlave->getCount();	// Узнаем старое кол-во в команде

		if (!isNewTypeCountValid(command)) {			
			emit signal_removeCommand(command);	// Удаляем команду, в которой новое значение количества
			ModbusMaster_ns::structCommand_t oldState{ command };
			oldState.count = oldTypeCount;
			emit signal_commandExist(oldState);	// Добавляем команду со старым количеством
			return;
		}

		auto oldSlaveCount = m_currentSlave->getTypeCount(command.type);
		if (!m_currentSlave->setTypeCount(command.type, oldSlaveCount - oldTypeCount + command.count)) {	// Добавляем еще количество в данный тип
			MsgBox::error("Ошибка записи количества запрашиваемых сигналов");
			return;
		}

		if (!fillCommand(commandFromSlave, command))	// Редактируем команду
			return;

		for (int i = 0; i < ui.comBoxCommand->count(); i++) {	// Ищем в комбобоксе нужный ключ
			if (key == ui.comBoxCommand->itemData(i).value<key_t>()) {	// Если нашли
				ui.comBoxCommand->setItemText(i, command.name);	// Обновляем имя
				break;
			}
		}

		// Нужно узнать, является ли редактируемая команд текущей отображаемой, потом обновлять виджеты
		if (key == ui.comBoxCommand->currentData().value<key_t>()) {			
			bool currentFunctionIsSupported = commandFromSlave->isSupportedFunctionCode(commandFromSlave->getModbusFunction()->getFunctionType());
			if (m_currentFunctionWidget && currentFunctionIsSupported) {	// Если отображается модбас функция, обновляем данные в ней
				m_currentFunctionWidget->updateFromFunction(commandFromSlave->getModbusFunction());
			}
			if (m_currentCommandWidget)	// При обновлении функции надо пересчитывать кол-во ожидаемых байт в ответе
				m_currentCommandWidget->updateFromFunctionInfo(commandFromSlave->getModbusFunction());	// Обновляем виджет команды
		}
			   
	}
	
}

void ModbusWidgetSlaveSettings::slot_eraseCommand(const ModbusMaster_ns::structCommand_t &command) {
	if (!m_currentSlave) {
		MsgBox::error("Невозможно добавить к команду! Нет текущего слейва!");
		return;
	}

	key_t key(command.type, command.index);	// Создаем ключ для поиска по ХЭШу
	spCommandDescr_t commandFromSlave(m_currentSlave->getCommand(key));	// Получаем команду из слейва (если есть)

	if (!commandFromSlave) {
		MsgBox::error("Команда для удаления не найдена в слейве!");
		return;
	}

	// Удаляем из ХЭШ таблицы из слейва
	if (!m_currentSlave->removeCommand(key)) {
		MsgBox::error("Проблема с удалением команды из слейва!");
		return;
	}

	if (!removeFromCommandCombobox(key)) {
		MsgBox::error("Ошибка удаления команды из комбобокса!");
		return;
	}


	uint16_t typeCount(m_currentSlave->getTypeCount(command.type));	// Узнаем текущее кол-во каналов
	typeCount -= commandFromSlave->getCount();	// Узнаем старое кол-во в команде
	m_currentSlave->setTypeCount(command.type, typeCount);	// Добавляем еще количество в данный тип

	// Проверяем, был эти удаленный элемент последний, и нужно ли обновить ключи, стоящие после него
	const QHash<key_t, spCommandDescr_t> hash(m_currentSlave->getCommandsHash());
	key_t oldKey(command.type, command.index + 1);	// Следующий за удаленным элементов
	key_t newKey(key);	// Позиция для переназначения (вместо удаленного элемента)
	QHash<key_t, spCommandDescr_t>::const_iterator iter(hash.find(oldKey));	// Получаем итератор на следующий после удаленного элемент
	while (iter != hash.end()) {	// Если это не последний элемент, то надо обновить все последующие ключи
		spCommandDescr_t command(iter.value());	// Сохраняем команду
		m_currentSlave->removeCommand(oldKey);	// Удаляем старый ключ
		m_currentSlave->addCommand(newKey, command);

		// Также обновляем данные в комбобоксе!
		int indexInComboBox = ui.comBoxCommand->findData(QVariant::fromValue(oldKey));	// Ищем индекс, который содержит редактируемый ключ
		if (indexInComboBox < 0) {
			MsgBox::error("Ошибка поиска команды в комбобоксе!");
			return;
		}
		ui.comBoxCommand->setItemData(indexInComboBox, QVariant::fromValue<key_t>(newKey));

		oldKey.second++;
		newKey.second++;
		iter = hash.find(oldKey);
	}
}



void ModbusWidgetSlaveSettings::slot_addCommandsFromLibrary(QList<spCommandDescr_t> list) {

	if (list.isEmpty())
		return;

	for (spCommandDescr_t command : list) {
		
		// Рассчитываем новый ключ для команды. Последний занятый + 1
		auto it = m_currentSlave->getCommandsHash().begin();
		ModbusMaster_ns::channelType_t type = command->getCommandType();
		key_t key(type, 0);
		
		while (it != m_currentSlave->getCommandsHash().end()) {
			key.second++;
			it = m_currentSlave->getCommandsHash().find(key);
		}

		// Делаем структуру для посылки в таблицу, отображающие все аналоги
		ModbusMaster_ns::structCommand_t comStruct;
		comStruct.count = command->getCount();
		comStruct.index = key.second;
		comStruct.name = command->getCommandName();
		comStruct.startChannel = command->getStartAddress();
		comStruct.type = command->getCommandType();		

		setCheckBoxTypeState(type, true);	// Включаем чекбокс, если он не включен

		if (!m_currentSlave->addCommand(key, command)) {
			MsgBox::error("Ошибка добавления команды в слейв");
			return;
		}
		
		uint16_t count = m_currentSlave->getTypeCount(type);		// Обновляем количество
		m_currentSlave->setTypeCount(type, count + comStruct.count);

		if (!addToCommandCombobox(key, comStruct.name)) {
			MsgBox::error("Ошибка добавления команды в комбобокс команд");
			return;
		}

		emit signal_commandExist(comStruct);	// Обновляем таблицу с командами

	}

	//updateCommandCombobox();	// Обновляем комбобокс с командами
}

void ModbusWidgetSlaveSettings::slot_addInitCommandsFromLibrary(QList<sp_InitCommand> list)
{	
	for (sp_InitCommand command : list) {
		if (!m_currentSlave->addInitCommand(command))
			MsgBox::error("Ошибка добавления команды инициализации в слейв");
	}

	m_initCommandWidget->setCommandVector(m_currentSlave->getInitCommandsVector());
}
		
