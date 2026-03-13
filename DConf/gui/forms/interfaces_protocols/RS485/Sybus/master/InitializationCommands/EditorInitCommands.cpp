#include "EditorInitCommands.h"
#include <qmap.h>
#include <qlabel.h>
#include <qgroupbox.h>
#include <qpushbutton.h>

#include <gui/forms/interfaces_protocols/RS485/macrosCheckPointer.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/InitSybusCommandSetParam.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/init_commands_defines.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/InitSybusCommandCopyParam.h>

using namespace Dpc::Gui;

static const QMap<uint8_t, QString> cm_initCommandTypes{
	{ns_sybusInitCommands::SET_PARAM_CODE, " Установить параметр "},
	{ns_sybusInitCommands::COPY_PARAM_CODE, " Скопировать параметр "}
};



EditorInitCommands::EditorInitCommands(DcController *controller, uint16_t startSlaveInitIndex, QWidget *parent)
	: QDialog(parent), m_controller(controller), m_startInitFieldIndex(startSlaveInitIndex), 
	m_currentCommandWidget(nullptr)

{
    this->setWindowIcon(QIcon(":/icons/params.svg"));
	this->setWindowTitle("Редактирование команд инициализации");
	this->setMinimumHeight(500);
	bool connectOk;

	QVBoxLayout *mainVBox = new QVBoxLayout(this);

	// Заполняем верхнюю строчку для создания команды
	QHBoxLayout *hBoxCreate = new QHBoxLayout;
	mainVBox->addLayout(hBoxCreate);
	QLabel *labCreate = new QLabel("Выберите тип команды для создания: ");
	labCreate->setMinimumWidth(220);
	hBoxCreate->addWidget(labCreate);

	m_comBoxCreateType = new QComboBox;
	m_comBoxCreateType->setMinimumWidth(250);
	QMapIterator<uint8_t, QString> i(cm_initCommandTypes);
	while (i.hasNext()) {
		i.next();
		m_comBoxCreateType->addItem(i.value(), i.key());
	}
	hBoxCreate->addWidget(m_comBoxCreateType);

	QPushButton *butCreate = new QPushButton("Создать");
	hBoxCreate->addWidget(butCreate);
	connectOk = connect(butCreate, SIGNAL(clicked()), SLOT(slot_butCreate_clicked()));
	Q_ASSERT(connectOk);

	// Создаем вторую строку с выбором текущей редактируемой команды
	QHBoxLayout *hBoxCurrentCommand = new QHBoxLayout;
	mainVBox->addLayout(hBoxCurrentCommand);
	QLabel *labChoose = new QLabel("Выберите команду для редактирования: ");
	labChoose->setMinimumWidth(220);
	hBoxCurrentCommand->addWidget(labChoose);

	m_comBoxCommands = new QComboBox;
	m_comBoxCommands->setMinimumWidth(250);
	hBoxCurrentCommand->addWidget(m_comBoxCommands);
	connectOk = connect(m_comBoxCommands, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxCommand_itemChanged(int)));
	Q_ASSERT(connectOk);

	QPushButton *butErase = new QPushButton("Удалить текущую");
	hBoxCurrentCommand->addWidget(butErase);
	connectOk = connect(butErase, SIGNAL(clicked()), SLOT(slot_butErase_cliked()));
	Q_ASSERT(connectOk);

	// Добавление места для вставки виджетов команд
	QGroupBox *group = new QGroupBox("Параметры команды");
	mainVBox->addWidget(group);
	m_commandLayout = new QVBoxLayout(group);

	// Добавление кнопок сохранения и выхода
	QHBoxLayout *hBoxSaveAndClose = new QHBoxLayout;
	mainVBox->addLayout(hBoxSaveAndClose);
	hBoxSaveAndClose->addStretch(1);
	
	QPushButton *butSave = new QPushButton("Сохранить");
	butSave->setDefault(true);
	hBoxSaveAndClose->addWidget(butSave);
	connectOk = connect(butSave, SIGNAL(clicked()), SLOT(slot_butSave_clicked()));
	Q_ASSERT(connectOk);

	QPushButton *butCansel = new QPushButton("Отменить");
	hBoxSaveAndClose->addWidget(butCansel);
	connectOk = connect(butCansel, SIGNAL(clicked()), SLOT(slot_butCancel_clicked()));
	Q_ASSERT(connectOk);
		
	 ////Если парсинг успешный, то после подключенных сигналов должен отработать слот на изменение в комбобоксе текущей команды
	if (parseModel()) {	// Если удачно распарсили модель
		uint16_t count = mv_initCommands.size();
		for (int i = 0; i < count; i++) {

			uint8_t type = mv_initCommands[i]->getCommandByteArray()[COMMAND_TYPE_POS];
			switch (type)	// Ищем в буфера код функции, должна быть на позиции codeIndexInBuf
			{
			case ns_sybusInitCommands::SET_PARAM_CODE:
				m_comBoxCommands->addItem(QString::number(i + 1) + cm_initCommandTypes.value(ns_sybusInitCommands::SET_PARAM_CODE), QVariant(ns_sybusInitCommands::SET_PARAM_CODE));
				break;

			case ns_sybusInitCommands::COPY_PARAM_CODE:
				m_comBoxCommands->addItem(QString::number(i + 1) + cm_initCommandTypes.value(ns_sybusInitCommands::COPY_PARAM_CODE), QVariant(ns_sybusInitCommands::COPY_PARAM_CODE));
				break;

			default:
				MsgBox::error("Неопределенный тип команды!");
				mv_initCommands.remove(i);
				break;
			}
		}
	}	

}

