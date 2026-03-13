#include "DcIEC101SettingsForm.h"

#include <qtabwidget.h>
#include <qscrollarea.h>

#include <gui/editors/EditorsManager.h>

namespace {
	const QList<int> DinAsduTypeList1 = { 1, 3, 0 };
	const QList<int> DinAsduTypeList2 = { 30, 31, 0 };

	const QList<int> AinAsduTypeList1 = { 13, 0 };
	const QList<int> AinAsduTypeList2 = { 13, 14, 36, 0 }; 

	const QList<int> CinAsduTypeList = { 15, 16, 37, 0 };

	ListEditorContainer makeEditorList(const QList<int> &ints) {
		ListEditorContainer res;
		for (auto &i : ints)
			res.append({ i ? QString::number(i) : "Не используется", i });

		return res;
	}	
}

namespace Text {
	namespace Settings {
		const QString Title = "Параметры протокола";

		const QString IEC_104_PARAMS0 = "Таймаут при открытии соединения";
		const QString IEC_104_PARAMS1 = "Таймаут при посылке или тестировании APDU";
		const QString IEC_104_PARAMS2 = "Таймаут для подтверждения без сообщения данных";
		const QString IEC_104_PARAMS3 = "Таймаут блоков тестирования в случае длительного простоя";
		const QString IEC_104_PARAMS4 = "Глубина доверия при передаче (k APDU)";
		const QString IEC_104_PARAMS5 = "Глубина доверия на приёме (w APDU)";
		const QString IEC_104_PARAMS6 = "Адрес станции (общий адрес ASDU)";
		const QString IEC_104_PARAMS10 = "Номер порта";
		const QString IEC_104_PARAMS11 = "Таймаут активности на открытом соединении (сек)";
		const QString IEC_104_PARAMS12 = "Количество посылаемых 'пачкой' TESTFR act";

		const QString IEC_104_PARAMS7 = "Длина общего адреса ASDU для 104, байт (для 101 в настройках RS485)";
		const QString IEC_104_PARAMS8 = "Длина причины передачи для 104, байт (для 101 в настройках RS485)";
		const QString IEC_104_PARAMS9 = "Длина адреса объекта информации для 104, байт (для 101 в настройках RS485)";

		const QVariant IEC_104_PARAMS7_Value = 2;
		const QVariant IEC_104_PARAMS8_Value = 2;
		const QVariant IEC_104_PARAMS9_Value = 3;
	}

	namespace Assingments {
		const QString Title = "Назначения параметров";

		const QString DinTitle = "Дискретный вход/ТС/DIN";
		const QString AinTitle = "Аналоговый вход/ТИ/AIN";
		const QString CinTitle = "Счётный вход/интегральные суммы/СIN";
		const QString DoutTitle = "Дискретный выход/ТУ/DOUT";

		const QString IEC_104_PARAMS13 = "Длина кольцевого буфера аналогов";
		const QString IEC_104_ASDU_ASSIGN0 = "Адрес объекта";
		const QString IEC_104_ASDU_ASSIGN1 = "Тип ASDU при опросе";
		const QString IEC_104_ASDU_ASSIGN2 = "Тип ASDU при спонтанной передаче";
		const QString IEC_104_ASDU_ASSIGN3 = "Тип ASDU при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN4 = "Период выдачи при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN256 = "Адрес объекта";
		const QString IEC_104_ASDU_ASSIGN257 = "Тип ASDU при опросе";
		const QString IEC_104_ASDU_ASSIGN258 = "Тип ASDU при спонтанной передаче";
		const QString IEC_104_ASDU_ASSIGN259 = "Тип ASDU при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN260 = "Период выдачи при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN512 = "Адрес объекта";
		const QString IEC_104_ASDU_ASSIGN513 = "Тип ASDU при опросе";
		const QString IEC_104_ASDU_ASSIGN514 = "Тип ASDU при спонтанной передаче";
		const QString IEC_104_ASDU_ASSIGN515 = "Тип ASDU при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN516 = "Период выдачи при циклической передаче";
		const QString IEC_104_ASDU_ASSIGN768 = "Адрес объекта";
		const QString IEC_104_ASDU_ASSIGN1280 = "Передача файлов";
	}
}

