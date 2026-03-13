#include "log_settings_dialog.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "dpc/dep_settings.h"

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
LogSettingsDialog::LogSettingsDialog(uint8_t filter, QWidget* parent) : QDialog(parent)
{
	//	Инициализация свойств класса
	_filter = filter;

	//	Окно формы
	//	Формирование вида окна диалога
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QString("Настройка параметров логирования"));

	//	Создание шаблона с параметрами
	_info_checkbox = new QCheckBox("Вывод информационных сообщений");
	_warning_checkbox = new QCheckBox("Вывод Предупреждений");
	_error_checkbox = new QCheckBox("Вывод сообщений об ошибках");
	_debug_checkbox = new QCheckBox("Вывод отладочной информации");

	//	Создание кнопок
	QPushButton* login_button = new QPushButton(QString("Применить"));
	login_button->setDefault(true);
	login_button->setMinimumSize(80, 24);
	QPushButton* exit_button = new QPushButton(QString("Отмена"));
	exit_button->setMinimumSize(80, 24);

	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addStretch();
	button_layout->addWidget(exit_button);
	button_layout->addWidget(login_button);

	//	Вывод основной формы
	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addWidget(_info_checkbox);
	main_layout->addWidget(_warning_checkbox);
	main_layout->addWidget(_error_checkbox);
	main_layout->addWidget(_debug_checkbox);
	main_layout->addStretch();
	main_layout->addLayout(button_layout);

	//	Установка параметров логирования
	//(_filter & INFORMATION_LOG) == 0 ? _info_checkbox->setChecked(false) : _info_checkbox->setChecked(true);
	//(_filter & WARNING_LOG) == 0 ? _warning_checkbox->setChecked(false) : _warning_checkbox->setChecked(true);
	//(_filter & ERROR_LOG) == 0 ? _error_checkbox->setChecked(false) : _error_checkbox->setChecked(true);
	//(_filter & DEBUG_LOG) == 0 ? _debug_checkbox->setChecked(false) : _debug_checkbox->setChecked(true);

	this->setMinimumSize(265, 165);
	this->setMaximumSize(265, 165);

	//	Обработка сигналов
	connect(exit_button, SIGNAL(clicked()), this, SLOT(reject()));
	connect(login_button, SIGNAL(clicked()), this, SLOT(EnterClicked()));

	connect(_info_checkbox, SIGNAL(stateChanged(int)), this, SLOT(InformationCheck(int)));
	connect(_warning_checkbox, SIGNAL(stateChanged(int)), this, SLOT(WarningCheck(int)));
	connect(_error_checkbox, SIGNAL(stateChanged(int)), this, SLOT(ErrorCheck(int)));
	connect(_debug_checkbox, SIGNAL(stateChanged(int)), this, SLOT(DebugCheck(int)));

	return;
}

LogSettingsDialog::~LogSettingsDialog()
{
	return;
}


//===================================================================================================================================================
//	Обработка событий формы
//===================================================================================================================================================
void LogSettingsDialog::EnterClicked()
{
	this->accept();
	return;
}

void LogSettingsDialog::InformationCheck(int state)
{
	//state == Qt::Checked ? _filter |= INFORMATION_LOG : _filter  &= ~(INFORMATION_LOG);
}

void LogSettingsDialog::WarningCheck(int state)
{
	//state == Qt::Checked ? _filter |= WARNING_LOG : _filter &= ~(WARNING_LOG);
}

void LogSettingsDialog::ErrorCheck(int state)
{
	//state == Qt::Checked ? _filter |= ERROR_LOG : _filter &= ~(ERROR_LOG);
}

void LogSettingsDialog::DebugCheck(int state)
{
	//state == Qt::Checked ? _filter |= DEBUG_LOG : _filter &= ~(DEBUG_LOG);
}
