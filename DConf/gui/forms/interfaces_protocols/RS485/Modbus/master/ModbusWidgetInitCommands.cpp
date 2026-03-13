#include "ModbusWidgetInitCommands.h"

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_ExtraFunctions.h>

static const QString INIT_COMMAND_NAME = ("Команда инициализации ");

ModbusWidgetInitCommands::ModbusWidgetInitCommands(QVector<spInitCommand>* vector, QWidget *parent)
	: QWidget(parent),
	m_curInitCommand(nullptr),
	mv_commands(nullptr)
{
	ui.setupUi(this);
	

	bool ok;
	ok = connect(ui.comBoxInitCommands, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxInitCommand_itemChanged(int)));
	Q_ASSERT(ok);

	ok = connect(ui.butAdd, SIGNAL(clicked()), SLOT(slot_butAdd_clicked()));
	Q_ASSERT(ok);
	ok = connect(ui.butErase, SIGNAL(clicked()), SLOT(slot_butErase_clicked()));
	Q_ASSERT(ok);

	ok = connect(ui.spBoxExpectedBcnt, SIGNAL(valueChanged(int)), SLOT(slot_spBox_valueChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.lEdExpectedFirstRspByte, SIGNAL(editingFinished()), SLOT(slot_lEdExpectedFirstRspByte_editingFinished()));
	Q_ASSERT(ok);
	ok = connect(ui.lEdInitCommand, SIGNAL(editingFinished()), SLOT(slot_lEdInitCmdBuf_editingFinished()));
	Q_ASSERT(ok);
	
	setCommandVector(vector);

	if (vector->isEmpty()) {	// Если команд нет, блокируем редактирование полей
		blockFields(true);
	}
}

ModbusWidgetInitCommands::~ModbusWidgetInitCommands()
{
}

bool ModbusWidgetInitCommands::setCommandVector(QVector<spInitCommand>* vector)
{
	mv_commands = vector;	// Будем модифицировать сразу его
	ui.comBoxInitCommands->clear();

	for (int i = 0; i < mv_commands->size(); i++) {
		QString name = INIT_COMMAND_NAME + QString::number(i + 1);
		ui.comBoxInitCommands->addItem(name);
	}

	return true;
}



// SLOTS

void ModbusWidgetInitCommands::slot_comBoxInitCommand_itemChanged(int value) {
	if (ui.comBoxInitCommands->count() == 0)
		return;

	if (value > mv_commands->size())
		return;

	m_curInitCommand = mv_commands->at(value);
	ui.spBoxExpectedBcnt->setValue(m_curInitCommand->getExpectedBcnt());	

	QString ExpStr (makeHexByteStringFromArray(m_curInitCommand->getExpectedFirstRspByte(), m_curInitCommand->getAnalizedRspBcnt()));
	ui.lEdExpectedFirstRspByte->setText(ExpStr);

	QString initStr(makeHexByteStringFromArray(m_curInitCommand->getInitCmdBuf(), m_curInitCommand->getInitCmdBuf().size()));
	ui.lEdInitCommand->setText(initStr);

	blockFields(false);	// Снимаем блокировку при заполненных полях
}

void ModbusWidgetInitCommands::blockFields(bool block)
{
	bool isEnable = !block;
	ui.spBoxExpectedBcnt->setEnabled(isEnable);
	ui.lEdExpectedFirstRspByte->setEnabled(isEnable);
	ui.lEdInitCommand->setEnabled(isEnable);
}

void ModbusWidgetInitCommands::slot_butAdd_clicked() {

	if (!mv_commands)
		return;

	spInitCommand newCommand(new ModbusCommandInitialization);
	mv_commands->append(newCommand);

	QString name = INIT_COMMAND_NAME + QString::number(ui.comBoxInitCommands->count() + 1);
	ui.comBoxInitCommands->addItem(name);
	ui.comBoxInitCommands->setCurrentText(name);
	m_curInitCommand = newCommand;

	if (mv_commands->count() == 1) {
		ui.spBoxExpectedBcnt->setEnabled(true);
		ui.lEdExpectedFirstRspByte->setEnabled(true);
		ui.lEdInitCommand->setEnabled(true);
	}
}

void ModbusWidgetInitCommands::slot_butErase_clicked() {
	if (!mv_commands)
		return;
	if (!m_curInitCommand)
		return;
	if (mv_commands->isEmpty())
		return;

	mv_commands->removeOne(m_curInitCommand);
	m_curInitCommand = nullptr;
	ui.comBoxInitCommands->removeItem(ui.comBoxInitCommands->currentIndex());

	if (mv_commands->isEmpty()) {
		ui.spBoxExpectedBcnt->setEnabled(false);
		ui.spBoxExpectedBcnt->clear();
		ui.lEdExpectedFirstRspByte->setEnabled(false);
		ui.lEdExpectedFirstRspByte->clear();
		ui.lEdInitCommand->setEnabled(false);
		ui.lEdInitCommand->clear();
	}
	else {
		for (int i = 0; i < ui.comBoxInitCommands->count(); i++) {	// Переименовываем все
			ui.comBoxInitCommands->setItemText(i, INIT_COMMAND_NAME + QString::number(i + 1));
		}
	}

}

void ModbusWidgetInitCommands::slot_spBox_valueChanged(int value) {
	m_curInitCommand->setExpectedBcnt(value);
}

void ModbusWidgetInitCommands::slot_lEdExpectedFirstRspByte_editingFinished() {

	if (ui.lEdExpectedFirstRspByte->text().isEmpty())
		return;

	QByteArray arr;
	if (makeArrayFromHexString(ui.lEdExpectedFirstRspByte->text(), arr)) {
		if (arr.size() > ModbusCommand_ns::ExpectedFirstRspBcnt_Count) {
			QString text = "Размер не должен превышать %1 байт";
			text = text.arg(ModbusCommand_ns::ExpectedFirstRspBcnt_Count);
			ui.lEdExpectedFirstRspByte->setText(text);
		}
		else
			if (!m_curInitCommand->setExpectedFirstRspByte(arr)) {
				ui.lEdExpectedFirstRspByte->setText("Ошибка сохранения в слейве");
			}
	}
	else
		ui.lEdExpectedFirstRspByte->setText("Ошибка формата записи");
	
}

void ModbusWidgetInitCommands::slot_lEdInitCmdBuf_editingFinished() {
	
	if (ui.lEdInitCommand->text().isEmpty())
		return;

	QByteArray arr;
	if (makeArrayFromHexString(ui.lEdInitCommand->text(), arr)) {
		if (arr.size() > ModbusCommandInitialization::MAX_INIT_COMMAND_BYTE_LEN) {
			QString text = "Размер не должен превышать %1 байт";
			text = text.arg(ModbusCommandInitialization::MAX_INIT_COMMAND_BYTE_LEN);
			ui.lEdInitCommand->setText(text);
		}			
		else
			if (!m_curInitCommand->setInitCmdBuf(arr)) {
				ui.lEdInitCommand->setText("Ошибка сохранения в слейве");
			}
	}
	else
		ui.lEdInitCommand->setText("Ошибка формата записи");
}