DcIEC101SettingsForm::DcIEC101SettingsForm(DcController *controller) :
	DcForm(controller,  "Параметры МЭК60870-5-101/104", false)
{
	QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

	QTabWidget *tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createProtocolSettingsTab(), Text::Settings::Title);
	tabWidget->addTab(createProtocolAssingmentsTab(), Text::Assingments::Title);
}

bool DcIEC101SettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_IEC_104_PARAMS},
		{SP_IEC_104_ASDU_ASSIGN}
	};

	return hasAny(controller, params);
}

void DcIEC101SettingsForm::fillReport(DcIConfigReport * report)
{
	auto device = report->device();

	report->insertSection(Text::Settings::Title);
	DcReportTable table(device);
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 0, Text::Settings::IEC_104_PARAMS0));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 1, Text::Settings::IEC_104_PARAMS1));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 2, Text::Settings::IEC_104_PARAMS2));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 3, Text::Settings::IEC_104_PARAMS3));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 4, Text::Settings::IEC_104_PARAMS4));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 5, Text::Settings::IEC_104_PARAMS5));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 6, Text::Settings::IEC_104_PARAMS6));
	table.addRow({ Text::Settings::IEC_104_PARAMS7, Text::Settings::IEC_104_PARAMS7_Value.toString() });
	table.addRow({ Text::Settings::IEC_104_PARAMS8, Text::Settings::IEC_104_PARAMS8_Value.toString() });
	table.addRow({ Text::Settings::IEC_104_PARAMS9, Text::Settings::IEC_104_PARAMS9_Value.toString() });
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 10, Text::Settings::IEC_104_PARAMS10));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 11, Text::Settings::IEC_104_PARAMS11));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 12, Text::Settings::IEC_104_PARAMS12));
	report->insertTable(table);

	report->addSectionPrefix(Text::Assingments::Title);
	report->insertSection();
	table.clear();
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_PARAMS, 13, Text::Assingments::IEC_104_PARAMS13));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 1280, Text::Assingments::IEC_104_ASDU_ASSIGN1280));
	report->insertTable(table);

	report->insertSection(Text::Assingments::DinTitle);
	table.clear();
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 0, Text::Assingments::IEC_104_ASDU_ASSIGN0));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 1, makeEditorList(DinAsduTypeList1).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN1));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 2, makeEditorList(DinAsduTypeList2).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN2));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 3, makeEditorList(DinAsduTypeList1).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN3));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 4, Text::Assingments::IEC_104_ASDU_ASSIGN4));
	report->insertTable(table);

	report->insertSection(Text::Assingments::AinTitle);
	table.clear();
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 256, Text::Assingments::IEC_104_ASDU_ASSIGN256));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 257, makeEditorList(AinAsduTypeList1).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN257));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 258, makeEditorList(AinAsduTypeList2).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN258));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 259, makeEditorList(AinAsduTypeList1).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN259));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 260, Text::Assingments::IEC_104_ASDU_ASSIGN260));
	report->insertTable(table);

	report->insertSection(Text::Assingments::CinTitle);
	table.clear();
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 512, Text::Assingments::IEC_104_ASDU_ASSIGN512));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 513, makeEditorList(CinAsduTypeList).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN513));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 514, makeEditorList(CinAsduTypeList).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN514));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 515, makeEditorList(CinAsduTypeList).toHash(), Text::Assingments::IEC_104_ASDU_ASSIGN515));
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 516, Text::Assingments::IEC_104_ASDU_ASSIGN516));
	report->insertTable(table);

	report->insertSection(Text::Assingments::DoutTitle);
	table.clear();
	table.addRow(DcReportTable::ParamRecord(SP_IEC_104_ASDU_ASSIGN, 768, Text::Assingments::IEC_104_ASDU_ASSIGN768));
	report->insertTable(table);

	report->takeSecionPrefix();
}

