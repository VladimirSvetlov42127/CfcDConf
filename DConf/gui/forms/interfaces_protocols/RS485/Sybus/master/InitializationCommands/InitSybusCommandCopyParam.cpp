#include "InitSybusCommandCopyParam.h"

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/init_commands_defines.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

InitSybusCommandCopyParam::InitSybusCommandCopyParam(const QByteArray &commandArray, QWidget *parent)
	: InterfaseInitCommand(parent), m_initCommandBuf(commandArray),
	m_count(0), m_destAddress(0), m_destIndex(0),
	m_len(FIX_COMMAND_LEN - 1), m_sourseAddress(0), m_sourseIndex(0)
{
	ui.setupUi(this);
	ui.lEdCode->setText("0x" + QString::number(ns_sybusInitCommands::COPY_PARAM_CODE, ns_sybusInitCommands::PRINT_VIEW));	// Устанавливаю код команды

	// Если буфер меньше минимального, будем создавать заново
	if(m_initCommandBuf.isEmpty())
		setDefaultValues();
	else if (m_initCommandBuf.size() != FIX_COMMAND_LEN || m_initCommandBuf.at(LEN_POS) != FIX_COMMAND_LEN - 1) {	// Включая байт длины сообщения
		MsgBox::error("Команда имеет неверный размер. Не распознана.");
		setDefaultValues();		
	}
	else {
		getConfigFromBuf();	// Читаем из буфера, заполняем форму
	}


	bool ok;
	ok = connect(ui.lEdSourseAddress, SIGNAL(editingFinished()), SLOT(slot_lEdSourseAddress_editingFinished()));
	Q_ASSERT(ok);
	ok = connect(ui.spBoxSourseIndex, SIGNAL(valueChanged(int)), SLOT(slot_spBoxSourseIndex_valueChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.spBoxCount, SIGNAL(valueChanged(int)), SLOT(slot_spBoxCount_valueChanged(int)));
	Q_ASSERT(ok);
	ok = connect(ui.lEdDestAddress, SIGNAL(editingFinished()), SLOT(slot_lEdDestAddress_editingFinished()));
	Q_ASSERT(ok);
	ok = connect(ui.spBoxDestIndex, SIGNAL(valueChanged(int)), SLOT(slot_spBoxDestIndex_valueChanged(int)));
	Q_ASSERT(ok);

}

InitSybusCommandCopyParam::~InitSybusCommandCopyParam()
{
}

QByteArray InitSybusCommandCopyParam::getCommandByteArray() const
{
	QByteArray arr;

	arr.append(m_len);
	arr.append(ns_sybusInitCommands::COPY_PARAM_CODE);
	arr.append((char*)&m_sourseAddress, sizeof(m_sourseAddress));
	arr.append((char*)&m_sourseIndex, sizeof(m_sourseIndex));
	arr.append(m_count);
	arr.append((char*)&m_destAddress, sizeof(m_destAddress));
	arr.append((char*)&m_destIndex, sizeof(m_destIndex));

	return arr;
}


bool InitSybusCommandCopyParam::setDefaultValues() {	
	m_len = FIX_COMMAND_LEN - 1;
	return true;
}

bool InitSybusCommandCopyParam::getConfigFromBuf() {
	if (!checkDefaultValuesFromBuf()) {
		MsgBox::error("При проверке сохраненной команды возникли ошибки. Данные по умолчанию будут перезаписаны");
		setDefaultValues();
		return false;
	}
	
	uint8_t low = m_initCommandBuf.at(PARAM_SOURSE_NUMBER_POS);
	uint8_t high = m_initCommandBuf.at(PARAM_SOURSE_NUMBER_POS + 1);
	m_sourseAddress = low + (high << 8);
	QString sourseAddressStr = "0x%1";
	sourseAddressStr = sourseAddressStr.arg(QString::number(m_sourseAddress, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdSourseAddress->setText(sourseAddressStr);

	uint8_t lowSourseInd = m_initCommandBuf.at(PARAM_SOURSE_INDEX_POS);
	uint8_t highSourseInd = m_initCommandBuf.at(PARAM_SOURSE_INDEX_POS + 1);
	m_sourseIndex = lowSourseInd + (highSourseInd << 8);
	ui.spBoxSourseIndex->setValue(m_sourseIndex);

	m_count = m_initCommandBuf.at(COUNT_POS);
	ui.spBoxCount->setValue(m_count);

	uint8_t lowDestAddr = m_initCommandBuf.at(PARAM_DEST_NUMBER_POS);
	uint8_t highDestAddr = m_initCommandBuf.at(PARAM_DEST_NUMBER_POS + 1);
	m_destAddress = lowDestAddr + (highDestAddr << 8);
	QString destAddrStr = "0x%1";
	destAddrStr = destAddrStr.arg(QString::number(m_destAddress, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdDestAddress->setText(destAddrStr);

	uint8_t lowDestInd = m_initCommandBuf.at(PARAM_DEST_INDEX_POS);
	uint8_t highDestInd = m_initCommandBuf.at(PARAM_DEST_INDEX_POS + 1);
	m_destIndex = lowDestInd + (highDestInd << 8);
	ui.spBoxDestIndex->setValue(m_destIndex);
	return true;
}

bool InitSybusCommandCopyParam::checkDefaultValuesFromBuf() {

	if (m_initCommandBuf.at(CODE_POS) != static_cast<char>(ns_sybusInitCommands::COPY_PARAM_CODE))
		return false;
	if (m_initCommandBuf.at(LEN_POS) != FIX_COMMAND_LEN - 1)
		return false;

	return true;
}


// SLOTS
void InitSybusCommandCopyParam::slot_lEdSourseAddress_editingFinished() {
	bool ok;
	uint16_t paramNumber = ui.lEdSourseAddress->text().toInt(&ok, ns_sybusInitCommands::PRINT_VIEW);
	if (!ok) {
		ui.lEdSourseAddress->setText("Ошибка формата");
		return;
	}

	m_sourseAddress = paramNumber;

	QString number = "0x%1";
	number = number.arg(QString::number(paramNumber, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdSourseAddress->setText(number);
}

void InitSybusCommandCopyParam::slot_spBoxSourseIndex_valueChanged(int value) {
	m_sourseIndex = value;
}

void InitSybusCommandCopyParam::slot_spBoxCount_valueChanged(int val) {
	m_count = val;
}

void InitSybusCommandCopyParam::slot_lEdDestAddress_editingFinished() {
	bool ok;
	uint16_t paramNumber = ui.lEdDestAddress->text().toInt(&ok, ns_sybusInitCommands::PRINT_VIEW);
	if (!ok) {
		ui.lEdDestAddress->setText("Ошибка формата");
		return;
	}

	m_destAddress = paramNumber;

	QString number = "0x%1";
	number = number.arg(QString::number(paramNumber, ns_sybusInitCommands::PRINT_VIEW).toUpper(), 4, '0');
	ui.lEdDestAddress->setText(number);
}

void InitSybusCommandCopyParam::slot_spBoxDestIndex_valueChanged(int value) {
	m_destIndex = value;
}