#include "bindings_form.h"


//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/dialogs/msg_box/MsgBox.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QDebug>
#include <QVBoxLayout>
#include <QWidget>
#include <QTabWidget>
#include <QList>
#include <QVariant>
#include <QStandardItem>
#include <QModelIndex>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <dpc/helper.h>

#include "gui/forms/table_of_bindings/algs_delegate.h"
#include "gui/forms/table_of_bindings/outputs_delegate.h"
#include "gui/dialogs/binding_dialog.h"
#include "service_manager/signals/target_element.h"
#include "service_manager/signals/virtual_input_signal.h"
#include "service_manager/signals/input_signal.h"
#include "service_manager/signals/output_signal.h"

#include "service_manager/services/alg/alg_service.h"
#include "service_manager/services/service_input.h"
#include "service_manager/services/service_output.h"


//===================================================================================================================================================
//	Namespace
//===================================================================================================================================================
namespace {
    const int COMMON_LEN_COLOMN = 450;
}

namespace Text {
    const QString AlgsTitle = "Алгоритмы";
    const QString InputTitle = "Входы";
    const QString OutputTitle = "Выходы";

    const QString Phisical = "Физические";
    const QString Led = "Индикаторы";
    const QString Logical = "Логические";
    const QString Virtual = "Виртуальные";
    const QString Remote = "Внешние";

    const QString ConnectedSignal = "Привязаный сигнал";
}


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
BindingForm::BindingForm(DcController *controller)
    : DcForm(controller, Text::AlgsTitle, false)
{
	//	Свойства класса
	_controller = controller;
    _alg_model = new QStandardItemModel();
    _output_model = new QStandardItemModel();

    //  Создание формы
    QVBoxLayout* vbox = new QVBoxLayout(centralWidget());
    QTabWidget* tab_widgets = new QTabWidget();
    vbox->addWidget(tab_widgets);

    //  Вывод закладки алгоритмов
    if (controller->serviceManager()->algManager().count()) {
        QWidget *algs_tab = new QWidget();
        QHBoxLayout *algs_box = new QHBoxLayout();
        algs_tab->setLayout(algs_box);
        tab_widgets->addTab(algs_tab, Text::AlgsTitle);


        _alg_tree = new QTreeView();
        AlgsDelegate* alg_delegate = new AlgsDelegate(this);
        connect(alg_delegate, &AlgsDelegate::onClicked, this, &BindingForm::onAlgClicked);
        _alg_tree->setItemDelegateForColumn(1, alg_delegate);

        _alg_tree->setHeaderHidden(true);
        _alg_tree->setModel(_alg_model);
        getAlgModel(_alg_model);
        _alg_tree->setColumnWidth(0, COMMON_LEN_COLOMN);

        algs_box->addWidget(_alg_tree);
    }

    // Виджет для выходов
    auto param = controller->paramsRegistry().parameter(SP_CROSSTABLEDOUT);
    if (param) {
        QWidget *outputs_tab = new QWidget();
        QHBoxLayout *outputs_box = new QHBoxLayout();
        outputs_tab->setLayout(outputs_box);
        tab_widgets->addTab(outputs_tab, Text::OutputTitle);

        getOutputModel(_output_model);

        QTreeView *output_tree = new QTreeView();
        OutputsDelegate* output_delegate = new OutputsDelegate();
        connect(output_delegate, &OutputsDelegate::onClicked, this, &BindingForm::onOutputClicked);
        output_tree->setItemDelegateForColumn(1, output_delegate);

        output_tree->setHeaderHidden(true);
        output_tree->setModel(_output_model);
        output_tree->setColumnWidth(0, COMMON_LEN_COLOMN);

        outputs_box->addWidget(output_tree);
        output_tree->expandAll();
    }

    return;
}

BindingForm::~BindingForm()
{
    if (_alg_model)
        delete _alg_model;
    if (_output_model)
        delete _output_model;
    if (_alg_tree)
        delete _alg_tree;
}


//===================================================================================================================================================
//	Обязательные методы класса
//===================================================================================================================================================
bool BindingForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_CROSSTABLEDOUT}			//	Нужен список привязок алгоритмов
	};

    if (hasAny(controller, params) || controller->serviceManager()->algManager().count())
        return true;

    return false;
}

