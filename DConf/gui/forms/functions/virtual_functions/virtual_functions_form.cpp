#include "virtual_functions_form.h"


//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QHeaderView>
#include <QStringList>
#include <QWidget>
#include <QHBoxLayout>

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>


//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include "gui/forms/functions/virtual_functions/virtual_functions_model.h"
#include "gui/forms/functions/virtual_functions/vfunc_delegate.h"


//===================================================================================================================================================
//	Конструктор класса
//===================================================================================================================================================
VirtualFunctionsForm::VirtualFunctionsForm(DcController* controller) : DcForm(controller, "Настройки функций виртуальных выходов")
{
    //  Создание делегата для списка функций
    Dpc::Gui::ComboBoxDelegate* func_delegate = new Dpc::Gui::ComboBoxDelegate(this);
    auto types_list = controller->serviceManager()->funcService().supportedTypes();
    for (auto& item : types_list) {
        func_delegate->append({ item.second.name, item.first });
    }

    //  Универсальный делегат для вывода параметров
    VFuncDelegate* params_delegate = new VFuncDelegate(controller, this);

    //  Таблица для вывода списка виртуальных функций
    Dpc::Gui::TableView* table_view = new Dpc::Gui::TableView();
    VirtualFunctionsModel* model = new VirtualFunctionsModel(&controller->serviceManager()->funcService());
    table_view->setModel(model);
    table_view->horizontalHeader()->resizeSection(VirtualFunctionsModel::OUTPUT_COLUMN, 200);
    table_view->horizontalHeader()->resizeSection(VirtualFunctionsModel::FUNCTION_COLUMN, 350);
    table_view->horizontalHeader()->resizeSection(VirtualFunctionsModel::PARAMETER_COLUMN, 200);

    table_view->setItemDelegateForColumn(VirtualFunctionsModel::FUNCTION_COLUMN, func_delegate);
    table_view->setItemDelegateForColumn(VirtualFunctionsModel::PARAMETER_COLUMN, params_delegate);

    QHBoxLayout* layout = new QHBoxLayout(centralWidget());
    layout->addWidget(table_view);
}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
bool VirtualFunctionsForm::isAvailableFor(DcController * controller)
{
    static QList<Param> params = {
        {SP_DOUT_TYPE}
    };

    return hasAny(controller, params);
}

void VirtualFunctionsForm::fillReport(DcIConfigReport * report)
{
    // auto device = report->device();

    // auto newFuncList = device->virtualFunctionList();
    // ListEditorContainer functionsList;
    // for (auto& it : newFuncList) functionsList.append(it.second, it.first);
    // auto virtualDiscretList = virtualInputDiscretList(device);
    // auto fixedVirtualDiscretList = fixedVirtualInputDiscretList(device);
    // auto xcbrList = ::xcbrList(device);

    // report->insertSection();

    // QStringList headers = {Text::ReportTable::Name, "Тип", "Аргумент" };
    // DcReportTable table(report->device(), headers, {40});

    // auto functionsParam = device->paramsRegistry().parameter(SP_DOUT_TYPE);
    // for (size_t i = 0; i < functionsParam->profilesCount(); i++) {
    //     auto name = QString("Функция %1").arg(i + 1);

    //     auto fParam = device->paramsRegistry().element(SP_DOUT_TYPE, i * PROFILE_SIZE);
    //     auto type = functionsList.text(fParam->value());

    //     QString arg = device->paramsRegistry().element(SP_DOUT_TYPE, i * PROFILE_SIZE + 1)->value();
    //     switch (device->virtualFunctionType(fParam->value().toUInt()))
    //     {
    //     case DcController::VDIN_CONTROL:
    //     case DcController::VDIN_EVENT:
    //         arg = virtualDiscretList.text(arg);
    //         break;
    //     case DcController::XCBR_CNTRL:
    //         arg = xcbrList.text(arg);
    //         break;
    //     case DcController::FIX_VDIN:
    //         arg = fixedVirtualDiscretList.text(arg);
    //         break;
    //     default:
    //         break;
    //     }

    //     table.addRow({name, type, arg });
    // }

    // report->insertTable(table);
}
