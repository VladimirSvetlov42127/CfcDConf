#include "IModbusWidgetFunction.h"

IModbusWidgetFunction::IModbusWidgetFunction(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

IModbusWidgetFunction::~IModbusWidgetFunction()
{
}

void IModbusWidgetFunction::updateFromFunction(spModbusFunc_t function)
{
	if (!function)
		return;

	if (ModbusFunctions_ns::isStandartFunctionCode(function->getFunctionType()))  {	// Если функция пользовательская, у нее нет этих полей
		ui.lEdStartAddress->setText(QString::number(function->getStartAddress()));
		ui.lEdCount->setText(QString::number(function->getModbusRegisterCount()));	// Рассчитываем кол-во регистров (если надо, иначе просто кол-во запрашиваемых каналов)
	}
}

