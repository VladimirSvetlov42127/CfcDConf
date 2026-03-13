#include "config_deprotec.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QStringList>
#include <QIcon>
#include <QGroupBox>
#include <QPushButton>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFont>



//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================

//===================================================================================================================================================
//	Параметры модулей DeProtec
//===================================================================================================================================================
const QStringList BodyNames = { "KV", "KW" };												//	Модификация крейта
const QStringList PanelNames = { "C0", "C1" };												//	Вид передней панели
const QStringList PowerNames = { "100AC", "110DC", "230AC", "220DC" };						//	Вид оперативного тока энергообъекта

const QStringList ACinputs_1 = { "5/5/100", "5/1/100", "1/1/100" };							//	DRZA-AFE
const QStringList ACinputs_2 = { "5:5/5", "5:5/1", "1:5/5", "1:5/1", "1:1/1" };				//	DRZA-AFE7
const QStringList ACinputs_3 = { "5:5:5/5:5", "1:5:5/5:5", "1:5:5/1:1", "1:1:1/1:1" };		//	DRZA-AFE11
const QStringList ACinputs_4 = { "5:5/5/100", "1:5/1/100", "5:1/1/100" };					//	DRZA-AFE47

const QStringList modules_names_1 = { "1 - DRZA-AFE", "2 - DRZA-AFE7", "3 - DRZA-AFE11", "4 - DRZA-AFE47" };
const QStringList modules_names_2 = { "0", "1 - DRZA-DIO", "3 - DRZA-EM3", "4 - DRZA-2TX-3FS", "5 - DRZA-2FX-3FS", "PWC" };
const QStringList modules_names_3 = { "0", "1 - DRZA-DIO", "3 - DRZA-EM3", "6 - DRZA-KEY", "7 - DRZA-R2IN8 " };
const QStringList modules_names_4 = { "0", "1 - DRZA-DIO", "3 - DRZA-EM3" };
const QStringList modules_names_5 = { "0", "1 - DRZA-DIO", "3 - DRZA-EM3" };


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
ConfigDeprotec::ConfigDeprotec(const QMap<QString, QString> &config, QWidget *parent) : QDialog(parent)
{
	//	Свойства класса
	_config = config;

	//	Формирование окна диалога
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QString("Выбор модификации DeProtec"));
	this->setWindowIcon(QIcon(":/icons/dep_deprotec.svg"));

	//	Формирование макеты выбора корпуса
	QHBoxLayout* body_layout = new QHBoxLayout;
	QLabel* body_label = new QLabel("Корпус: ");
	_body_list = new QComboBox(this);
	_body_list->addItems(BodyNames);
	_body_list->setCurrentIndex(0);
	QLabel* panel_label = new QLabel("Лицевая панель: ");
	_panels_list = new QComboBox(this);
	_panels_list->addItems(PanelNames);
	_panels_list->setCurrentIndex(0);

	body_layout->addWidget(body_label);
	body_layout->addWidget(_body_list);
	body_layout->addStretch();
	body_layout->addWidget(panel_label);
	body_layout->addWidget(_panels_list);

	//	Создание макета модулей
	QGridLayout* grid_layout = new QGridLayout;
	QGroupBox* controller_group = new QGroupBox;
	controller_group->setStyleSheet("QGroupBox { border: 1px solid grey; border-radius: 1px}");
	controller_group->setLayout(grid_layout);

	//	Слоты расширения
	QLabel* ps_label = new QLabel("Слот PS:");
	_power_list = new QComboBox(this);
	_power_list->addItems(PowerNames);
	_power_list->setCurrentIndex(0);

	QLabel* slot_one = new QLabel("Слот S1:");
	QLabel* slot_two = new QLabel("Слот S2:");
	QLabel* slot_three = new QLabel("Слот S3:");
	_label_4 = new QLabel("Слот S4:");
	_label_4->setVisible(false);
	_label_5 = new QLabel("Слот S5:");
	_label_5->setVisible(false);

	_slot_1 = new QComboBox(this);
	_slot_1->addItems(modules_names_1);
	_slot_1->setCurrentIndex(0);
	_slot_2 = new QComboBox(this);
	_slot_2->addItems(modules_names_2);
	_slot_2->setCurrentIndex(0);
	_slot_3 = new QComboBox(this);
	_slot_3->addItems(modules_names_3);
	_slot_3->setCurrentIndex(0);
	_slot_4 = new QComboBox(this);
	_slot_4->addItems(modules_names_4);
	_slot_4->setCurrentIndex(0);
	_slot_4->setVisible(false);
	_slot_5 = new QComboBox(this);
	_slot_5->addItems(modules_names_5);
	_slot_5->setCurrentIndex(0);
	_slot_5->setVisible(false);

	_input_list = new QComboBox(this);
	_input_list->addItems(ACinputs_1);
	_input_list->setCurrentIndex(0);

	//	Формирование макета модулей
	grid_layout->addWidget(ps_label, 0, 0);
	grid_layout->addWidget(_power_list, 0, 2);
	grid_layout->setRowMinimumHeight(1, 10);
	grid_layout->addWidget(slot_one, 2, 0);
	grid_layout->addWidget(_slot_1, 2, 2);
	grid_layout->addWidget(_input_list, 2, 4);
	grid_layout->addWidget(slot_two, 3, 0);
	grid_layout->addWidget(_slot_2, 3, 2);
	grid_layout->addWidget(slot_three, 4, 0);
	grid_layout->addWidget(_slot_3, 4, 2);
	grid_layout->setRowMinimumHeight(5, 10);
	grid_layout->addWidget(_label_4, 6, 0);
	grid_layout->addWidget(_slot_4, 6, 2);
	grid_layout->addWidget(_label_5, 7, 0);
	grid_layout->addWidget(_slot_5, 7, 2);

	//	Формирование макета кнопок
	QHBoxLayout* buttons_layout = new QHBoxLayout;
	QPushButton* cancel_button = new QPushButton(QString("Отмена"));
	cancel_button->setMinimumSize(80, 24);
	cancel_button->setMaximumSize(80, 24);
	_select_button = new QPushButton(QString("Выбор"));
	_select_button->setDefault(true);
	_select_button->setMinimumSize(80, 24);
	_select_button->setMaximumSize(80, 24);

	buttons_layout->addWidget(cancel_button);
	buttons_layout->addStretch();
	buttons_layout->addWidget(_select_button);


	//	Название контроллера
	_device_name = new QLabel(this);
	_device_name->setContentsMargins(5,5,5,5);
	QFont name_font = _device_name->font();
	name_font.setBold(true);
	name_font.setPointSize(name_font.pointSize() + 2);
	_device_name->setFont(name_font);
	_device_name->setStyleSheet("QLabel { border: 2px solid lightgrey; border-radius: 5px}");
	_device_name->setText(DeprotecName());

	_error_label = new QLabel("Данная конфигурация не поддерживается");
	_error_label->setStyleSheet("QLabel {color: red;}");
	_error_label->setVisible(false);

	//	Формирование формы
	QGridLayout* main_layout = new QGridLayout(this);
	main_layout->addWidget(_device_name, 0, 0);
	main_layout->addWidget(_error_label, 1, 0);
	main_layout->addLayout(body_layout, 2, 0);
	main_layout->addWidget(controller_group, 3, 0);
	main_layout->addLayout(buttons_layout, 5, 0);

	main_layout->setRowStretch(main_layout->rowCount() - 2, 1);
	main_layout->setColumnStretch(main_layout->columnCount() - 1, 1);

	//	Обработка сигналов
	connect(_select_button, SIGNAL(clicked()), this, SLOT(accept()));
	connect(cancel_button, SIGNAL(clicked()), this, SLOT(reject()));
	connect(_body_list, SIGNAL(currentIndexChanged(int)), this, SLOT(ShowSlots(int)));
	connect(_panels_list, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));
	connect(_power_list, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));
	connect(_input_list, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));
	connect(_slot_1, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotOneChanged(int)));
	connect(_slot_2, SIGNAL(currentIndexChanged(int)), this, SLOT(SlotTwoChanged()));
	connect(_slot_3, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));
	connect(_slot_4, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));
	connect(_slot_5, SIGNAL(currentIndexChanged(int)), this, SLOT(ChangeData()));

	return;
}

