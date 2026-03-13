#include "ModbusWidgetCommonCommand.h"

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Common/common_CommandDefines.h>

using namespace Dpc::Gui;

static const QString ERROR_VALUE("Ошибка ввода");

ModbusWidgetCommonCommand::ModbusWidgetCommonCommand(spCommandDescr_t command, bool isModifyModbus, QWidget *parent)
	: QWidget(parent), m_command(command), m_isModifyModbus(isModifyModbus)
{
	ui.setupUi(this);

	if (m_command->getDataBytes() == 0)
		ui.spinBoxDataBytes->setValue(8);
	else 
		ui.spinBoxDataBytes->setValue(m_command->getDataBytes());

	ui.sBoxDataOrder->setValue(m_command->getDataOrder());
	ui.sBoxUpdAlg->setValue(m_command->getUpdAlg());
	ui.sBoxExpectedBcnt->setValue(m_command->getExpectedBcnt());
	ui.sBoxAnalizedRspBcnt->setValue(m_command->getAnalizedRspBcnt());

	setExpectedFirstRspBcnt(m_command->getExpectedFirstRspBcnt());

}

ModbusWidgetCommonCommand::~ModbusWidgetCommonCommand()
{
}


void ModbusWidgetCommonCommand::updateFromFunctionInfo(spModbusFunc_t function)
{
	if (!function)
		return;

	setDataBytes(m_command->getDataBytes());	// Виртуальная функция. Особенность CIns

	// Если функция стандартная, она сама определяет параметры, иначе - они хранятся только в команде, введенные пользователем
	if (m_command->isSupportedFunctionCode(function->getFunctionType())) {
		setExpectedBcnt(function->getExpectedBcnt());
		setAnalizedRspBcnt(function->getAnalizedRspBcnt());
		setExpectedFirstRspBcnt(function->getExpectedFirstRspBcnt());
	}

}

void ModbusWidgetCommonCommand::setExpectedFirstRspBcnt(QByteArray value)
{
	uint8_t analizedByte = m_command->getAnalizedRspBcnt();	// Должно быть меньше размера массива
	
	uint limit = (analizedByte > value.size() ? value.size() : analizedByte);

	QString arrayStr;
	for (int i = 0; i < limit; i++) {
		QString str("%1");
		uint8_t byte = value[i];
		//QString tmp = QString::number(byte, 16);
		arrayStr += str.arg(QString::number(byte, 16).toUpper(), 2, '0') + " ";
	}
	ui.lEdExpectedFirstRspByte->setText(arrayStr);
}

void ModbusWidgetCommonCommand::setDataBytesVisible(bool isVesible)
{
	ui.spinBoxDataBytes->setVisible(isVesible);
	ui.labDataBytes->setVisible(isVesible);
}

void ModbusWidgetCommonCommand::setDataOrderVisible(bool isVesible)
{
	ui.sBoxDataOrder->setVisible(isVesible);
	ui.labDataOrder->setVisible(isVesible);
}

void ModbusWidgetCommonCommand::setUpdAlgVisible(bool isVesible)
{
	ui.sBoxUpdAlg->setVisible(isVesible);
	ui.labUpdAlg->setVisible(isVesible);
}

void ModbusWidgetCommonCommand::setExpectedBcntVisible(bool isVesible)
{
	ui.sBoxExpectedBcnt->setVisible(isVesible);
	ui.labExpectedBcnt->setVisible(isVesible);
}

void ModbusWidgetCommonCommand::setAnalizedRspBcntVisible(bool isVesible)
{
	ui.sBoxAnalizedRspBcnt->setVisible(isVesible);
	ui.labAnalizedRspBcnt->setVisible(isVesible);
}

void ModbusWidgetCommonCommand::setExpectedFirstRspBcntVisible(bool isVesible)
{
	ui.lEdExpectedFirstRspByte->setVisible(isVesible);
	ui.labExpectedFirstRspByte->setVisible(isVesible);
}

