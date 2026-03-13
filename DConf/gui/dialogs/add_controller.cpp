#include "add_controller.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QGridLayout>
#include <QPushButton>
#include <QTextStream>
#include <QDebug>

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "config_deprotec.h"
#include "device_templates/dc_templates_manager.h"

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
AddController::AddController(QWidget* parent)
    : QDialog(parent)
    , _controllers_list{ new QListView(this) }
    , _is_C1{false}
    , _is_AC{false}
    , _is_deprotec{false}
{
    //	Создание формы
    this->setWindowTitle(QString("Выбор устройства"));
    this->setMinimumSize(QSize(400, 300));
    this->setMaximumSize(QSize(400, 300));
    this->setWindowIcon(QIcon(":/icons/table_+.svg"));
    this->setModal(false);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

	auto select_button = new QPushButton(this);
	select_button->setText(QString("Выбор"));
	select_button->setMinimumSize(QSize(75, 25));
	select_button->setMaximumSize(QSize(75, 25));
	select_button->setDefault(true);    
	auto cancel_button = new QPushButton(this);
	cancel_button->setText(QString("Отмена"));
	cancel_button->setMinimumSize(QSize(75, 25));
	cancel_button->setMaximumSize(QSize(75, 25));

    auto main_layout = new QGridLayout(this);
	main_layout->addWidget(_controllers_list, 0, 0, 1, 3);
	main_layout->addWidget(select_button, 1, 1);
    main_layout->addWidget(cancel_button, 1, 2);

    //	Заполнение списка контроллеров
    auto model = new QStringListModel{gTemplatesManager.deviceTemplates().keys(), _controllers_list};
    _controllers_list->setModel(model);
    _controllers_list->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//	Обработка сигналов
	connect(cancel_button, &QPushButton::clicked, this, &AddController::reject);
	connect(select_button, &QPushButton::clicked, this, &AddController::SelectPressed);
	connect(_controllers_list, &QListView::doubleClicked, this, &AddController::SelectPressed);
}

AddController::~AddController()
{
}

//===================================================================================================================================================
//	Слоты
//===================================================================================================================================================
void AddController::SelectPressed()
{
	//	Проверка выбранного устройства
    auto selectedIndex = _controllers_list->selectionModel()->selectedRows().value(0);
    if (!selectedIndex.isValid()) {
		Dpc::Gui::MsgBox::error(QString("Не выбрано значение из списка"));
        return;
    }

    m_deviceType = selectedIndex.data().toString();
    if (m_deviceType.isEmpty()) {
        Dpc::Gui::MsgBox::error(QString("Выбрано пустое значение"));
        return;
    }

    m_deviceTemplateFilePath = gTemplatesManager.deviceTemplates()[m_deviceType];
	_is_deprotec = false;
    _is_C1 = false;
    _is_AC = false;

	//	Выбор типа Deprotec
    if (m_deviceType.contains("DEPROTEC")) {
        ConfigDeprotec deprotecDlg(gTemplatesManager.deprotecModifications());
        if (QDialog::Accepted != deprotecDlg.exec())
            return;

        _is_deprotec = true;
        m_deviceTemplateFilePath = deprotecDlg.Path();
        m_deviceType = deprotecDlg.DeprotecName();
        _is_C1 = deprotecDlg.IsC1();
    }

    accept();
}
