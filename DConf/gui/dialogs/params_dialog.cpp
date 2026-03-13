#include "params_dialog.h"


//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QIcon>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
ParamsDialog::ParamsDialog(QList<NodeParam> params, QWidget* parent) : QDialog(parent)
{
	//	Свойства класса
	_params = params;
	_edit_list.clear();

	//	Формирование окна диалога
	setWindowTitle("Параметры элемента");
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowIcon(QIcon(":/icons/32/setting_1.png"));

	//	Создание кнопок
	QPushButton* ok_button = new QPushButton(QString("OK"));
	ok_button->setDefault(true);
	ok_button->setMinimumSize(80, 24);
	QPushButton* cancel_button = new QPushButton(QString("Отмена"));
	cancel_button->setDefault(false);
	cancel_button->setMinimumSize(80, 24);

	QHBoxLayout* button_layout = new QHBoxLayout();
	button_layout->addStretch();
	button_layout->addWidget(cancel_button);
	button_layout->addWidget(ok_button);

	//	Формирование виджета
	QVBoxLayout* main_layout = new QVBoxLayout;
	for (int i = 0; i < _params.count(); i++) {
        NodeParam param = _params.at(i);
		QHBoxLayout* param_layout = new QHBoxLayout;
		QLabel* name_label = new QLabel(this);
		name_label->setText(param.name);
		param_layout->addWidget(name_label);
	
		QLineEdit* value_edit = new QLineEdit(this);
		value_edit->setMinimumWidth(50);
		value_edit->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
		value_edit->setText(param.value.toString());
		param_layout->addWidget(value_edit);
		_edit_list.append(value_edit);
		
		main_layout->addLayout(param_layout); }
	main_layout->addStretch();
	main_layout->addLayout(button_layout);
	setLayout(main_layout);

	//	Обработка сигналов
	connect(ok_button, &QPushButton::clicked, this, &ParamsDialog::OkClicked);
	connect(cancel_button, &QPushButton::clicked, this, &ParamsDialog::reject);
}

ParamsDialog::~ParamsDialog()
{
}


//===================================================================================================================================================
//	Обработка сигналов формы
//===================================================================================================================================================
void ParamsDialog::OkClicked()
{
	for (int i = 0; i < _edit_list.count(); i++) _params[i].value = _edit_list.at(i)->text();
	this->accept();
}
