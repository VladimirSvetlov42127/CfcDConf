#pragma once

#include <QWidget>
#include "ui_InitSybusCommandSetParam.h"
#include <qbytearray.h>
#include <qvector.h>
#include <qlineedit.h>

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/init_commands_defines.h>
#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/InterfaseInitCommand.h>
  
class InitSybusCommandSetParam : public InterfaseInitCommand
{
	Q_OBJECT

	enum {
		LEN_POS =	0,	// Байт длины не считается!
		CODE_POS =				1,
		PARAM_NUMBER_POS =		2,	// 2 байта
		PARAM_TYPE_POS =		4,
		COUNT_POS =				5,
		PARAM_INDEX_POS =		6,	// 2 байта
		DATA_POS =				8,
	
		MIN_COMMAND_LEN = 7	// Без учета байтов данных
	};



public:
	InitSybusCommandSetParam(const QByteArray &commandArray, QWidget *parent = Q_NULLPTR);
	virtual ~InitSybusCommandSetParam();

	virtual QByteArray getCommandByteArray() const;

private:
	Ui::InitSybusCommandSetParam ui;

	const QByteArray &m_initCommandBuf;
	uint8_t m_len;
	uint8_t m_code;
	uint16_t m_address;
	uint8_t m_type;
	uint8_t m_count;
	uint16_t m_index;
	QVector<QByteArray> mv_dataByteArray;

	QVector<QLineEdit*> mv_dataFields;

private:
	bool setCustomDataToWidget();
	bool setDefaultValues();
	bool getConfigFromBuf();
	bool checkDefaultValuesFromBuf();

	void changeCountOfDataFields(uint16_t newCount);
	void displayData();
	void updateDataInIndex(uint16_t index);

private slots:
	void slot_lEdParamNumber_editingFinished();
	void slot_spBoxParamIndex_valueChanged(int);
	void slot_comBoxParamType_currentIndexChanged(int);
	void slot_spBoxCount_valueChanged(int);
	void slot_lEdParamData_editingFinished();

	void slot_ParamData_editingFinished();
};
