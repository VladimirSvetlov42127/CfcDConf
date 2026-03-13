#include "binding_dialog.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStandardItem>
#include <QModelIndexList>
#include <QModelIndex>


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
BindingDialog::BindingDialog(uint8_t type, ServiceManager* service_manager, QWidget* parent) : QDialog(parent)
{
	//	Свойства класса
	_selected_index = -1;
    _service_manager = service_manager;

	//	Формирование вида окна диалога
    setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
	setWindowTitle(QString("Выбор сигнала для привязки"));
    setMinimumHeight(500);
    QVBoxLayout* main_layout = new QVBoxLayout(this);

	//	Формирование дерева выбора сигналов
    _tree_view = new QTreeView;
    _tree_view->collapseAll();
    _tree_view->setHeaderHidden(true);
    _tree_view->setIconSize(QSize(20, 20));
	
	//	Формирование модели данных
    _model = new QStandardItemModel(this);
    setModel(type);
    _tree_view->setModel(_model);

    //	Формирование кнопок
    QPushButton* enter_button = new QPushButton(QString("Выбрать"));
    enter_button->setDefault(true);
    QPushButton* cancel_button = new QPushButton(QString("Отмена"));
    cancel_button->setDefault(false);

    QHBoxLayout* control_layout = new QHBoxLayout;
    control_layout->addStretch();
    control_layout->addWidget(enter_button);
    control_layout->addWidget(cancel_button);

    //	Формирование диалога
    main_layout->addWidget(_tree_view);
    main_layout->addLayout(control_layout);

    //  Обработка сигналов
    connect(_tree_view, &QTreeView::doubleClicked, this, &BindingDialog::doubleClicked);
    connect(enter_button, &QPushButton::clicked, this, &BindingDialog::enterClicked);
    connect(cancel_button, &QPushButton::clicked, this, &BindingDialog::reject);

    resize(400, 500);
}

BindingDialog::~BindingDialog()
{
    if (_model) delete _model;
}


//===================================================================================================================================================
//	Методы обработки сигналов формы
//===================================================================================================================================================
InputSignal* BindingDialog::selectedSignal() const
{
    if (_selected_index < 0)
        return nullptr;
    return _service_manager->din(_selected_index);
}

void BindingDialog::enterClicked()
{
    QModelIndexList indexes = _tree_view->selectionModel()->selectedIndexes();
    if (indexes.count() == 0)
        return;

    QModelIndex index = indexes.first();
    QStandardItemModel* model = (QStandardItemModel*)_tree_view->model();
    QStandardItem* item = model->itemFromIndex(index);
    if (!item->data(Qt::UserRole).isValid())
       return;

    _selected_index = item->data(Qt::UserRole).toInt();
    accept();

    return;
}

void BindingDialog::doubleClicked(const QModelIndex& index)
{
    QStandardItemModel* model = (QStandardItemModel*)_tree_view->model();
    QStandardItem* item = model->itemFromIndex(index);
    if (!item->data(Qt::UserRole).isValid())
        return;

    _selected_index = item->data(Qt::UserRole).toInt();
    accept();

    return;
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void BindingDialog::setModel(uint8_t type)
{
    if (type == TYPE_INPUT) setInputModel();
    if (type == TYPE_FULL_INPUT) setFullInputModel();
    if (type == TYPE_OUTPUT) setOutputModel();
}

void BindingDialog::setInputModel()
{
    //  Создание корневых папок
    QStandardItem* root = _model->invisibleRootItem();
    QStandardItem* physical_folder = nullptr;
    QStandardItem* logical_folder = nullptr;
    QStandardItem* virtual_folder = nullptr;
    QStandardItem* remote_folder = nullptr;

    //  Проверка количества сигналов
    int count = serviceManager()->dins().count();
    if (count < 1)
        return;

    QList<VirtualInputSignal*> virtual_list = serviceManager()->busyVdins();

    //  Создание групп сигналов
    for (int i = 0; i < count; i++) {
        if (physical_folder && logical_folder && virtual_folder && remote_folder)
            break;
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_PHIS && !physical_folder) {
            physical_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Физические входы");
            physical_folder->setEditable(false);
            root->appendRow(physical_folder);
            continue;
        }
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_LOGIC && !logical_folder) {
            logical_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Логические входы");
            logical_folder->setEditable(false);
            root->appendRow(logical_folder);
            continue;
        }
        if (virtual_list.count() > 0 && physical_folder  && logical_folder && !virtual_folder) {
            virtual_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Назначенные виртуальные входы");
            virtual_folder->setEditable(false);
            root->appendRow(virtual_folder);
        }
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_REMOTE && !remote_folder) {
            remote_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Внешние входы");
            remote_folder->setEditable(false);
            root->appendRow(remote_folder);
            continue;
        }
    }

    //  Добавление сигналов
    for (int i = 0; i < count; i++) {
        InputSignal* input = serviceManager()->dins().at(i);
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), input->text());
        signal_item->setEditable(false);
        signal_item->setData(input->internalID(), Qt::UserRole);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_PHIS && physical_folder)
            physical_folder->appendRow(signal_item);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_LOGIC && logical_folder)
            logical_folder->appendRow(signal_item);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_REMOTE && remote_folder)
            remote_folder->appendRow(signal_item);
    }

    //  Добавление назначенных виртуальных входов
    for (int i = 0; i < virtual_list.count(); i++) {
        VirtualInputSignal* input = virtual_list.at(i);
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), input->text());
        signal_item->setEditable(false);
        signal_item->setData(input->internalID(), Qt::UserRole);
        if (virtual_folder) virtual_folder->appendRow(signal_item);
    }

    return;
}