ConfigDeprotec::~ConfigDeprotec()
{
}


//===================================================================================================================================================
//	Вывод параметров устройства
//===================================================================================================================================================
QString ConfigDeprotec::DeprotecName()
{
	return QString("DeProtec-LT-") + _body_list->currentText() + "-" + _input_list->currentText() + "-" + _power_list->currentText() + "-" + Modules() + "-" + _panels_list->currentText();
}

QString ConfigDeprotec::Modules()
{
	//return QString("1-PWC");
	QString modules = QString(_slot_1->currentText()).at(0) + QString("-");
	_slot_2->currentText() == "PWC" ? modules += _slot_2->currentText() : modules += QString(_slot_2->currentText()).at(0);
	if (_slot_2->currentText() != "PWC") modules += QString("-") + QString(_slot_3->currentText()).at(0);
	if (_body_list->currentIndex() > 0) modules += QString("-") + QString(_slot_4->currentText()).at(0) + QString("-") + QString(_slot_5->currentText()).at(0);

	return modules;
}

bool ConfigDeprotec::IsC1()
{
	if (_panels_list->currentIndex() == 1) return true;
	return false;
}


//===================================================================================================================================================
//	Методы обработки сигналов формы
//===================================================================================================================================================
void ConfigDeprotec::ChangeData()
{
	_device_name->setText(DeprotecName());
	CheckConfig();
}

void ConfigDeprotec::ShowSlots(int modification)
{
	bool visible = false;
	if (modification == 1) visible = true;
	_label_4->setVisible(visible);
	_label_5->setVisible(visible);
	_slot_4->setVisible(visible);
	_slot_5->setVisible(visible);
	_device_name->setText(DeprotecName());
	ChangeData();

	return;
}

void ConfigDeprotec::SlotOneChanged(int index)
{
	_input_list->clear();
	switch (index) {
	case 0:	_input_list->addItems(ACinputs_1); break;
	case 1:	_input_list->addItems(ACinputs_2); break;
	case 2:	_input_list->addItems(ACinputs_3); break;
	case 3: _input_list->addItems(ACinputs_4); break; };
	ChangeData();

	return;
}

void ConfigDeprotec::SlotTwoChanged()
{
	if (_slot_2->currentText() == "PWC") {
		_slot_3->clear();
		_slot_3->setEnabled(false);	}
	if (_slot_2->currentText() != "PWC" && _slot_3->isEnabled() == false) {
		_slot_3->setEnabled(true);
		_slot_3->addItems(modules_names_3);	}
	ChangeData();

	return;
}


//===================================================================================================================================================
//  Вспомогательные методы класса
//===================================================================================================================================================
void ConfigDeprotec::CheckConfig()
{
	if (_config.contains(Modules())) {
		_select_button->setEnabled(true);
		_device_name->setStyleSheet("QLabel { border: 2px solid lightgrey; border-radius: 5px; color: black;}");
		_error_label->setVisible(false);
	} else {
		_select_button->setEnabled(false);
		_device_name->setStyleSheet("QLabel { border: 2px solid lightgrey; border-radius: 5px; color: red;}"); 
		_error_label->setVisible(true);	}

	return;
}