void BindingForm::fillReport(DcIConfigReport * report)
{
    auto addSection = [](DcReportTable *table, const QString &sectionName, const DcReportTable::ValueList &values) {
        if (values.isEmpty()) return;
        table->addRow({ sectionName });
        table->addMerge({ table->size(), 0, 1, static_cast<int>(table->headers().size()) });
        for (auto &val : values)
            table->addRow(val);
    };

    report->insertSection(Text::AlgsTitle);
    report->nextLine();

    QStringList headers = { "Ножка алгоритма", Text::ConnectedSignal };
    QFont algTitleFont("Times New Roman", -1, -1, true);

    auto device = report->device();
//    for (size_t i = 0; i < device->algs_internal()->size(); i++) {
//        auto alg = device->algs_internal()->get(i);
//        report->insertSection(alg->name(), false, &algTitleFont);

//        DcReportTable::ValueList inputs;
//        DcReportTable::ValueList outputs;
//        for (size_t j = 0; j < alg->ios()->size(); j++) {
//            auto io = alg->ios()->get(j);

//            QString value;
//            auto bind = device->matrix_alg()->get(io->index());
//            if (bind) {
//                auto signal = device->getSignal(bind->dst());
//                if (signal)
//                    value = QString("(%1) %2").arg(signal->internalId()).arg(signal->name());
//            }

//            QStringList rec = { io->name(), value };
//            switch (io->direction())
//            {
//            case IO_DIRECTION_INPUT: inputs << rec; break;
//            case IO_DIRECTION_OUTPUT: outputs << rec; break;
//            default: break;
//            }
//        }

//        DcReportTable table(device, headers);
//        addSection(&table, Text::InputTitle, inputs);
//        addSection(&table, Text::OutputTitle, outputs);
//        report->insertTable(table);
//    }

    report->insertSection(Text::OutputTitle);
    DcReportTable::ValueList phisicals;
    DcReportTable::ValueList leds;
    DcReportTable::ValueList logicals;
    DcReportTable::ValueList virtuals;
    DcReportTable::ValueList remotes;
//    for (auto signal: device->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_UNDEF, DEF_SIG_DIRECTION_OUTPUT)) {
//        QString value;
//        auto bind = device->matrix_signals()->getDst(signal->index());
//        if (bind) {
//            auto sourceSignal = device->getSignal(bind->src());
//            if (sourceSignal)
//                value = QString("(%1) %2").arg(sourceSignal->internalId()).arg(sourceSignal->name());
//        }

//        QStringList rec = { QString::number(signal->internalId()), signal->name(), value };
//        switch (signal->subtype())
//        {
//        case DEF_SIG_SUBTYPE_PHIS: phisicals << rec; break;
//        case DEF_SIG_SUBTYPE_LED_AD: leds << rec; break;
//        case DEF_SIG_SUBTYPE_LOGIC: logicals << rec; break;
//        case DEF_SIG_SUBTYPE_VIRTUAL: virtuals << rec; break;
//        case DEF_SIG_SUBTYPE_REMOTE: remotes << rec; break;
//        default: break;
//        }
//    }

    headers = QStringList{ "Номер", "Выход", Text::ConnectedSignal };
    DcReportTable table(device, headers, { 5 });
    addSection(&table, Text::Phisical, phisicals);
    addSection(&table, Text::Led, leds);
    addSection(&table, Text::Logical, logicals);
    addSection(&table, Text::Virtual, virtuals);
    addSection(&table, Text::Remote, remotes);
    report->insertTable(table);
}


