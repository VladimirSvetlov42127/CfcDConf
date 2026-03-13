#pragma once

#include <QWidget>
#include "ui_ModbusWidgetCommonCommand.h"
#include <gui/forms/interfaces_protocols/RS485/Modbus/master/Commands/ModbusCommandDescription.h>

class ModbusWidgetCommonCommand : public QWidget
{
	Q_OBJECT

public:
	ModbusWidgetCommonCommand(spCommandDescr_t command, bool isModifyModbus, QWidget *parent = Q_NULLPTR);
	~ModbusWidgetCommonCommand();
	
	enum {	// Общие для всех константы
		COMMON_LEN = 150	// Длина поля данных максимальная
	};

	void updateFromFunctionInfo(spModbusFunc_t function);
	virtual void setDataBytes(uint8_t value) { ui.spinBoxDataBytes->setValue(value); };
	void setExpectedBcnt(uint8_t value) { ui.sBoxExpectedBcnt->setValue(value);  };
	void setAnalizedRspBcnt(uint8_t value) { ui.sBoxAnalizedRspBcnt->setValue(value); };
	void setExpectedFirstRspBcnt(QByteArray value);

protected:
	Ui::ModbusWidgetCommonCommand ui;
	spCommandDescr_t m_command;	// Команда, которую реализуем
	bool m_isModifyModbus;

protected:
	void setDataBytesVisible(bool isVisible);
	void setDataOrderVisible(bool isVisible);
	void setUpdAlgVisible(bool isVisible);
	void setExpectedBcntVisible(bool isVisible);
	void setAnalizedRspBcntVisible(bool isVisible);
	void setExpectedFirstRspBcntVisible(bool isVisible);


public slots:	// Общие для всех слоты
	void on_spinBoxDataBytes_valueChanged(int);
	void on_sBoxDataOrder_valueChanged(int);	// Может быть заменен на комбобокс
	void on_sBoxUpdAlg_valueChanged(int);	// Может быть заменен на комбобокс
	void on_sBoxExpectedBcnt_valueChanged(int);
	void on_sBoxAnalizedRspBcnt_valueChanged(int);
	void on_lEdExpectedFirstRspByte_editingFinished();

};