void BindingDialog::setFullInputModel()
{
    //  Создание корневых папок
    QStandardItem* root = _model->invisibleRootItem();
    QStandardItem* adc_folder = nullptr;
    QStandardItem* physical_folder = nullptr;
    QStandardItem* logical_folder = nullptr;
    QStandardItem* virtual_folder = nullptr;
    QStandardItem* free_virtual_folder = nullptr;
    QStandardItem* remote_folder = nullptr;

    //  Проверка количества сигналов
    int count = serviceManager()->dins().count();
    if (count < 1)
        return;

    QList<VirtualInputSignal*> virtual_list = serviceManager()->busyVdins();
    QList<VirtualInputSignal*> free_virtual_list = serviceManager()->freeVdins();

    //  Создание групп сигналов
    if (!virtual_list.isEmpty()) {
        virtual_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Назначенные виртуальные входы");
        virtual_folder->setEditable(false);
    }

    if (!free_virtual_list.isEmpty()) {
        free_virtual_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Свободные виртуальные входы");
        free_virtual_folder->setEditable(false);
    }

    for (int i = 0; i < count; i++) {
        if (physical_folder && logical_folder && remote_folder)
            break;
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_LED_AD && !adc_folder) {
            physical_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Входы встроенных АЦП");
            physical_folder->setEditable(false);
            continue;
        }
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_PHIS && !physical_folder) {
            physical_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Физические входы");
            physical_folder->setEditable(false);
            continue;
        }
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_LOGIC && !logical_folder) {
            logical_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Логические входы");
            logical_folder->setEditable(false);
            continue;
        }
        if (serviceManager()->dins().at(i)->subType() == Signal::DEF_SIG_SUBTYPE_REMOTE && !remote_folder) {
            remote_folder = new QStandardItem(QIcon(":/icons/extension.svg"), "Внешние входы");
            remote_folder->setEditable(false);
            continue;
        }
    }

    if (physical_folder)
        root->appendRow(physical_folder);
    if (logical_folder)
        root->appendRow(logical_folder);
    if (virtual_folder)
        root->appendRow(virtual_folder);
    if (free_virtual_folder)
        root->appendRow(free_virtual_folder);
    if (remote_folder)
        root->appendRow(remote_folder);

    //  Добавление сигналов
    for (int i = 0; i < count; i++) {
        InputSignal* input = serviceManager()->dins().at(i);
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), input->text());
        signal_item->setEditable(false);
        signal_item->setData(input->internalID(), Qt::UserRole);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_PHIS && physical_folder)
            physical_folder->appendRow(signal_item);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_LOGIC && logical_folder)
            logical_folder->appendRow(signal_item);
        if (input->subType() == Signal::DEF_SIG_SUBTYPE_REMOTE && remote_folder)
            remote_folder->appendRow(signal_item);
    }

    //  Добавление назначенных виртуальных входов
    for (int i = 0; i < virtual_list.count(); i++) {
        VirtualInputSignal* input = virtual_list.at(i);
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), input->text());
        signal_item->setEditable(false);
        signal_item->setData(input->internalID(), Qt::UserRole);
        if (virtual_folder) virtual_folder->appendRow(signal_item);
    }

    //  Добавление назначенных виртуальных входов
    for (int i = 0; i < free_virtual_list.count(); i++) {
        VirtualInputSignal* input = free_virtual_list.at(i);
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), input->text());
        signal_item->setEditable(false);
        signal_item->setData(input->internalID(), Qt::UserRole);
        if (free_virtual_folder) free_virtual_folder->appendRow(signal_item);
    }

    return;
}

void BindingDialog::setOutputModel()
{
    //  Создание корневой папки
    QStandardItem* root = _model->invisibleRootItem();
    QStandardItem* folder_item = new QStandardItem(QIcon(":/icons/extension.svg"), "Виртуальные входы");
    root->appendRow(folder_item);

    //  Добавление сигналов
    QList<VirtualInputSignal*> dins_list = serviceManager()->freeVdins();
    for (int i = 0; i < dins_list.count(); i++) {
        QStandardItem* signal_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), dins_list.at(i)->text());
        signal_item->setEditable(false);
        signal_item->setData(dins_list.at(i)->internalID(), Qt::UserRole);
        folder_item->appendRow(signal_item);
    }

    return;
}
