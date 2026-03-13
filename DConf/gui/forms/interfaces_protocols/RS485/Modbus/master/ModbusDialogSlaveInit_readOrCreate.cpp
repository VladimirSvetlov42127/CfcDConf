#include "ModbusDialogSlaveInit_readOrCreate.h"

#include "qboxlayout.h"
#include "qpushbutton.h"
#include "qlabel.h"


ModbusDialogSlaveInir_readOrCreate::ModbusDialogSlaveInir_readOrCreate(QWidget* parent) 
	: QDialog(parent)
{
	makeWinget();
}

ModbusDialogSlaveInir_readOrCreate::~ModbusDialogSlaveInir_readOrCreate() {
}


void ModbusDialogSlaveInir_readOrCreate::makeWinget() {

    setWindowIcon(QIcon(":/icons/info.svg"));
	setWindowTitle("Выбор загрузки конфигурации слейва");
	setWindowFlag(Qt::WindowStaysOnTopHint);

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget(new QLabel("Выберите, загрузить сохраненную конфигурацию или создать новую"));

	QBoxLayout* hbox = new QHBoxLayout;					// Слой для кнопок
	vbox->addLayout(hbox);

	QPushButton* butLoad = new QPushButton("Загрузить сохраненную");
	//connect(butLoad, SIGNAL(clicked()), SLOT(done(loadConfig)));
	bool ok = connect(butLoad, SIGNAL(clicked()), SLOT(slot_loadChoosed()));
	Q_ASSERT(ok);
	hbox->addWidget(butLoad);

	QPushButton* butCreate = new QPushButton("Создать новую");
	ok = connect(butCreate, SIGNAL(clicked()), SLOT(slot_createChoosed()));
	Q_ASSERT(ok);
	//connect(butCreate, SIGNAL(clicked()), SLOT(accept()));
	hbox->addWidget(butCreate);

	setLayout(vbox);

	ok = connect(this, SIGNAL(signal_modeChoosed(int)), SLOT(done(int)));
	Q_ASSERT(ok);
}

void ModbusDialogSlaveInir_readOrCreate::closeEvent(QCloseEvent * e)
{
	emit rejected();
	QDialog::closeEvent(e);
}


void ModbusDialogSlaveInir_readOrCreate::slot_loadChoosed() {
	emit signal_modeChoosed(loadConfig);
}

void ModbusDialogSlaveInir_readOrCreate::slot_createChoosed() {
	emit signal_modeChoosed(createConfig);
}
