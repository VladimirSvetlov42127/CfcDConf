#include "ModbusDialogSlaveInit_standartOrNot.h"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>

ModbusDialogSlaveInit_standartOrNot::ModbusDialogSlaveInit_standartOrNot(QWidget *parent)
	: QDialog(parent)
{
    setWindowIcon(QIcon(":/icons/info.svg"));
	setWindowTitle("Выбор загрузки конфигурации слейва");
	setWindowFlag(Qt::WindowStaysOnTopHint);

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(new QLabel("Выберите, создать ли стандартный модбас или модифицированный"));
	vbox->addWidget(new QLabel("Стандартный позволит заполнить некоторые поля автоматически, а модифицированный предоставит полный достпу к настройке без проверок"));

	QBoxLayout* hbox = new QHBoxLayout;					// Слой для кнопок
	vbox->addLayout(hbox);

	bool ok;
	QPushButton* butLoad = new QPushButton("Стандартный");
	ok = connect(butLoad, SIGNAL(clicked()), SLOT(slot_standartChoosed()));
	Q_ASSERT(ok);
	hbox->addWidget(butLoad);

	QPushButton* butCreate = new QPushButton("Специализированный");
	ok = connect(butCreate, SIGNAL(clicked()), SLOT(slot_modifyChoosed()));
	Q_ASSERT(ok);
	hbox->addWidget(butCreate);
	setLayout(vbox);

	ok = connect(this, SIGNAL(signal_modeChoosed(int)), SLOT(done(int)));
	Q_ASSERT(ok);

}

ModbusDialogSlaveInit_standartOrNot::~ModbusDialogSlaveInit_standartOrNot()
{
}



void ModbusDialogSlaveInit_standartOrNot::slot_standartChoosed() {
	emit signal_modeChoosed(standartModbus);
}

void ModbusDialogSlaveInit_standartOrNot::slot_modifyChoosed() {
	emit signal_modeChoosed(modifyModbus);
}
