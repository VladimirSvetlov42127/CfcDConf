#pragma once

#include <QDialog>

class ModbusDialogSlaveInir_readOrCreate : public QDialog
{
	Q_OBJECT

public:
	ModbusDialogSlaveInir_readOrCreate(QWidget* parent = 0);
	~ModbusDialogSlaveInir_readOrCreate();

	enum {
		loadConfig = 11,
		createConfig = 12
	};

private:	
	void makeWinget();
	virtual void closeEvent(QCloseEvent *e);

signals:
	void signal_modeChoosed(int);

public slots:
	void slot_loadChoosed();
	void slot_createChoosed();

};