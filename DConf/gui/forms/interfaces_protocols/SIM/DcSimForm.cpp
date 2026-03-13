#include "DcSimForm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_SlotList = { "Слот 1", "Слот 2" };

	const uint32_t PARAM_SIM_SETTINGS = 0x5E30;
	const uint32_t PARAM_GSM_SETTINGS = 0x5F30;
}

namespace Text {
	const QString Title = "SIM %1";

	const QString SIM_SETTINGS0 = "№ предпочтительной SIM-карты";
	const QString SIM_SETTINGS1 = "Работа с двумя SIM картами";

	const QString GSM_SETTINGS0 = "APN";
	const QString GSM_SETTINGS1 = "Login";
	const QString GSM_SETTINGS2 = "Password";
	const QString GSM_SETTINGS3 = "APN";
	const QString GSM_SETTINGS4 = "Login";
	const QString GSM_SETTINGS5 = "Password";
}

DcSimForm::DcSimForm(DcController *controller) :
	DcForm(controller,  "SIM карты")
{
	EditorsManager manager(controller, new QGridLayout(centralWidget()));

	auto groupManager = manager.addGroupBox("Общие параметры");
	groupManager->addListEditor(PARAM_SIM_SETTINGS, 0, Text::SIM_SETTINGS0, g_SlotList);
	groupManager->addCheckEditor(PARAM_SIM_SETTINGS, 1, Text::SIM_SETTINGS1);

	groupManager = manager.addGroupBox(QString(Text::Title).arg(1));
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 0, Text::GSM_SETTINGS0);
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 1, Text::GSM_SETTINGS1);
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 2, Text::GSM_SETTINGS2);

	groupManager = manager.addGroupBox(QString(Text::Title).arg(2));
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 3, Text::GSM_SETTINGS3);
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 4, Text::GSM_SETTINGS4);
	groupManager->addLineEditor(PARAM_GSM_SETTINGS, 5, Text::GSM_SETTINGS5);

	manager.addStretch();
}

bool DcSimForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{PARAM_SIM_SETTINGS},
		{PARAM_GSM_SETTINGS}
	};

	return hasAny(controller, params);
}

void DcSimForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();
	DcReportTable table(report->device());
	table.addRow(DcReportTable::ParamRecord(PARAM_SIM_SETTINGS, 0, g_SlotList.toHash(), Text::SIM_SETTINGS0));
	table.addRow(DcReportTable::ParamRecord(PARAM_SIM_SETTINGS, 1, Text::SIM_SETTINGS1, true));
	report->insertTable(table);

	QStringList headers = {
		Text::ReportTable::Name,
		Text::GSM_SETTINGS0,
		Text::GSM_SETTINGS1,
		Text::GSM_SETTINGS2
	};

	DcReportTable simTable(report->device(), headers, { 50 });
	QList<DcReportTable::ParamRecord> recs;
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 0));
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 1));
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 2));
	simTable.addRow(Text::Title.arg(1), recs);

	recs.clear();
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 3));
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 4));
	recs.append(DcReportTable::ParamRecord(PARAM_GSM_SETTINGS, 5));
	simTable.addRow(Text::Title.arg(2), recs);

	report->insertTable(simTable);
}