// Слоты
void ModbusWidgetCommonCommand::on_spinBoxDataBytes_valueChanged(int value) {
	if (value < 0 || value > 8)
		return;

	m_command->setDataBytes(value);
	spModbusFunc_t fuction(m_command->getModbusFunction());
	if (fuction) {
		fuction->setDataByte(value);
	}
}

void ModbusWidgetCommonCommand::on_sBoxDataOrder_valueChanged(int value) {
	m_command->setDataOrder(value);

	/*uint16_t editVal;
	bool ok;
	editVal = ui.lEdDataOrder->text().toUShort(&ok);
	if (ok) {
		if (editVal < 0xFF)
			m_command->setDataOrder(editVal);
		else
			ui.lEdDataOrder->setText(ERROR_VALUE);
	}
	else
		ui.lEdDataOrder->setText(ERROR_VALUE);*/

}

void ModbusWidgetCommonCommand::on_sBoxUpdAlg_valueChanged(int value) {
	
	m_command->setUpdAlg(value);

	//uint16_t editVal;
	//bool ok;
	//editVal = ui.lEdUpdAlg->text().toUShort(&ok);
	//if (ok) {
	//	if (editVal < 0xFF)
	//		m_command->setUpdAlg(editVal);
	//	else
	//		ui.lEdUpdAlg->setText(ERROR_VALUE);
	//}
	//else
	//	ui.lEdUpdAlg->setText(ERROR_VALUE);
}

void ModbusWidgetCommonCommand::on_sBoxExpectedBcnt_valueChanged(int value) {

	m_command->setExpectedBcnt(value);

	/*uint16_t editVal;
	bool ok;
	editVal = ui.lEdExpectedBcnt->text().toUShort(&ok);
	if (ok) {
		if (editVal < 0xFF)
			m_command->setExpectedBcnt(editVal);
		else
			ui.lEdExpectedBcnt->setText(ERROR_VALUE);
	}
	else
		ui.lEdExpectedBcnt->setText(ERROR_VALUE);*/
}

void ModbusWidgetCommonCommand::on_sBoxAnalizedRspBcnt_valueChanged(int value) {

	if (value <= ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
		m_command->setAnalizedRspBcnt(value);


	/*uint16_t editVal;
	bool ok;
	editVal = ui.lEdAnalizedRspBcnt->text().toUShort(&ok);
	if (ok) {
		if (editVal <= ModbusCommand_ns::ExpectedFirstRspBcnt_Count)
			m_command->setExpectedBcnt(editVal);
		else
			ui.lEdAnalizedRspBcnt->setText(ERROR_VALUE);
	}
	else
		ui.lEdAnalizedRspBcnt->setText(ERROR_VALUE);*/
}

void ModbusWidgetCommonCommand::on_lEdExpectedFirstRspByte_editingFinished() {

	QString values(ui.lEdExpectedFirstRspByte->text());

	QStringList list(values.split(' ', Qt::SkipEmptyParts, Qt::CaseInsensitive));
	if (list.size() > ModbusCommand_ns::ExpectedFirstRspBcnt_Count) {
		ui.lEdExpectedFirstRspByte->setText("Слишком много байт");
		return;
	}

	bool ok;
	uint16_t val = 0;
	QByteArray byteArray;
	for(QString str: list)
	{
		val = str.toUShort(&ok, 16);
		if (!ok) {
			ui.lEdExpectedFirstRspByte->blockSignals(true);
			ui.lEdExpectedFirstRspByte->setText("Неверный формат байта");
			ui.lEdExpectedFirstRspByte->blockSignals(false);
			return;
		}
		if (val > 0xff) {
			ui.lEdExpectedFirstRspByte->blockSignals(true);
			ui.lEdExpectedFirstRspByte->setText("Превышен размер байта");
			ui.lEdExpectedFirstRspByte->blockSignals(false);
			return;
		}

		uint8_t byte(val & 0xFF);
		byteArray.append(byte);
	}

	if (!m_command->setExpectedFirstRspBcnt(byteArray)) {
		MsgBox::error("Не удалось записать значение ExpectedFirstRspBcnt");
		return;
	}

}