//===================================================================================================================================================
//	Методы обработки сигналов делегатов
//===================================================================================================================================================
void BindingForm::onAlgClicked(const QModelIndex& index)
{
    //  Параметры выбранного алгоритма
    int flag = index.data(AlgsDelegate::IO_FLAG).toInt();

    // Операции со входами
    if (flag == 0) {
        auto service_data = Dpc::toPtr<ServiceInput>(index.data(AlgsDelegate::SERVICE_DATA));
        if (!service_data)
            return;

        if (service_data->source()) {
            //  Удаление входа
            if (!Dpc::Gui::MsgBox::question("Вы уверены, что хотите удалить привязку?"))
                return;

            service_data->setSource(nullptr);
        }
        else {
            //  Добавление входа
            BindingDialog dialog(BindingDialog::TYPE_INPUT, controller()->serviceManager());
            if (dialog.exec() != QDialog::Accepted)
                return;

            InputSignal* input_signal = dialog.selectedSignal();
            if (input_signal)
                service_data->setSource(input_signal);
        }
    }

    // Операции со выходами
    if (flag == 1) {
        auto service_data = Dpc::toPtr<ServiceOutput>(index.data(AlgsDelegate::SERVICE_DATA));
        if (!service_data)
            return;

        if (service_data->target()) {
            //  Удаление выхода
            //  Проверка на принадлежность гибкой логике
//            if (isUsedInCfc(service_data->target())) {
//                QString info = "Привязка уже используется в алгоритме гибкой логики! \nДля удаления переназначьте используемый сигнал. ";
//                Dpc::Gui::MsgBox::error(info);
//                return;
//            }

            if (!Dpc::Gui::MsgBox::question("Вы уверены, что хотите удалить привязку?"))
                return;

            service_data->setTarget(nullptr);
        }
        else {
            //  Добавление выхода
            BindingDialog dialog(BindingDialog::TYPE_OUTPUT, controller()->serviceManager());
            if (dialog.exec() != QDialog::Accepted)
                return;

            VirtualInputSignal* output_signal = dynamic_cast<VirtualInputSignal*>(dialog.selectedSignal());
            if (!output_signal)
                return;

            service_data->setTarget(output_signal);
        }
    }
}

void BindingForm::onOutputClicked(const QModelIndex& index)
{
    auto output_data = Dpc::toPtr<OutputSignal>(index.data(OutputsDelegate::SERVICE_DATA));
    if (!output_data)
        return;

    if (output_data->source()) {
        //  Удаление привязки
        if (!Dpc::Gui::MsgBox::question("Вы уверены, что хотите удалить привязку?"))
            return;

        output_data->setSource(nullptr);
    }
    else {
        //  Добавление привязки
        BindingDialog dialog(BindingDialog::TYPE_INPUT, controller()->serviceManager());
        if (dialog.exec() != QDialog::Accepted)
            return;

        InputSignal* input_signal = dialog.selectedSignal();
        if (input_signal)
            output_data->setSource(input_signal);
    }
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
bool BindingForm::getAlgModel(QStandardItemModel* model)
{
    model->setColumnCount(2);
    QStandardItem* root = model->invisibleRootItem();

    auto algList = controller()->serviceManager()->algManager().algList();
    std::sort(algList.begin(), algList.end(), [](AlgService* a, AlgService* b) { return a->name() < b->name(); });
    for (int i = 0; i < algList.count(); i++) {
        auto alg_service = algList.at(i);
        QStandardItem* row_item = new QStandardItem(QIcon(":/icons/algorithm.svg"), alg_service->name());
        row_item->setEditable(false);

        bool expand = false;
        for (size_t i = 0; i < alg_service->inputs().size(); i++) {
            QString name = QString();
            QList<QStandardItem*> row_items;
            QStandardItem* input_item = new QStandardItem(QIcon(":/icons/signal_in.svg"), alg_service->inputs().at(i)->name());
            input_item->setEditable(false);
            row_items.append(input_item);
            QStandardItem* data_item = new QStandardItem;

            ServiceInput* service_data = alg_service->inputs().at(i).get();
            if (service_data->source()) expand = true;
            data_item->setData(0, AlgsDelegate::IO_FLAG);            //  Признак входа
            data_item->setData(Dpc::fromPtr(service_data), AlgsDelegate::SERVICE_DATA);

            row_items.append(data_item);
            row_item->appendRow(row_items);
        }

        //  Добавленте выходов
        for (size_t i = 0; i < alg_service->outputs().size(); i++) {
            QString name = QString();
            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), alg_service->outputs().at(i)->name());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;

            ServiceOutput* service_data = alg_service->outputs().at(i).get();
            if (service_data->target()) expand = true;

            data_item->setData(1,  AlgsDelegate::IO_FLAG);            //  Признак выхода
            data_item->setData(Dpc::fromPtr(service_data),  AlgsDelegate::SERVICE_DATA);

            row_items.append(data_item);
            row_item->appendRow(row_items);
        }
        root->appendRow(row_item);
        QModelIndex index = model->indexFromItem(row_item);
        if (expand)
            _alg_tree->expand(index);
    }

    return true;
}