QWidget * DcIEC101SettingsForm::createProtocolSettingsTab() const
{
    auto param = controller()->paramsRegistry().element(SP_IEC_104_PARAMS, 0);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);

	manager.addLineEditor(SP_IEC_104_PARAMS, 0, Text::Settings::IEC_104_PARAMS0);
	manager.addLineEditor(SP_IEC_104_PARAMS, 1, Text::Settings::IEC_104_PARAMS1);
	manager.addLineEditor(SP_IEC_104_PARAMS, 2, Text::Settings::IEC_104_PARAMS2);
	manager.addLineEditor(SP_IEC_104_PARAMS, 3, Text::Settings::IEC_104_PARAMS3);
	manager.addLineEditor(SP_IEC_104_PARAMS, 4, Text::Settings::IEC_104_PARAMS4);
	manager.addLineEditor(SP_IEC_104_PARAMS, 5, Text::Settings::IEC_104_PARAMS5);
	manager.addLineEditor(SP_IEC_104_PARAMS, 6, Text::Settings::IEC_104_PARAMS6);

	manager.addLineConst(Text::Settings::IEC_104_PARAMS7, Text::Settings::IEC_104_PARAMS7_Value);
	manager.addLineConst(Text::Settings::IEC_104_PARAMS8, Text::Settings::IEC_104_PARAMS8_Value);
	manager.addLineConst(Text::Settings::IEC_104_PARAMS9, Text::Settings::IEC_104_PARAMS9_Value);

	manager.addLineEditor(SP_IEC_104_PARAMS, 10, Text::Settings::IEC_104_PARAMS10);
	manager.addLineEditor(SP_IEC_104_PARAMS, 11, Text::Settings::IEC_104_PARAMS11);
	manager.addLineEditor(SP_IEC_104_PARAMS, 12, Text::Settings::IEC_104_PARAMS12);

	manager.addStretch();
	return tab;
}

QWidget * DcIEC101SettingsForm::createProtocolAssingmentsTab() const
{
    auto param = controller()->paramsRegistry().element(SP_IEC_104_ASDU_ASSIGN, 0);
	if (!param)
		return nullptr;

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);
	
	manager.addLineEditor(SP_IEC_104_PARAMS, 13, Text::Assingments::IEC_104_PARAMS13);
	manager.addLineEditor(SP_IEC_104_ASDU_ASSIGN, 1280, Text::Assingments::IEC_104_ASDU_ASSIGN1280);

	auto groupManager = manager.addGroupBox(Text::Assingments::DinTitle, 0, 1, 3);
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 0, Text::Assingments::IEC_104_ASDU_ASSIGN0);
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 1, Text::Assingments::IEC_104_ASDU_ASSIGN1, makeEditorList(DinAsduTypeList1));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 2, Text::Assingments::IEC_104_ASDU_ASSIGN2, makeEditorList(DinAsduTypeList2));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 3, Text::Assingments::IEC_104_ASDU_ASSIGN3, makeEditorList(DinAsduTypeList1));
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 4, Text::Assingments::IEC_104_ASDU_ASSIGN4);

	groupManager = manager.addGroupBox(Text::Assingments::AinTitle, 0, 1, 3);
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 256, Text::Assingments::IEC_104_ASDU_ASSIGN256);
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 257, Text::Assingments::IEC_104_ASDU_ASSIGN257, makeEditorList(AinAsduTypeList1));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 258, Text::Assingments::IEC_104_ASDU_ASSIGN258, makeEditorList(AinAsduTypeList2));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 259, Text::Assingments::IEC_104_ASDU_ASSIGN259, makeEditorList(AinAsduTypeList1));
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 260, Text::Assingments::IEC_104_ASDU_ASSIGN260);

	groupManager = manager.addGroupBox(Text::Assingments::CinTitle, 0, 1, 3);
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 512, Text::Assingments::IEC_104_ASDU_ASSIGN512);
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 513, Text::Assingments::IEC_104_ASDU_ASSIGN513, makeEditorList(CinAsduTypeList));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 514, Text::Assingments::IEC_104_ASDU_ASSIGN514, makeEditorList(CinAsduTypeList));
	groupManager->addListEditor(SP_IEC_104_ASDU_ASSIGN, 515, Text::Assingments::IEC_104_ASDU_ASSIGN515, makeEditorList(CinAsduTypeList));
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 516, Text::Assingments::IEC_104_ASDU_ASSIGN516);
	
	groupManager = manager.addGroupBox(Text::Assingments::DoutTitle, 0, 1, 3);
	groupManager->addLineEditor(SP_IEC_104_ASDU_ASSIGN, 768, Text::Assingments::IEC_104_ASDU_ASSIGN768);
	manager.addStretch();

	QScrollArea *scrollArea = new QScrollArea;
	scrollArea->setFrameShape(QFrame::NoFrame);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(tab);

	return scrollArea;
}
