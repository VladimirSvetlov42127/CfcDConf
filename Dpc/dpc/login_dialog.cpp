#include "login_dialog.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QMessageBox>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
//#include "dep_user_roles.h"
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Namespaces и глобальные переменные
//===================================================================================================================================================
using namespace Dpc::Gui;

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent)
{
	//	Установка параметров
	//_settings = settings;
	//if (!_settings.contains(login_password)) _settings.insert(login_password, QString(""));
	//if (!_settings.contains(login_login)) _settings.insert(login_login, QString(""));
	//if (!_settings.contains(user_role)) _settings.insert(user_role, NO_ROLE);

	//	Формирование вида окна диалога
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QString("Авторизация в устройстве"));

	//	Создание полей редактирования
	_login_edit = new QLineEdit;
	_login_edit->setMaximumWidth(150);
	_login_edit->setMaxLength(16);

	_password_edit = new QLineEdit;
	_password_edit->setMaximumWidth(150);
	_password_edit->setMaxLength(16);
	_password_edit->setPlaceholderText(QString("Пароль"));
	_password_edit->setEchoMode(QLineEdit::Password);

	//	Создание кнопок
	QPushButton* login_button = new QPushButton(QString("Войти"));
	login_button->setDefault(true);
	login_button->setMinimumSize(80, 24);
	QPushButton* exit_button = new QPushButton(QString("Отмена"));
	exit_button->setMinimumSize(80, 24);

	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addStretch();
	button_layout->addWidget(exit_button);
	button_layout->addWidget(login_button);

	//	Вывод формы с текстом
	QGridLayout* grid_layout = new QGridLayout;
	grid_layout->setContentsMargins(5, 5, 5, 5);
	grid_layout->addWidget(new QLabel("Имя пользователя:"), 0, 0);
	grid_layout->addWidget(_login_edit, 0, 1);
	grid_layout->addWidget(new QLabel("Пароль:"), 1, 0);
	grid_layout->addWidget(_password_edit, 1, 1);

	//	Вывод основной формы
	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addLayout(grid_layout);
	main_layout->addStretch();
	main_layout->addLayout(button_layout);
	this->setMinimumSize(250, 125);
	this->setMaximumSize(250, 125);

	//	Обработка сигналов
	connect(exit_button, SIGNAL(clicked()), this, SLOT(reject()));
	connect(login_button, SIGNAL(clicked()), this, SLOT(LoginClicked()));

	return;
}

LoginDialog::~LoginDialog()
{
}


//===================================================================================================================================================
//	Слоты
//===================================================================================================================================================
void LoginDialog::LoginClicked()
{
	//	Проверка поля логин
	if (_login_edit->text().isEmpty()) {
		MsgBox::error(QString("Не задано имя пользователя."));
		return;	}

	//	Проверка поля пароля
	if (_password_edit->text().isEmpty()) {
		MsgBox::error(QString("Не задан пароль."));
		return; }
	this->accept();

	return;
}