EditorInitCommands::~EditorInitCommands()
{
	for (InterfaseInitCommand* cmd : mv_initCommands)
		delete cmd;
}


bool EditorInitCommands::parseModel()
{
	if (!m_controller) {
		MsgBox::error("Нет контроллера!");
		return false;
	}
			
	// Определяем размер поля для команд инициализации

    auto startParam = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, m_startInitFieldIndex);
	CHECK_ADDRESS_RETURN_BOOL(startParam);
    m_maxCommandByteSize = startParam->parameter()->elementsCount() - 1;	// Сразу для записи определяем

	uint16_t commandCountToParse = startParam->value().toInt();	// Сначала идет индекс команды от последней к первой
	if (commandCountToParse == 0)
		return false;	// Нет команд

	uint16_t cmdStartIndex = m_startInitFieldIndex + 1;	// Начинаем считывать с длины
    auto firstCommandLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, cmdStartIndex);
	CHECK_ADDRESS_RETURN_BOOL(firstCommandLen);

	uint16_t commandLen = firstCommandLen->value().toInt();	// Длина первой команды
	if (commandLen == 0) {
		MsgBox::error("Длина первой команды равна нулю");
		return false;
	}

	while (commandLen != 0) {

		QByteArray commandBuf;
		commandLen++;	// Увеличиваем длину команды, т.к. в длину не входит первый байт, хранящий размер команды
		for (int i = 0; i < commandLen; i++) {	
            auto param = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, cmdStartIndex + i);
			CHECK_ADDRESS_RETURN_BOOL(param);
			commandBuf.append(param->value().toInt());
		}
		
		uint8_t type = commandBuf[COMMAND_TYPE_POS];
		switch (type)
		{
		case ns_sybusInitCommands::SET_PARAM_CODE: {
			mv_initCommands.append(new InitSybusCommandSetParam(commandBuf));
			break;
		}
		case ns_sybusInitCommands::COPY_PARAM_CODE: {
			mv_initCommands.append(new InitSybusCommandCopyParam(commandBuf));
			break;
		}
		default:
			MsgBox::error("Обнаружен неопознанный код команды, команда не вычитана");
			break;
		}

		//mv_initCommandArrays.append(commandBuf);
		--commandCountToParse;
		if (commandCountToParse == 0)
			return true;

		// Проверяем порядковый номер следующей команды
        auto nextCommandId = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, cmdStartIndex + commandLen);
		CHECK_ADDRESS_RETURN_BOOL(nextCommandId);
		if (nextCommandId->value().toInt() != commandCountToParse) {	// Если следующий индекс не совпал с ожидаемым, то это ошибка
			MsgBox::error("Ошибка парсинга! Номер команды не совпадает с ожидаемым!");
			break;	// Одну распарсили уже, ее надо отобразить
		}
		cmdStartIndex++;
        auto nextLen = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, cmdStartIndex + commandLen);
		CHECK_ADDRESS_RETURN_BOOL(nextLen);
		
		cmdStartIndex += commandLen;
		commandLen = nextLen->value().toInt();
		if (commandLen > m_maxCommandByteSize + m_startInitFieldIndex - cmdStartIndex)
			break;	// Ошибка! Выход за пределы параметров

	}

	return true;
}

