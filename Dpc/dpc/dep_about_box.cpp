#include "dep_about_box.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QPixmap>

namespace {

QPixmap loadPixmap(const QString &fileName, const QSize &size)
{
    QPixmap pix(fileName);
    return pix.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

}

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DepAboutBox::DepAboutBox(DepSettings settings, QWidget* parent) : QDialog(parent)
{
	//	Параметры настроек окна
	_settings = settings;

	//	Формирование вида окна диалога
	this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	this->setWindowTitle(QString("О программе"));
    this->setWindowIcon(QPixmap(":/icons/info.svg"));

	//	Формирование элементов для макета с пиктограммами
	QLabel* qr_code = new QLabel;
    qr_code->setPixmap(loadPixmap(":/icons/QR1.svg", QSize(64, 64)));
	QLabel* dep_logo = new QLabel;
    dep_logo->setPixmap(loadPixmap(":/icons/dep_logo_o.svg", QSize(64, 64)));
	QLabel* icon = new QLabel;
    icon->setPixmap(loadPixmap(_settings[about_icon].toString(), QSize(32, 32)));
	QLabel* name = new QLabel(_settings[about_name].toString());
	QLabel* version = new QLabel(QString("Версия: ") + _settings[about_version].toString());

	//  Формирование макета с пиктограммами
	QGridLayout* grid_layout = new QGridLayout;
	grid_layout->addWidget(qr_code, 0, 0, 4, 1, Qt::AlignLeft);
	grid_layout->addWidget(dep_logo, 0, 1, 4, 1, Qt::AlignVCenter | Qt::AlignHCenter);
	grid_layout->addWidget(icon, 1, 2, 2, 1, Qt::AlignRight);
	grid_layout->addWidget(name, 1, 3, Qt::AlignVCenter | Qt::AlignLeft);
	grid_layout->addWidget(version, 2, 3, Qt::AlignVCenter | Qt::AlignLeft);
	grid_layout->setColumnStretch(1, 1);
	grid_layout->setContentsMargins(3, 1, 3, 1);

	//  Добавление текстового сообщения
	QLabel* message = new QLabel(_settings[about_message].toString());
	message->setContentsMargins(3, 10, 3, 10);
	message->setTextFormat(Qt::AutoText);
	message->setWordWrap(true);
	message->setStyleSheet("QLabel {border: 2px solid lightgrey;}");

	//	Формирование элементов для техподдержки
	QLabel* tech_support = new QLabel("Техподдержка");
	tech_support->setContentsMargins(3, 1, 3, 1);
	QLabel* phone = new QLabel("Тел. +7 (495) 995-00-12 (доб. 241)");
	QLabel* mail = new QLabel("E-mail:");
	QLabel* address = new QLabel("support@dep.ru");
	address->setStyleSheet("QLabel {font: underline; color: blue;}");

	//	Формирование макета для техподдержки
	QHBoxLayout* support_layout = new QHBoxLayout;
	support_layout->addWidget(phone);
	support_layout->addStretch();
	support_layout->addWidget(mail);
	support_layout->addWidget(address);
	support_layout->setContentsMargins(3, 1, 3, 1);

	//	Формирование макета для вывода кнопки
	QPushButton* close_button = new QPushButton(QString("Закрыть"));
	close_button->setMinimumSize(80, 24);
	close_button->setMaximumSize(80, 24);

	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addStretch();
	button_layout->addWidget(close_button);
	button_layout->setContentsMargins(3, 10, 3, 3);

	//	Формирование основной формы
	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addLayout(grid_layout);
	main_layout->addWidget(message);
	main_layout->addWidget(tech_support);
	main_layout->addLayout(support_layout);
	main_layout->addStretch();
	main_layout->addLayout(button_layout);

	//	Фиксация размера окна
	this->setMinimumSize(400, 270);
	this->setMaximumSize(400, 270);

	//	Обработка сигналов
	connect(close_button, SIGNAL(clicked()), this, SLOT(reject()));

	return;
}

DepAboutBox::~DepAboutBox()
{
}
