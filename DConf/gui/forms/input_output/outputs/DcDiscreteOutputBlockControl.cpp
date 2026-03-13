#include "DcDiscreteOutputBlockControl.h"

#include <gui/editors/EditorsManager.h>

namespace {
	const QMap<int, QString> gBlockReasons = { 
		{0 , "Ошибки конфигурации"},
		{1 , "Результаты внутренних тестов"},
		{2 , "Несоответствия аппаратуры и конфигурации"},
		{3 , "Падения питания"},
		{4 , "По внешним сигналам/командам"},
		{5 , "По состоянию входа разрешения управления"},
		{6 , "Запрет ТУ"},
		{7 , "Запрет управления по месту"}
	};

	const ListEditorContainer gControlModes = { "Разрешение управления", "Запрет телеуправления", "Ключ Дистанция/Местный" };
	const QStringList gControlModesDescriptions = { "1 - Разрешение", "1 - Запрет", "0 - Дистанция, 1 - Местный" };

	ListEditorContainer getDiscretList(DcController *device)
	{
		ListEditorContainer discreteList = { {"Не используется", 0xFF} };
		for (auto signal: device->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS)) {
			discreteList.append({ signal->name(), signal->internalId() });
		}

		return discreteList;
	}	
}

namespace Text {
	const QString BlockReason = "Запретить блокировки управления при";
	const QString TuBlock = "Запрет/Разрешение управления";
	const QString Discrete = "Дискрет";
	const QString Mode = "Режим";
}

DcDiscreteOutputBlockControl::DcDiscreteOutputBlockControl(DcController *controller) :
	DcForm(controller,  "Блокировки управления")
{
	auto blockBox = new QGroupBox(Text::BlockReason);
	EditorsManager mg(controller, new QGridLayout(blockBox));

    auto blockParam = controller->paramsRegistry().element(SP_DOUT_BLOCK, 0);
	for (auto it = gBlockReasons.begin(); it != gBlockReasons.end(); it++)
		mg.addCheckEditor(blockParam, it.value(), it.key());

	mg.addStretch();

    auto paramTele = controller->paramsRegistry().element(SP_DOUT_BLOCK, 2);
	auto controlModeEditor = new ListParamEditor(paramTele, gControlModes);
	auto modeDescriptionLabel = new QLabel;
	auto onControlModeEditorValueChanged = [=](const QString &value) { modeDescriptionLabel->setText(gControlModesDescriptions.value(value.toInt())); };
	connect(controlModeEditor, &ListParamEditor::valueChanged, this, onControlModeEditorValueChanged);
	onControlModeEditorValueChanged(paramTele->value());

	QGroupBox *controlGroupBox = new QGroupBox(Text::TuBlock);
	QGridLayout *controlGroupBoxLayout = new QGridLayout(controlGroupBox);
	mg.setLayout(controlGroupBoxLayout);

	mg.addListEditor(SP_DOUT_BLOCK, 1, Text::Discrete, getDiscretList(controller));
	mg.addWidget(controlModeEditor, new QLabel(Text::Mode), modeDescriptionLabel);
	mg.addStretch();

	mg.setLayout(new QGridLayout(centralWidget()));
	mg.addWidget(blockBox);
	mg.addWidget(controlGroupBox);
	mg.addVerticalStretch();
}

bool DcDiscreteOutputBlockControl::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DOUT_BLOCK}
	};

	return hasAny(controller, params);
}

void DcDiscreteOutputBlockControl::fillReport(DcIConfigReport * report)
{	
	report->insertSection();

	auto device = report->device();
	QStringList headers = { Text::ReportTable::Name };
	QList<DcReportTable::ParamRecord> recs;
    auto blockParam = device->paramsRegistry().element(SP_DOUT_BLOCK, 0);
	for (auto it = gBlockReasons.begin(); it != gBlockReasons.end(); it++) {
		headers << it.value();
		recs.append(DcReportTable::ParamRecord(blockParam, QString(), true, it.key()));
	}

	DcReportTable blockReasonTable(device, headers, { 20 });
	blockReasonTable.addRow(Text::BlockReason, recs);
	report->insertTable(blockReasonTable);

	QStringList headers1 = { Text::ReportTable::Name, Text::Discrete, Text::Mode };
	DcReportTable table(device, headers1);
	recs.clear();
	recs.append(DcReportTable::ParamRecord(SP_DOUT_BLOCK, 1, getDiscretList(device).toHash()));
	recs.append(DcReportTable::ParamRecord(SP_DOUT_BLOCK, 2, gControlModes.toHash()));
	table.addRow(Text::TuBlock, recs);
	report->insertTable(table);
}
