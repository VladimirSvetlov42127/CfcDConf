#include "DcIEC103SettingsForm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_CoreAmperageList = { "200А", "5А" };
}

namespace Text {
	const QString IEC_103_PARAMS0 = "Период выдачи циклических данных IEC 103";
	const QString IEC_103_PARAMS1 = "Бит недостоверности неподдерживаемых AIN";
	const QString FIO_TypeFIO3 = "Керн токов РЗА";

}

DcIEC103SettingsForm::DcIEC103SettingsForm(DcController *controller) :
    DcForm(controller,  "Параметры МЭК60870-5-103")
{
	QGridLayout *layout = new QGridLayout(centralWidget());

	EditorsManager manager(controller, layout);
	manager.addLineEditor(SP_IEC_103_PARAMS, 0, Text::IEC_103_PARAMS0);
	manager.addCheckEditor(SP_IEC_103_PARAMS, 1, Text::IEC_103_PARAMS1);
	//manager.addListEditor(SP_FIO_TypeFIO, 3, Text::FIO_TypeFIO3, g_CoreAmperageList);	

	manager.addStretch();
}

bool DcIEC103SettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_IEC_103_PARAMS}
	};

	return hasAny(controller, params);
}

void DcIEC103SettingsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	auto device = report->device();
	DcReportTable table(device);
	table.addRow(DcReportTable::ParamRecord(SP_IEC_103_PARAMS, 0, Text::IEC_103_PARAMS0));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_103_PARAMS, 1, Text::IEC_103_PARAMS1, true));
	//table.addRow(DcReportTable::ParamRecord(SP_FIO_TypeFIO, 3, g_CoreAmperageList.toHash(), Text::FIO_TypeFIO3));

	report->insertTable(table);
}
