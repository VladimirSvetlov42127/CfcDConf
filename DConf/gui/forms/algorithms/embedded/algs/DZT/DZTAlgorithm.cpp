#include "DZTAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_DZT_WORDIN_PARAM)

	const ListEditorContainer g_ProtectionTypeList = { "На сигнал", "На выключатель" };
}

namespace Text {
	const QString Byte1 = "Действие на выкл. стороны ВН";	
	const QString Byte3 = "Блокировка по 2-й гармонике";
	const QString Byte4 = "Кросс блокировка по 2-й гармонике";
	const QString Byte5 = "Блокировка по 5-й гармонике";
	const QString Byte6 = "Кросс блокировка по 5-й гармонике";
	const QString Byte7 = "Загрублять ДЗТ по КЦТ";
	const QString Byte8 = "Вывод ДЗТ по КЦТ";

	const QString Float3 = "Ток срабатывания, грубый";
	const QString Float4 = "Ток начала торможения (о.е. Iном)";
	const QString Float5 = "К торможения 1-го участка";
	const QString Float6 = "Ток второго перегиба (о.е. Iном)";
	const QString Float7 = "К торможения 2-го участка";
	const QString Float8 = "Уровень блокир. по 2-й гарм.";
	const QString Float9 = "Время блокир. по 2-й гарм.";
	const QString Float10 = "Уровень блокир. по 5-й гарм.";
	const QString Float11 = "Время блокир. по 5-й гарм.";
}

DZTAlgorithm::DZTAlgorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* DZTAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addListEditor(bytesParam, alg_index(i, 1), Text::Byte1, g_ProtectionTypeList);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::ActivateUrovTimer);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Text::Byte3);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Text::Byte5);
		em.addCheckEditor(bytesParam, alg_index(i, 6), Text::Byte6);
		em.addCheckEditor(bytesParam, alg_index(i, 7), Text::Byte7);
		em.addCheckEditor(bytesParam, alg_index(i, 8), Text::Byte8);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 6), Text::Float6, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 7), Text::Float7, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 8), Text::Float8, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 9), Text::Float9, 0,  std::numeric_limits<float>::max());//10);
		em.addLineEditor(floatsParam, alg_index(i, 10), Text::Float10, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 11), Text::Float11, 0, std::numeric_limits<float>::max());//10);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void DZTAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;
	QList<DcReportTable::ParamRecord> byte6;
	QList<DcReportTable::ParamRecord> byte7;
	QList<DcReportTable::ParamRecord> byte8;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;
	QList<DcReportTable::ParamRecord> float6;
	QList<DcReportTable::ParamRecord> float7;
	QList<DcReportTable::ParamRecord> float8;
	QList<DcReportTable::ParamRecord> float9;
	QList<DcReportTable::ParamRecord> float10;
	QList<DcReportTable::ParamRecord> float11;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), g_ProtectionTypeList.toHash());
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true);
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), "", true);
		byte7 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 7), "", true);
		byte8 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 8), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
		float6 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 6));
		float7 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 7));
		float8 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 8));
		float9 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 9));
		float10 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 10));
		float11 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 11));
	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Text::Byte6, byte6);
	table->addRow(Text::Byte7, byte7);
	table->addRow(Text::Byte8, byte8);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);
	table->addRow(Text::Float6, float6);
	table->addRow(Text::Float7, float7);
	table->addRow(Text::Float8, float8);
	table->addRow(Text::Float9, float9);
	table->addRow(Text::Float10, float10);
	table->addRow(Text::Float11, float11);
}
