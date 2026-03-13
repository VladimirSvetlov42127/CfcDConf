#pragma once

#include <QDialog>

class ModbusDialogSlaveInit_standartOrNot : public QDialog
{
	Q_OBJECT

public:
	ModbusDialogSlaveInit_standartOrNot(QWidget *parent = nullptr);
	~ModbusDialogSlaveInit_standartOrNot();

	enum {
		standartModbus = 11,
		modifyModbus = 12
	};

signals:
	void signal_modeChoosed(int);

public slots:
	void slot_standartChoosed();
	void slot_modifyChoosed();
};