bool BindingForm::getOutputModel(QStandardItemModel* model)
{
    //	Проверка сигналов
    QList<OutputSignal*> douts_list = controller()->serviceManager()->douts();
    if (douts_list.count() < 1)
        return false;

    //	Заполнение модели
    model->setColumnCount(2);
    QStandardItem* root = model->invisibleRootItem();

    //	Инициализация параметров модели
    QStandardItem* led_item = nullptr;
    QStandardItem* physical_item = nullptr;
    QStandardItem* logical_item = nullptr;
    QStandardItem* virtual_item = nullptr;
    QStandardItem* remote_item = nullptr;

    //  Цикл по сигналам
    for (int i = 0; i < douts_list.count(); i++) {
        OutputSignal* output = douts_list.at(i);

        //	Добавление индикаторов
        if ((output->subType() == DEF_SIG_SUBTYPE_PHIS || output->subType() == DEF_SIG_SUBTYPE_LED_AD) &&
            douts_list.at(i)->name().contains("LED")) {
            if (!led_item) {
                led_item = new QStandardItem(QIcon(":/icons/led.svg"), "Индикаторы");
                led_item->setEditable(false);
                root->appendRow(led_item);
            }

            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), output->text());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;
            data_item->setData(Dpc::fromPtr(output), OutputsDelegate::SERVICE_DATA);
            row_items.append(data_item);
            led_item->appendRow(row_items);
        }

        //	Добавление физических выходов
        if (output->subType() == DEF_SIG_SUBTYPE_PHIS && !output->name().contains("LED")) {
            if (!physical_item) {
                physical_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), "Физические выходы");
                physical_item->setEditable(false);
                root->appendRow(physical_item);
            }
            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), output->text());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;
            data_item->setData(Dpc::fromPtr(output), OutputsDelegate::SERVICE_DATA);
            row_items.append(data_item);
            physical_item->appendRow(row_items);
        }

        //	Добавление логических выходов
        if (output->subType() == DEF_SIG_SUBTYPE_LOGIC) {
            if (!logical_item) {
                logical_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), "Логические выходы");
                logical_item->setEditable(false);
                root->appendRow(logical_item);
            }
            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), output->text());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;
            data_item->setData(Dpc::fromPtr(output), OutputsDelegate::SERVICE_DATA);
            row_items.append(data_item);
            logical_item->appendRow(row_items);
        }

        //	Добавление виртуальных выходов
        if (output->subType() == DEF_SIG_SUBTYPE_VIRTUAL) {
            if (!virtual_item) {
                virtual_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), "Выходы виртуальных функций");
                virtual_item->setEditable(false);
                root->appendRow(virtual_item);
            }
            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), output->text());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;
            data_item->setData(Dpc::fromPtr(output), OutputsDelegate::SERVICE_DATA);
            row_items.append(data_item);
            virtual_item->appendRow(row_items);
        }

        //	Добавление внешних выходов
        if (output->subType() == DEF_SIG_SUBTYPE_REMOTE) {
            if (!remote_item) {
                remote_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), "Внешние выходы");
                remote_item->setEditable(false);
                root->appendRow(remote_item);

            }

            QList<QStandardItem*> row_items;
            QStandardItem* output_item = new QStandardItem(QIcon(":/icons/signal_out.svg"), output->text());
            output_item->setEditable(false);
            row_items.append(output_item);
            QStandardItem* data_item = new QStandardItem;
            data_item->setData(Dpc::fromPtr(output), OutputsDelegate::SERVICE_DATA);
            row_items.append(data_item);
            remote_item->appendRow(row_items);
        }
    }

    return true;
}

bool BindingForm::isUsedInCfc(VirtualInputSignal *vdin) const
{
    if (vdin->source() && vdin->source()->service()->type() == Service::CfcAlgType)
        return true;

    for(auto target: vdin->targets())
        if (target->service() && target->service()->type() == Service::CfcAlgType)
            return true;

    return false;
}
