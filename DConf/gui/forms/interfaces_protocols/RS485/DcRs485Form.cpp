#include "DcRs485Form.h"

#include <gui/editors/EditorsManager.h>
#include <gui/forms/interfaces_protocols/RS485/DcRs485PortWidget.h>

namespace {
	const ListEditorContainer g_RepeaterModeList = { "Прозрачный", "Резервирование", "Независимый" };
	const QString g_RepeaterModeDescription = "Режим 'Независимый' возможен только в устройствах с кодами ПО 525 и 527";

	const uint32_t PARAM_REPEATER_MODE = 0x5930;
}

namespace Text {
	const QString Port = "Порт";
	const QString RepeaterMode = "Режим репитера";
}

DcRs485Form::DcRs485Form(DcController *controller) :
	DcForm(controller,  "Настройки RS-485")
{
    auto rs485Param = controller->paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);

	ListEditorContainer wItems;
    for (int i = 0; i < rs485Param->profilesCount(); i++)
		wItems.append({QString::number(i + 1), i});

	auto wCreator = [=](const QVariant &data) { return new DcRs485PortWidget(controller, data.toInt()); };

	QGridLayout *layout = new QGridLayout(centralWidget());
	EditorsManager mg(controller, layout);

	mg.addListEditor(PARAM_REPEATER_MODE, 0, Text::RepeaterMode, g_RepeaterModeList);
	auto editor = mg.addListEditor(Text::Port, wItems);
	mg.addWidgetCreatorLayout(editor, wCreator);	

	layout->setRowStretch(layout->rowCount() - 1, 1);
	layout->setColumnStretch(2, 1);
}

bool DcRs485Form::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_USARTPRTPAR_BYTE},
		{SP_USARTPRTPAR_WORD},
		{SP_USARTPRTPAR_DWORD},
	};

	return hasAny(controller, params);
}

void DcRs485Form::fillReport(DcIConfigReport * report)
{
	auto device = report->device();
    auto repeaterParam = device->paramsRegistry().element(PARAM_REPEATER_MODE, 0);
	if (repeaterParam) {
		report->insertSection();
		DcReportTable table(device);
		table.addRow(DcReportTable::ParamRecord(repeaterParam, g_RepeaterModeList.toHash(), Text::RepeaterMode));
		report->insertTable(table);
		//report->nextLine();
	}

    auto rs485Param = device->paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);
    for (int i = 0; i < rs485Param->profilesCount(); i++) {
		report->insertSection(QString("%1 %2").arg(Text::Port).arg(i + 1));
		DcRs485PortWidget::fillReport(report, i);
	}
}
