#include "ModbusWidgetCommandAins.h"

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

static const QString ZERO_NOT_USED("0 (не преобразуется)");

ModbusWidgetCommandAins::ModbusWidgetCommandAins(spCommandDescr_t command, bool showAllParams, QWidget *parent)
	: ModbusWidgetCommonCommand(command, showAllParams, parent),
	m_comBoxDataOrder(nullptr),
	m_comBoxUpdAlg(nullptr)
{
	m_command = dynamic_cast<ModbusCommandAins*>(command.data());
	if (!m_command) {
		MsgBox::error("Невозможно преобразовать команду к типу Ains");
		return;
	}

	if (showAllParams == false) {	// Если не модифицированный Модбас, скрываем некоторые поля 
		setExpectedBcntVisible(m_isModifyModbus);
		setExpectedFirstRspBcntVisible(m_isModifyModbus);
		setAnalizedRspBcntVisible(m_isModifyModbus);

	}

	// Удаляем текстовое поле "Последовательность байт" и ставим комбобокс
	m_comBoxDataOrder = new QComboBox;
	//m_comBoxDataOrder->setMaximumWidth(COMMON_LEN);
	m_comBoxDataOrder->addItem("4 байта, начиная со старшего 4-3-2-1", ModbusAinBytesOrder_BE);
	m_comBoxDataOrder->addItem("4 байта, начиная с младшего 1-2-3-4", ModbusAinBytesOrder_LE);
	m_comBoxDataOrder->addItem("4 байта в последовательности 2-1-4-3", ModbusAinBytesOrder_HLFL);
	m_comBoxDataOrder->addItem("4 байта в последовательности 3-4-1-2", ModbusAinBytesOrder_HLFB);
	m_comBoxDataOrder->addItem("2 байта в последовательности 1-2", ModbusAinBytesOrder_16LE);
	m_comBoxDataOrder->addItem("2 байта в последовательности 2-1", ModbusAinBytesOrder_16BE);
	m_comBoxDataOrder->addItem("3 байта в последовательности 3-1-2", ModbusAinBytesOrder_Mercury3B);
	m_comBoxDataOrder->addItem("4 байта в последовательности 3-4-1-2", ModbusAinBytesOrder_Mercury4B);
	m_comBoxDataOrder->setCurrentIndex(m_command->getDataOrder());

	QLayoutItem* oldItem = ui.formLayoutForCommand->replaceWidget(ui.sBoxDataOrder, m_comBoxDataOrder);
	if (oldItem) {
		delete oldItem->widget();
		delete oldItem;
	}

	bool ok = connect(m_comBoxDataOrder, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxDataOrder_currentIndexChanged(int)));
	Q_ASSERT(ok);

	m_comBoxUpdAlg = new QComboBox;
	//m_comBoxUpdAlg->setMaximumWidth(COMMON_LEN);
	m_comBoxUpdAlg->addItem("Естественный float, преобразование не требуется", ModbusAinValAlg_Float);
	m_comBoxUpdAlg->addItem("Преобразование uint32_t во float", ModbusAinValAlg_Uint32);
	m_comBoxUpdAlg->addItem("Преобразование int32_t во float", ModbusAinValAlg_Int32);
	m_comBoxUpdAlg->addItem("Преобразование uint16_t во float", ModbusAinValAlg_Uint16);
	m_comBoxUpdAlg->addItem("Преобразование int16_t во float", ModbusAinValAlg_Int16);
	m_comBoxUpdAlg->addItem("Меркурий: знаковые 3 или 4 байта для активной энергии", ModbusAinValAlg_Mercury_P);
	m_comBoxUpdAlg->addItem("Меркурий: знаковые 3 или 4 байта для реактивной энергии", ModbusAinValAlg_Mercury_Q);
	m_comBoxUpdAlg->setCurrentIndex(m_command->getUpdAlg());

	oldItem = ui.formLayoutForCommand->replaceWidget(ui.sBoxUpdAlg, m_comBoxUpdAlg);
	if (oldItem) {
		delete oldItem->widget();
		delete oldItem;
	}

	ok = connect(m_comBoxUpdAlg, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxUpdAlg_currentIndexChanged(int)));
	Q_ASSERT(ok);


	// Независимые от типа Модбаса параметры
	m_lEdK0 = new QLineEdit(this);
	m_lEdK1 = new QLineEdit(this);
	m_lEdK0->setMaximumWidth(COMMON_LEN);
	m_lEdK1->setMaximumWidth(COMMON_LEN);
	ui.formLayoutForCommand->addRow("Коэффициент смещения К0", m_lEdK0);
	ui.formLayoutForCommand->addRow("Коэффициент умножения К1", m_lEdK1);
	m_lEdK0->setText(QString::number(m_command->getK0()));
	m_lEdK1->setText(QString::number(m_command->getK1()));
	ok = connect(m_lEdK0, SIGNAL(editingFinished()), SLOT(slot_lEdk0_editingFinished()));
	Q_ASSERT(ok);
	ok = connect(m_lEdK1, SIGNAL(editingFinished()), SLOT(slot_lEdk1_editingFinished()));
	Q_ASSERT(ok);


}

ModbusWidgetCommandAins::~ModbusWidgetCommandAins()
{
}

void ModbusWidgetCommandAins::slot_comBoxDataOrder_currentIndexChanged(int index) {
	uint8_t data = m_comBoxDataOrder->itemData(index).toInt();
	m_command->setDataOrder(data);
	//m_command->setDataOrder(index);
}

void ModbusWidgetCommandAins::slot_comBoxUpdAlg_currentIndexChanged(int index) {
	uint8_t data = m_comBoxUpdAlg->itemData(index).toInt();
	m_command->setUpdAlg(data);
	//m_command->setUpdAlg(index);
}

void ModbusWidgetCommandAins::slot_lEdk0_editingFinished() {
	bool ok;
	float k = m_lEdK0->text().toFloat(&ok);
	if (ok) {
		m_command->setK0(k);
	}
	else {
		m_lEdK0->blockSignals(true);
		m_lEdK0->setText("Ошибочное значение");
		m_lEdK0->blockSignals(false);
	}
}

void ModbusWidgetCommandAins::slot_lEdk1_editingFinished() {
	bool ok;
	float k = m_lEdK1->text().toFloat(&ok);
	if (ok) {
		m_command->setK1(k);
		if (k == 0) {
			m_lEdK1->blockSignals(true);
			m_lEdK1->setText(ZERO_NOT_USED);
			m_lEdK1->blockSignals(false);
		}
	}
	else {
		m_lEdK1->blockSignals(true);
		m_lEdK1->setText("Ошибочное значение");
		m_lEdK1->blockSignals(false);
	}
}