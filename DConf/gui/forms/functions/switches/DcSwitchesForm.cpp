#include "DcSwitchesForm.h"

#include <qtabwidget.h>


#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_BlockTypeList = { { "Запрет телеуправления", 1 }, { "Местный/Дистанция", 2 } };
}

namespace Text {
	namespace Switch {
		const QString Title = "Выключатель %1";

		const QString XCBRCFG0_0 = "Активен";
		const QString XCBRCFG0_1 = "Подсчет количества включений";
		const QString XCBRCFG0_2 = "Подсчет количества выключений";
		const QString XCBRCFG0_3 = "Измерение времени включения";
		const QString XCBRCFG0_4 = "Измерение времени выключения";

		const QString XCBRCFG2 = "Номер модуля обработки сигнала РПО контакта";
		const QString XCBRCFG3 = "Номер  канала сигнала РПО контакта в модуле";
		const QString XCBRCFG4 = "Номер  модуля обработки сигнала РПВ контакта";
		const QString XCBRCFG5 = "Номер канала сигнала РПВ контакта в модуле";
		const QString XCBRCFG6 = "Номер  модуля  сигнала управления: Включить";
		const QString XCBRCFG7 = "Номер канала сигнала управления: Включить";
		const QString XCBRCFG8 = "Номер  модуля сигнала управления: Отключить";
		const QString XCBRCFG9 = "Номер канала сигнала управления: Отключить";
	}

	namespace TUBlock {
		const QString Title = "Модуль ограничения ТУ";
		const QString Mode = "Режим запрета ТУ";
	}	
}

DcSwitchesForm::DcSwitchesForm(DcController *controller) :
	DcForm(controller,  "Выключатели", false)
{
	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	QTabWidget *tabWidget = new QTabWidget;
	layout->addWidget(tabWidget);

    auto xcbrParam = controller->paramsRegistry().parameter(SP_XCBRCFG);
    for (size_t i = 0; i < xcbrParam->profilesCount(); i++)
		tabWidget->addTab(createTab(i), QString(Text::Switch::Title).arg(i + 1));

	tabWidget->addTab(createTUBlockTab(), Text::TUBlock::Title);
}

bool DcSwitchesForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_XCBRCFG},
		{SP_TCRESTR_BYTE_PARAM}
	};

	return hasAny(controller, params);
}

void DcSwitchesForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers = {
		Text::ReportTable::Name,
		Text::Switch::XCBRCFG0_0,
		Text::Switch::XCBRCFG0_1,
		Text::Switch::XCBRCFG0_2,
		Text::Switch::XCBRCFG0_3,
		Text::Switch::XCBRCFG0_4,
		Text::Switch::XCBRCFG2,
		Text::Switch::XCBRCFG3,
		Text::Switch::XCBRCFG4,
		Text::Switch::XCBRCFG5,
		Text::Switch::XCBRCFG6,
		Text::Switch::XCBRCFG7,
		Text::Switch::XCBRCFG8,
		Text::Switch::XCBRCFG9
	};

	auto device = report->device();
	DcReportTable table(device, headers);
    auto xcbrParam = device->paramsRegistry().parameter(SP_XCBRCFG);
    for (size_t i = 0; i < xcbrParam->profilesCount(); i++) {
		auto idxBase = PROFILE_SIZE * i;
        auto xcbrMainParam = device->paramsRegistry().element(SP_XCBRCFG, idxBase);

		QList<DcReportTable::ParamRecord> recs;
		recs.append(DcReportTable::ParamRecord(xcbrMainParam, QString(), true, 0));
		recs.append(DcReportTable::ParamRecord(xcbrMainParam, QString(), true, 1));
		recs.append(DcReportTable::ParamRecord(xcbrMainParam, QString(), true, 2));
		recs.append(DcReportTable::ParamRecord(xcbrMainParam, QString(), true, 3));
		recs.append(DcReportTable::ParamRecord(xcbrMainParam, QString(), true, 4));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 2));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 3));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 4));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 5));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 6));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 7));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 8));
		recs.append(DcReportTable::ParamRecord(SP_XCBRCFG, idxBase + 9));
		table.addRow(QString(Text::Switch::Title).arg(i + 1), recs);
	}
	report->insertTable(table);
	
    if (auto param = device->paramsRegistry().element(SP_TCRESTR_BYTE_PARAM, 0); param) {
		report->insertSection(Text::TUBlock::Title);
		DcReportTable table(report->device());
		table.addRow(DcReportTable::ParamRecord(param, g_BlockTypeList.toHash(), Text::TUBlock::Mode));
		report->insertTable(table);
	}
}

QWidget * DcSwitchesForm::createTab(int idx) const
{
	QWidget *tab = new QWidget;
	QGridLayout *layout = new QGridLayout(tab);
	EditorsManager manager(controller(), layout);

	int paramIdxBase = idx * PROFILE_SIZE;
    auto xcbrMainParam = controller()->paramsRegistry().element(SP_XCBRCFG, paramIdxBase);

	manager.addCheckEditor(xcbrMainParam, Text::Switch::XCBRCFG0_0, 0);
	manager.addCheckEditor(xcbrMainParam, Text::Switch::XCBRCFG0_1, 1);
	manager.addCheckEditor(xcbrMainParam, Text::Switch::XCBRCFG0_2, 2);
	manager.addCheckEditor(xcbrMainParam, Text::Switch::XCBRCFG0_3, 3);
	manager.addCheckEditor(xcbrMainParam, Text::Switch::XCBRCFG0_4, 4);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 2, Text::Switch::XCBRCFG2);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 3, Text::Switch::XCBRCFG3);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 4, Text::Switch::XCBRCFG4);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 5, Text::Switch::XCBRCFG5);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 6, Text::Switch::XCBRCFG6);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 7, Text::Switch::XCBRCFG7);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 8, Text::Switch::XCBRCFG8);
	manager.addLineEditor(SP_XCBRCFG, paramIdxBase + 9, Text::Switch::XCBRCFG9);

	manager.addStretch();
	return tab;
}

QWidget* DcSwitchesForm::createTUBlockTab() const
{
    auto param = controller()->paramsRegistry().element(SP_TCRESTR_BYTE_PARAM, 0);
	if (!param)
		return nullptr;

	auto widget = new QWidget;
	QGridLayout* layout = new QGridLayout(widget);
	EditorsManager em(controller(), layout);

	em.addListEditor(param, Text::TUBlock::Mode, g_BlockTypeList);
	em.addStretch();

	return widget;
}
