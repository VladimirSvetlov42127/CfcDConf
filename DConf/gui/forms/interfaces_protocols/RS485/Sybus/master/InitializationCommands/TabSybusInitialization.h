#pragma once

#include <QWidget>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qlineedit.h>

#include "data_model/dc_controller.h"

class TabSybusInitialization : public QWidget
{
	Q_OBJECT

	enum {
		FLAG_COUNT = 8
	};

public:
	TabSybusInitialization(DcController* controller, uint8_t port, uint8_t slaveCount, QWidget *parent);
	~TabSybusInitialization();
	   
	void setSlaveCount(uint8_t slaveCount);

private:
	uint8_t m_port;
	uint8_t m_slaveCount;
	uint8_t m_maxSlaveCountForPort;
	uint16_t m_portDisplasement;
	uint8_t m_bytesForChannelMasks;
	DcController* m_controller;
	uint16_t m_startSlaveInitIndex;

	QComboBox *m_comBoxSlaveId;
	QPushButton *m_butChangeInit;
	QLineEdit *m_commandCount;

private:
	void calcExtraParams();
	bool calcStartInitIndex();
	uint8_t getCommandCount();

public slots:
	
	void slot_slaveIdChanged(int);
	void slot_butChandgeInit_clicked();

};
