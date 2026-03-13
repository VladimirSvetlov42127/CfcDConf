#include "TabSybusInitialization.h"
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qlabel.h>
#include <qdebug.h>

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/EditorInitCommands.h>
#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <gui/forms/interfaces_protocols/RS485/macrosCheckPointer.h>

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/definesSlaveMode.h>

using namespace Dpc::Gui;

TabSybusInitialization::TabSybusInitialization(DcController* controller, uint8_t port, uint8_t slaveCount, QWidget *parent)
	: QWidget(parent), m_slaveCount(slaveCount), m_port(port), m_controller(controller)
{	
	QVBoxLayout *vinitBox = new QVBoxLayout(this);

	QFormLayout *form = new QFormLayout;

	QHBoxLayout *hBoxForSlaveChoose = new QHBoxLayout;
	hBoxForSlaveChoose->addLayout(form);
	vinitBox->addLayout(hBoxForSlaveChoose);

	QLabel *labSlaveChoose = new QLabel("Выберите ведомого для настройки:");
	labSlaveChoose->setMinimumWidth(200);

	m_comBoxSlaveId = new QComboBox;
	m_comBoxSlaveId->setMinimumWidth(100);
	form->addRow(labSlaveChoose, m_comBoxSlaveId);

	for (int i = 0; i < m_slaveCount; i++)
		m_comBoxSlaveId->addItem("Ведомый " + QString::number(i+1));

	m_butChangeInit = new QPushButton("Редактировать команды инициализации");
	m_butChangeInit->setMaximumWidth(340);
	vinitBox->addWidget(m_butChangeInit);

	m_commandCount = new QLineEdit;
	m_commandCount->setEnabled(false);
	form->addRow(new QLabel("Количество команд "), m_commandCount);

	hBoxForSlaveChoose->addStretch(1);
	vinitBox->addStretch(1);

	bool ok;
	ok = connect(m_butChangeInit, SIGNAL(clicked()), SLOT(slot_butChandgeInit_clicked()));
	Q_ASSERT(ok);
	ok = connect(m_comBoxSlaveId, SIGNAL(currentIndexChanged(int)), SLOT(slot_slaveIdChanged(int)));
	Q_ASSERT(ok);

	calcExtraParams();
	m_commandCount->setText(QString::number(getCommandCount()));	

}

TabSybusInitialization::~TabSybusInitialization()
{

}

void TabSybusInitialization::setSlaveCount(uint8_t slaveCount)
{
	if (slaveCount == 0) {
		m_slaveCount = slaveCount;
		m_comBoxSlaveId->clear();
		return;
	}

	if (slaveCount > m_slaveCount) {	// Если увеличиваем в большу сторону, то просто добавляем
		for(int i = m_slaveCount; i < slaveCount; i++)
			m_comBoxSlaveId->addItem("Ведомый " + QString::number(i + 1));
	}
	else {	// Если стало меньше, обновляем все
		m_comBoxSlaveId->blockSignals(true);
		m_comBoxSlaveId->clear();
		for (int i = 0; i < slaveCount; i++)
			m_comBoxSlaveId->addItem("Ведомый " + QString::number(i + 1));
		m_comBoxSlaveId->blockSignals(false);
		m_comBoxSlaveId->setCurrentIndex(0);
	}

	m_slaveCount = slaveCount;	
}

void TabSybusInitialization::calcExtraParams()
{	
    auto setting = m_controller->setting("MASK_BYTES_COUNT");
	CHECK_ADDRESS(setting);
	m_bytesForChannelMasks = setting->value().toInt();	// Получаем значения кол-ва байт под маски каналов каждого типа


	// Определяем кол-ва слейвов на порту
    auto paramPorts = m_controller->paramsRegistry().parameter(SP_USARTPRTPAR_DWORD);
	if (!paramPorts) {	// Если устройство старое и не поддерживает cfg_v2
		MsgBox::error("Внимание! Вы используете старую версию прошивки и шаблона для данного устройства. Невозможно произвести необходимые вычисления. По умолчанию считается, что на один порт может быть настроено два слейва, если это не так, поведение не определено. Обновите устройство. ");
		m_maxSlaveCountForPort = 2;
		m_portDisplasement = m_port * m_maxSlaveCountForPort * 256;
		return;
	}

    uint8_t portCount = paramPorts->profilesCount();					// Высчитываем количество портов
    auto paramSlaveCount = m_controller->paramsRegistry().parameter(SP_USARTEXTNET_DEFINITION);
	CHECK_ADDRESS(paramSlaveCount);
    uint8_t maxSlaveCount = paramSlaveCount->profilesCount();						// Рассчитываем максимальное количество слейвов в данном контроллере

	if (portCount == 0) {
		MsgBox::error("Количество портов равно нулю!");
		return;
	}

	if (maxSlaveCount % portCount != 0) {
		MsgBox::error("Ошибка в делении количества слейвов на количество портов! Делится не нацело!");
		return;
	}
	m_maxSlaveCountForPort = maxSlaveCount / portCount;			// Рассчитываем кол-во ведомых на порте
	m_portDisplasement = m_port * m_maxSlaveCountForPort * 256;
}

bool TabSybusInitialization::calcStartInitIndex()
{
	if (!m_controller)
		return false;
	if (m_slaveCount == 0)
		return false;
	if (m_comBoxSlaveId->count() == 0)
		return false;
	
	uint16_t slaveInPortDisplasement = m_comBoxSlaveId->currentIndex() * 256;
	m_startSlaveInitIndex = m_portDisplasement + slaveInPortDisplasement + ns_slaveModeDefines::MASK_READ_ANALOG_INPUT + FLAG_COUNT * m_bytesForChannelMasks;	// Перемещаемся на индекс, с которого начинается запись команд инициализации

	return true;
}

uint8_t TabSybusInitialization::getCommandCount()
{
	if (!calcStartInitIndex())
		return 0;

    auto param = m_controller->paramsRegistry().element(SP_USARTEXTNET_DEFINITION, m_startSlaveInitIndex);
	if (!param)
		return 0;
	   
	return param->value().toInt();
}



void TabSybusInitialization::slot_slaveIdChanged(int id) {
	m_commandCount->setText(QString::number(getCommandCount()));	// Обновляем значение установленных команд
}

void TabSybusInitialization::slot_butChandgeInit_clicked() {

	if (m_comBoxSlaveId->count() == 0) {
		MsgBox::error("Нет доступных слейвов для настройки");
		return;
	}

	if (!calcStartInitIndex())	// Считаем начальный индекс для текущего слейва, сохраняем в m_startSlaveInitIndex
		return;

	EditorInitCommands test (m_controller, m_startSlaveInitIndex);
	test.exec();

	m_commandCount->setText(QString::number(getCommandCount()));	// Обновляем значение установленных команд
}
