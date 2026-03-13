#pragma once
#include <qcombobox.h>

#include <gui/forms/interfaces_protocols/RS485/Modbus/master/CommandWidgets/ModbusWidgetCommonCommand.h>
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandCins.h>

class ModbusWidgetCommandCins : public ModbusWidgetCommonCommand
{
	Q_OBJECT

public:
	ModbusWidgetCommandCins(spCommandDescr_t command, bool showAllParams = false, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetCommandCins();

	virtual void setDataBytes(uint8_t value) override;	// Переписываем функцию родителя, т.к. переделали представление 

private:
	QComboBox *m_comBoxDataBytes;
	QComboBox *m_comBoxDataOrder;
	QComboBox *m_comBoxUpdAlg;
	QSpinBox *m_sBoxMultiplicator;
	
	ModbusCommandCins *m_command;	//  Не удалять delete! Команда на конкретную команду, извлекается из spCommandDescr_t.

private:
	void replaceWidgets();

public slots:

	void slot_comBoxDataBytes_currentIndexChanged(int);
	void slot_comBoxDataOrder_currentIndexChanged(int);
	void slot_comBoxUpdAlg_currentIndexChanged(int);
	void slot_sBoxMultiplicator_valueChanged(int);
};