void EditorInitCommands::updateCommandNamesInCombobox()
{
	for (int i = 0; i < m_comBoxCommands->count(); i++) {

		switch (m_comBoxCommands->itemData(i).toInt())	// Ищем в буфера код функции, должна быть на позиции codeIndexInBuf
		{
		case ns_sybusInitCommands::SET_PARAM_CODE:
			m_comBoxCommands->setItemText(i, QString::number(i + 1) + cm_initCommandTypes.value(ns_sybusInitCommands::SET_PARAM_CODE));
			break;

		case ns_sybusInitCommands::COPY_PARAM_CODE:
			m_comBoxCommands->setItemText(i, QString::number(i + 1) + cm_initCommandTypes.value(ns_sybusInitCommands::COPY_PARAM_CODE));
			break;

		default:
			MsgBox::error("Неопределенный тип команды!");
			break;
		}
	}
}

void EditorInitCommands::replaceWidgetInGroupBox(QWidget * wid)
{
	if (!wid)
		return;

	if (m_commandLayout->isEmpty()) {
		m_commandLayout->addWidget(wid);
	}
	else {
		QLayoutItem* item = m_commandLayout->replaceWidget(m_currentCommandWidget, wid);
		if (item) {
			item->widget()->setParent(nullptr);
			//delete item->widget();	// Виджет не удаляем, т.к. храним его в векторе!
			delete item;
		}
	}

	m_currentCommandWidget = wid;
}


// SLOTS

void EditorInitCommands::slot_comBoxCommand_itemChanged(int index) {
	if (index > mv_initCommands.size())
		return;
	if (m_comBoxCommands->count() == 0) {
		replaceWidgetInGroupBox(new QWidget());
		return;
	}

	QWidget *wid = mv_initCommands[index];

	replaceWidgetInGroupBox(wid);
	
}

void EditorInitCommands::slot_butCreate_clicked() {

	uint8_t type = m_comBoxCreateType->currentData().toInt();	// Получаем индекс в шаблонном массиве, хранящем типы и их имена
	
	switch (type)
	{
	case ns_sybusInitCommands::SET_PARAM_CODE: {
		mv_initCommands.append(new InitSybusCommandSetParam(QByteArray()));
		break;
	}
	case ns_sybusInitCommands::COPY_PARAM_CODE: {
		mv_initCommands.append(new InitSybusCommandCopyParam(QByteArray()));
		break;
	}
	default:
		MsgBox::error("Обнаружен неопознанный код команды, команда не создана!");
		return;
	}

	m_comBoxCommands->addItem(QString::number(m_comBoxCommands->count() + 1) + cm_initCommandTypes.value(type), type);	// Добавляем в список
	m_comBoxCommands->setCurrentIndex(m_comBoxCommands->count() - 1);
}

void EditorInitCommands::slot_butErase_cliked() {
	if (mv_initCommands.isEmpty())
		return;

	if (m_comBoxCommands->count() == 0) {	// Если вдруг загрузилось неизвестно что, удаляем все что было
		for (auto command : mv_initCommands) {
			if(command)
				delete command;
		}
		mv_initCommands.clear();
		return;
	}

	int index = m_comBoxCommands->currentIndex();
	delete mv_initCommands[index];
	mv_initCommands.remove(index);
	
	m_comBoxCommands->removeItem(index);
	updateCommandNamesInCombobox();		// Обновляем имена 
}

void EditorInitCommands::slot_butSave_clicked() {

	if (!m_controller) {
		MsgBox::error("Нет контроллера!");
		return;
	}

	uint16_t allCommansByteLen = 0;
	for(auto command: mv_initCommands)
		allCommansByteLen += command->getCommandByteArray().size();

	if (allCommansByteLen >= m_maxCommandByteSize) {
		MsgBox::error("Суммарный размер всех команд превышает допустимый размер по байтам. Сохранение не выполнено");
		return;
	}

	uint16_t index = m_startInitFieldIndex;
	for (int commandId = 0; commandId < mv_initCommands.size(); commandId++) {	// Проходим по командам в обратном порядке

	// Записываем порядковый номер команды, обратный отсчет
        auto commandIndex = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, index++);
		CHECK_ADDRESS(commandIndex);
        commandIndex->updateValue(QString::number(mv_initCommands.size() - commandId));

		// Записываем данные команды
		const QByteArray& commandArray = mv_initCommands[commandId]->getCommandByteArray();
		uint16_t size = commandArray.size();
		for (int byteInd = 0; byteInd < size; byteInd++) {	// Проходим по массивам команд
            auto param = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, index++);
			CHECK_ADDRESS(param);
            param->updateValue(QString::number(commandArray[byteInd]));
		}
	}

	// Добавляем нули в конец
    auto param = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, index);
	CHECK_ADDRESS(param);
	param->updateValue(QString::number(0));
	
	accept();
}


void EditorInitCommands::slot_butCancel_clicked() {
	reject();
}
