#include "DcPqiSettingsForm.h"

#include <gui/editors/EditorsManager.h>

namespace {
} // namespace

namespace Text {
	const QString AIN_PHYLVL20 = "Порог пропадания напряжения (б/р от ном)";
	const QString AIN_PHYLVL21 = "Порог перегрузки по току (б/р от ном)";
	const QString AIN_PHYLVL22 = "Время срабатывания прг. по току (мсек)";
	const QString AIN_PHYLVL23 = "Порог перегрузки по току Io (б/р от ном)";
	const QString AIN_PHYLVL24 = "Порог гармоник по напряжению (б/р)";
	const QString AIN_PHYLVL25 = "Порог гармоник по току (б/р)";
	const QString AIN_PHYLVL26 = "Коэфф. возврата для уставок (б/р)";
	const QString AIN_PHYLVL27 = "Время усреднения для мощн. (мин)";

	const QString AIN_PHYLVL18 = "Порог защиты от самохода счетчика ЭЭ (% от Inom)";
	const QString AIN_PHYLVL19 = "Порог фиксации провалов (% от Unom)";
}

DcPqiSettingsForm::DcPqiSettingsForm(DcController *controller) :
	DcForm(controller,  "Уставки ПКЭ")
{
	QGridLayout *layout = new QGridLayout(centralWidget());
	EditorsManager mg(controller, layout);

	mg.addLineEditor(SP_AIN_PHYLVL2, 0, Text::AIN_PHYLVL20, 0, 1);
	mg.addLineEditor(SP_AIN_PHYLVL2, 1, Text::AIN_PHYLVL21, 1, 2);
	mg.addLineEditor(SP_AIN_PHYLVL2, 2, Text::AIN_PHYLVL22, 20, 10000);
	mg.addLineEditor(SP_AIN_PHYLVL2, 3, Text::AIN_PHYLVL23, 0, 1);
	mg.addLineEditor(SP_AIN_PHYLVL2, 4, Text::AIN_PHYLVL24, 0, 1);
	mg.addLineEditor(SP_AIN_PHYLVL2, 5, Text::AIN_PHYLVL25, 0, 2);
	mg.addLineEditor(SP_AIN_PHYLVL2, 6, Text::AIN_PHYLVL26, 0, 1);
	mg.addLineEditor(SP_AIN_PHYLVL2, 7, Text::AIN_PHYLVL27, 1, 60);

	mg.addLineEditor(SP_AIN_PHYLVL1, 8, Text::AIN_PHYLVL18);
	auto editor = mg.addLineEditor(SP_AIN_PHYLVL1, 9, Text::AIN_PHYLVL19);
	if (editor)
		editor->setDecimals(4);

	mg.addStretch();
}

bool DcPqiSettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_AIN_PHYLVL2},
	};

	return hasAny(controller, params);
}

void DcPqiSettingsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();
	DcReportTable table(report->device());
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 0, Text::AIN_PHYLVL20));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 1, Text::AIN_PHYLVL21));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 2, Text::AIN_PHYLVL22));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 3, Text::AIN_PHYLVL23));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 4, Text::AIN_PHYLVL24));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 5, Text::AIN_PHYLVL25));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 6, Text::AIN_PHYLVL26));
	table.addRow(DcReportTable::ParamRecord(SP_AIN_PHYLVL2, 7, Text::AIN_PHYLVL27));
	report->insertTable(table);
}
