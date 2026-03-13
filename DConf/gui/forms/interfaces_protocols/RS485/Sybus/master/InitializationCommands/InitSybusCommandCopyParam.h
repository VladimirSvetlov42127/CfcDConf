#pragma once

#include <QWidget>
#include "ui_InitSybusCommandCopyParam.h"
#include <qbytearray.h>

#include <gui/forms/interfaces_protocols/RS485/Sybus/master/InitializationCommands/InterfaseInitCommand.h>

class InitSybusCommandCopyParam : public InterfaseInitCommand
{
	Q_OBJECT

	enum {
		LEN_POS = 0,
		CODE_POS = 1,
		PARAM_SOURSE_NUMBER_POS = 2,	// 2 байта
		PARAM_SOURSE_INDEX_POS = 4,	// 2 байта
		COUNT_POS = 6,
		PARAM_DEST_NUMBER_POS = 7,	// 2 байта
		PARAM_DEST_INDEX_POS = 9,	// 2 байта

		FIX_COMMAND_LEN = 11	// С учетом длины, которая не учитывается при записи данных в LEN_POS
	};

public:
	InitSybusCommandCopyParam(const QByteArray &commandArray, QWidget *parent = Q_NULLPTR);
	~InitSybusCommandCopyParam();

	virtual QByteArray getCommandByteArray() const;

private:
	Ui::InitSybusCommandCopyParam ui;
	const QByteArray &m_initCommandBuf;	// Для загрузки из модели

	uint8_t m_len;
	uint16_t m_sourseAddress;
	uint16_t m_sourseIndex;
	uint8_t m_count;
	uint16_t m_destAddress;
	uint16_t m_destIndex;
	
private:
	bool setDefaultValues();
	bool getConfigFromBuf();
	bool checkDefaultValuesFromBuf();

private slots:
	void slot_lEdSourseAddress_editingFinished();
	void slot_spBoxSourseIndex_valueChanged(int);
	void slot_spBoxCount_valueChanged(int);
	void slot_lEdDestAddress_editingFinished();
	void slot_spBoxDestIndex_valueChanged(int);
};
