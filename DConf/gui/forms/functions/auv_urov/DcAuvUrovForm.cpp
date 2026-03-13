#include "DcAuvUrovForm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_AVRModeList = { "АВР выведен", "АВР СВ", "АВР ВВОД" };
} // namespace

namespace Text {
	namespace APV {
		const QString Title = "АПВ";

		const QString RZA_TIME10 = "Время готовности АПВ (с)";
		const QString RZA_TIME11 = "Время АПВ (с)";
	}

	namespace AVR {
		const QString Title = "АВР";

		const QString RZA_TIME27 = "Время АВР (с)";
		const QString SP_RZA_ALGMNG103 = "Режим работы АВР";
	}

	namespace UrovIn {
		const QString Title = "УРОВ вход";

		const QString RZA_ALGMNG131 = "Контролировать пуски МТЗ";
		const QString RZA_TIME32 = "По входу 1 задержка срабатывания (с)";
		const QString RZA_TIME33 = "По входу 2 задержка срабатывания (с)";
	}

	namespace UrovOut {
		const QString Title = "УРОВ выход";

		const QString RZA_TIME4 = "Задержка срабатывания (с)";
		const QString RZA_KOEFF4 = "К возврата";
		const QString RZA_THRESHOLDS4 = "Порог срабатывания (А)";
	}

	namespace ControlNCU {
		const QString Title = "Контроль НЦУ";

		const QString RZA_TIME7 = "Время диагностики НЦУ (с)";
	}

	namespace ControlVoltageSection {
		const QString Title = "Контроль напряжения секции";

		const QString RZA_LEVEL20 = "Контроль напряжения секции (В)";
		const QString RZA_ALGMNG132 = "Учитывать чередование фаз";
	}
}

DcAuvUrovForm::DcAuvUrovForm(DcController *controller) :
	DcForm(controller,  "Настройки АУВ/УРОВ")
{
	QGridLayout *layout = new QGridLayout(centralWidget());
	EditorsManager mg(controller, layout);

	auto groupManager = mg.addGroupBox(Text::APV::Title);
	groupManager->addLineEditor(SP_RZA_TIME, 10, Text::APV::RZA_TIME10, 0);
	groupManager->addLineEditor(SP_RZA_TIME, 11, Text::APV::RZA_TIME11, 0);
	
	groupManager = mg.addGroupBox(Text::AVR::Title);
	groupManager->addLineEditor(SP_RZA_TIME, 27, Text::AVR::RZA_TIME27 , 0);
	groupManager->addListEditor(SP_RZA_ALGMNG, 103, Text::AVR::SP_RZA_ALGMNG103, g_AVRModeList);

	groupManager = mg.addGroupBox(Text::UrovIn::Title);
	groupManager->addCheckEditor(SP_RZA_ALGMNG, 131, Text::UrovIn::RZA_ALGMNG131);
	groupManager->addLineEditor(SP_RZA_TIME, 32, Text::UrovIn::RZA_TIME32, 0);
	groupManager->addLineEditor(SP_RZA_TIME, 33, Text::UrovIn::RZA_TIME33, 0);

	groupManager = mg.addGroupBox(Text::UrovOut::Title);
	groupManager->addLineEditor(SP_RZA_TIME, 4, Text::UrovOut::RZA_TIME4, 0);
	groupManager->addLineEditor(SP_RZA_KOEFF, 4, Text::UrovOut::RZA_KOEFF4);
	groupManager->addLineEditor(SP_RZA_THRESHOLDS, 4, Text::UrovOut::RZA_THRESHOLDS4, 0);

	groupManager = mg.addGroupBox(Text::ControlNCU::Title);
	groupManager->addLineEditor(SP_RZA_TIME, 7, Text::ControlNCU::RZA_TIME7, 0.5, 60.0);

	groupManager = mg.addGroupBox(Text::ControlVoltageSection::Title);
	groupManager->addLineEditor(SP_RZA_LEVEL, 20, Text::ControlVoltageSection::RZA_LEVEL20, 0);
	groupManager->addCheckEditor(SP_RZA_ALGMNG, 132, Text::ControlVoltageSection::RZA_ALGMNG132);

	mg.addStretch();
}

bool DcAuvUrovForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_RZA_TIME, 10},
		{SP_RZA_TIME, 27},
		{SP_RZA_ALGMNG, 131},
		{SP_RZA_TIME, 4},
		{SP_RZA_TIME, 7},
		{SP_RZA_LEVEL, 20},
	};

	return hasAny(controller, params);
}

void DcAuvUrovForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	DcReportTable table(report->device());
	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 10, Text::APV::RZA_TIME10));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 11, Text::APV::RZA_TIME11));

	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 27, Text::AVR::RZA_TIME27));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_ALGMNG, 103, g_AVRModeList.toHash(), Text::AVR::SP_RZA_ALGMNG103));

	table.addRow(DcReportTable::ParamRecord(SP_RZA_ALGMNG, 131, Text::UrovIn::RZA_ALGMNG131, true));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 32, QString("%1: %2").arg(Text::UrovIn::Title, Text::UrovIn::RZA_TIME32)));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 33, QString("%1: %2").arg(Text::UrovIn::Title, Text::UrovIn::RZA_TIME33)));

	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 4, QString("%1: %2").arg(Text::UrovOut::Title, Text::UrovOut::RZA_TIME4)));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_KOEFF, 4, QString("%1: %2").arg(Text::UrovOut::Title, Text::UrovOut::RZA_KOEFF4)));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_THRESHOLDS, 4, QString("%1: %2").arg(Text::UrovOut::Title, Text::UrovOut::RZA_THRESHOLDS4)));

	table.addRow(DcReportTable::ParamRecord(SP_RZA_TIME, 7, Text::ControlNCU::RZA_TIME7));

	table.addRow(DcReportTable::ParamRecord(SP_RZA_LEVEL, 20, Text::ControlVoltageSection::RZA_LEVEL20));
	table.addRow(DcReportTable::ParamRecord(SP_RZA_ALGMNG, 132, Text::ControlVoltageSection::RZA_ALGMNG132, true));

	report->insertTable(table);
}
