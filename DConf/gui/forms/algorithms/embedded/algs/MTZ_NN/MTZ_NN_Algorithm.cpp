#include "MTZ_NN_Algorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_MTZ_LV_WORDIN_PARAM)

	const ListEditorContainer g_ProtectionTypeList = { "На сигнал", "На выключатель" };
	const ListEditorContainer g_UrovTypeList = { "Нет", "Активирует" };
	const ListEditorContainer g_ITypeList = { "Измеряемый", "Приведенный" };
	const ListEditorContainer g_AccelerTypeList = { "Нет", "Введено" };
	const ListEditorContainer g_AproveTypeList = { "Нет", "Разрешен" };
}

namespace Text {
	const QString Byte1 = "Действие на выключатель стороны НН";
	const QString Byte2 = "Формирует сигнал УРОВ";
	const QString Byte3 = "Ток Измеряемый/Приведённый";
	const QString Byte4 = "Ускорение по РПВ НН";
	const QString Byte5 = "Разр. пуск от напр. НН";
	const QString Byte6 = "Действие на выключатель стороны ВН";

	const QString Float0 = "Порог срабатывания";
	const QString Float1 = "Время срабатывания, с";
	const QString Float2 = "Коэффициент возврата";
	const QString Float3 = "Порог срабатывания чувствительный";
	const QString Float4 = "Время срабатывания по чувствительному";
	const QString Float5 = "Задержка отключения стороны ВН";
	const QString Float6 = "Время срабатывания ускоренное";
}

MTZ_NN_Algorithm::MTZ_NN_Algorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* MTZ_NN_Algorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addListEditor(bytesParam, alg_index(i, 1), Text::Byte1, g_ProtectionTypeList);
		em.addListEditor(bytesParam, alg_index(i, 2), Text::Byte2, g_UrovTypeList);
		em.addListEditor(bytesParam, alg_index(i, 3), Text::Byte3, g_ITypeList);
		em.addListEditor(bytesParam, alg_index(i, 4), Text::Byte4, g_AccelerTypeList);
		em.addListEditor(bytesParam, alg_index(i, 5), Text::Byte5, g_AproveTypeList);
		em.addListEditor(bytesParam, alg_index(i, 6), Text::Byte6, g_AproveTypeList);
		em.addListEditor(bytesParam, alg_index(i, 7), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);
		em.addListEditor(bytesParam, alg_index(i, 8), Algs::Text::APV, Algs::ApvTypeList);
	
		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0.10, 50000);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0.02, 100);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0.6, 0.99);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 10, 5000);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0.02, 100);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0.02, 100);
		em.addLineEditor(floatsParam, alg_index(i, 6), Text::Float6, 0.02, 100);

		layout->addWidget(group, 0, i);	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void MTZ_NN_Algorithm::fillReport(DcReportTable * table) const
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

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), g_ProtectionTypeList.toHash());
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), g_UrovTypeList.toHash());
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), g_ITypeList.toHash());
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), g_AccelerTypeList.toHash());
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), g_AproveTypeList.toHash());
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), g_AproveTypeList.toHash());
		byte7 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 7), Algs::DependencyTypeList.toHash());
		byte8 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 8), Algs::ApvTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
		float6 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 6));	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Text::Byte6, byte6);
	table->addRow(Algs::Text::DependencyAmperage, byte7);
	table->addRow(Algs::Text::APV, byte8);

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);
	table->addRow(Text::Float6, float6);
}
