#include "ModbusWidgetCommandCins.h"

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

enum {/*приходящая последовательность байт*/
	ModbusCinByteOrder_BE64 = 0,	/*8 байт, начиная со старшего 8-7-6-5-4-3-2-1*/
	ModbusCinByteOrder_LE64,		/*8 байт, начиная с младшего  1-2-3-4-5-6-7-8*/
	ModbusCinByteOrder_BE32,		/*4 байта, начиная со старшего   4-3-2-1*/
	ModbusCinByteOrder_LE32,		/*4 байта, начиная с младшего   1-2-3-4*/
	ModbusCinByteOrder_HLFL,		/*4 байта в последовательности  2-1-4-3*/
	ModbusCinByteOrder_HLFB,   		/*4 байта в последовательности  3-4-1-2*/

	ModbusCinByteOrder_Unknown,
};

enum {
	ModbusCinAlg_64 = 0,	 /*8 байт*/
	ModbusCinAlg_32,		/*младшие 4 байта*/
	ModbusCinAlg_32_INC,	/* младшие 4 байта с инкрементом старших байт при переполнении*/

	ModbusCinAlg_Unknown,
};


ModbusWidgetCommandCins::ModbusWidgetCommandCins(spCommandDescr_t command, bool isModifyModbus, QWidget *parent)
	: ModbusWidgetCommonCommand(command, isModifyModbus, parent),
	m_comBoxDataOrder(nullptr),
	m_comBoxUpdAlg(nullptr)
{
	m_command = dynamic_cast<ModbusCommandCins*>(command.data());
	if (!m_command) {
		MsgBox::error("Невозможно преобразовать команду к типу Ains");
		return;
	}

	if (!isModifyModbus) {	// Если не модифицированный Модбас, скрываем некоторые поля и ставим комбобоксы
		setExpectedBcntVisible(m_isModifyModbus);
		setExpectedFirstRspBcntVisible(m_isModifyModbus);
		setAnalizedRspBcntVisible(m_isModifyModbus);

		replaceWidgets();	// Заменяем на комбобоксы 
	}

	// Удаляем текстовое поле "Последовательность байт" и ставим комбобокс
	m_comBoxDataOrder = new QComboBox;
	//m_comBoxDataOrder->setMaximumWidth(COMMON_LEN);
	m_comBoxDataOrder->addItem("8 байт, начиная со старшего 8-7-6-5-4-3-2-1", ModbusCinByteOrder_BE64);
	m_comBoxDataOrder->addItem("8 байт, начиная с младшего 1-2-3-4-5-6-7-8", ModbusCinByteOrder_LE64);
	m_comBoxDataOrder->addItem("4 байта, начиная со старшего 4-3-2-1", ModbusCinByteOrder_BE32);
	m_comBoxDataOrder->addItem("4 байта, начиная с младшего   1-2-3-4", ModbusCinByteOrder_LE32);
	m_comBoxDataOrder->addItem("4 байта в последовательности  2-1-4-3", ModbusCinByteOrder_HLFL);
	m_comBoxDataOrder->addItem("4 байта в последовательности  3-4-1-2", ModbusCinByteOrder_HLFB);
	m_comBoxDataOrder->setCurrentIndex(m_command->getDataOrder());

	QLayoutItem* oldItemDataOrder = ui.formLayoutForCommand->replaceWidget(ui.sBoxDataOrder, m_comBoxDataOrder);
	if (oldItemDataOrder) {
		delete oldItemDataOrder->widget();
		delete oldItemDataOrder;
	}

	bool ok = connect(m_comBoxDataOrder, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxDataOrder_currentIndexChanged(int)));
	Q_ASSERT(ok);

	m_comBoxUpdAlg = new QComboBox;
	//m_comBoxUpdAlg->setMaximumWidth(COMMON_LEN);
	m_comBoxUpdAlg->addItem("8 байт", ModbusCinAlg_64);
	m_comBoxUpdAlg->addItem("Младшие 4 байта", ModbusCinAlg_32);
	m_comBoxUpdAlg->addItem("Младшие 4 байта с инкрементом старших байт при переполнении", ModbusCinAlg_32_INC);
	m_comBoxUpdAlg->setCurrentIndex(m_command->getUpdAlg());

	QLayoutItem* oldItemUpdAlg = ui.formLayoutForCommand->replaceWidget(ui.sBoxUpdAlg, m_comBoxUpdAlg);
	if (oldItemUpdAlg) {
		delete oldItemUpdAlg->widget();
		delete oldItemUpdAlg;
	}

	ok = connect(m_comBoxUpdAlg, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxUpdAlg_currentIndexChanged(int)));
	Q_ASSERT(ok);

	// Новые параметры
	m_sBoxMultiplicator = new QSpinBox;
	m_sBoxMultiplicator->setMaximumWidth(COMMON_LEN);
	ui.formLayoutForCommand->addRow("Множитель", m_sBoxMultiplicator);

	ok = connect(m_sBoxMultiplicator, SIGNAL(valueChanged(int)), SLOT(slot_sBoxMultiplicator_valueChanged(int)));
	Q_ASSERT(ok);
}

ModbusWidgetCommandCins::~ModbusWidgetCommandCins()
{
}

void ModbusWidgetCommandCins::setDataBytes(uint8_t value)
{
	if (!m_isModifyModbus) {	// Если не модифицированный модбас, то мы заменили комбобокс
		if (value == 8 || value == 0)
			m_comBoxDataBytes->setCurrentIndex(0);
		else if (value == 4)
			m_comBoxDataBytes->setCurrentIndex(1);
	}
	else {	// Если модбас модифицированный, то комбобокс остался спин боксом
		ModbusWidgetCommonCommand::setDataBytes(value);
	}

}

void ModbusWidgetCommandCins::replaceWidgets()
{
	// DataBytes представляет собой только два варианта: 8 или 4 байт, используем комбобокс
	m_comBoxDataBytes = new QComboBox;
	m_comBoxDataBytes->addItem("8 байт", 0);
	m_comBoxDataBytes->addItem("4 байта", 4);

	if (m_command->getDataBytes() != 0) {	// Если не равно нулю, то может быть любым числом
		m_command->setDataBytes(4);
		m_comBoxDataBytes->setCurrentText("4 байта");
	}
	else
		m_comBoxDataBytes->setCurrentText("8 байт");

	QLayoutItem* oldItemDataBytes = ui.formLayoutForCommand->replaceWidget(ui.spinBoxDataBytes, m_comBoxDataBytes);
	if (oldItemDataBytes) {
		delete oldItemDataBytes->widget();
		delete oldItemDataBytes;
	}
	bool ok = connect(m_comBoxDataBytes, SIGNAL(currentIndexChanged(int)), SLOT(slot_comBoxDataBytes_currentIndexChanged(int)));
	Q_ASSERT(ok);
	
}


void ModbusWidgetCommandCins::slot_comBoxDataBytes_currentIndexChanged(int index) {
	uint8_t data = m_comBoxDataBytes->itemData(index).toInt();
	m_command->setDataBytes(data);
}

void ModbusWidgetCommandCins::slot_comBoxDataOrder_currentIndexChanged(int index) {
	uint8_t data = m_comBoxDataOrder->itemData(index).toInt();
	m_command->setDataOrder(data);
}

void ModbusWidgetCommandCins::slot_comBoxUpdAlg_currentIndexChanged(int index) {
	uint8_t data = m_comBoxUpdAlg->itemData(index).toInt();
	m_command->setUpdAlg(data);	
}

void ModbusWidgetCommandCins::slot_sBoxMultiplicator_valueChanged(int value) {
	m_command->setMultilpicator(value);
}