#include "MTZ_SN_Algorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
    ALG_ADDRS_FUNC(SP_MTZ_MV_WORDIN_PARAM)

	const ListEditorContainer g_ProtectionTypeList = { "На сигнал", "На выключатель" };
	const ListEditorContainer g_UrovTypeList = { "Нет", "Активирует" };
	const ListEditorContainer g_ITypeList = { "Измеряемый", "Приведенный" };
	const ListEditorContainer g_AccelerTypeList = { "Нет", "Введено" };
	const ListEditorContainer g_AproveTypeList = { "Нет", "Разрешен" };
}

namespace Text {
	const QString Byte1 = "Действие на выключатель";
	const QString Byte2 = "Формирует сигнал УРОВ";
	const QString Byte3 = "Ток Измеряемый/Приведённый";
	const QString Byte4 = "Ускорение по РПВ СН";
	const QString Byte5 = "Разр. пуск от напр. СН";
	const QString Byte8 = "Действие на выключатель стороны ВН";

	const QString Float0 = "Порог срабатывания";
	const QString Float1 = "Время срабатывания, с";
	const QString Float2 = "Коэффициент возврата";
	const QString Float3 = "Время срабатывания ускоренное";
	const QString Float4 = "Задержка отключения стороны ВН";
}

MTZ_SN_Algorithm::MTZ_SN_Algorithm() :
    Board_Algorithm(BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* MTZ_SN_Algorithm::createWidget() const
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
		em.addListEditor(bytesParam, alg_index(i, 6), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);
		em.addListEditor(bytesParam, alg_index(i, 7), Algs::Text::APV, Algs::ApvTypeList);
		em.addListEditor(bytesParam, alg_index(i, 8), Text::Byte8, g_AproveTypeList);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0.10, 50000);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0.02, 100);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0.6, 0.99);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0.02, 100);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0.02, 100);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void MTZ_SN_Algorithm::fillReport(DcReportTable * table) const
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

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), g_ProtectionTypeList.toHash());
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), g_UrovTypeList.toHash());
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), g_ITypeList.toHash());
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), g_AccelerTypeList.toHash());
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), g_AproveTypeList.toHash());
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), Algs::DependencyTypeList.toHash());
		byte7 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 7), Algs::ApvTypeList.toHash());
		byte8 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 8), g_AproveTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Algs::Text::DependencyAmperage, byte6);
	table->addRow(Algs::Text::APV, byte7);
	table->addRow(Text::Byte8, byte8);

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
}
