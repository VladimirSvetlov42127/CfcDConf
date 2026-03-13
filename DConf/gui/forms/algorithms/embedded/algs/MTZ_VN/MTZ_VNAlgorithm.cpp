#include "MTZ_VNAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_MTZ_HV_WORDIN_PARAM)
		
	const ListEditorContainer g_ProtectionTypeList = { "На сигнал", "На выключатель" };
	const ListEditorContainer g_AmperageModeList = { "Измереные", "Приведённые" };
}

namespace Text {
	const QString Byte1 = "Действие на выкл. стороны ВН";
	const QString Byte3 = "Токи";
	const QString Byte4 = "Ускорение по РПВ ВН";
	const QString Byte5 = "Разр. пуск от напр. НН";
	const QString Byte7 = "Разрешено АПВ";

	const QString Float3 = "Ток сраб. чувствит.";
	const QString Float4 = "Время сраб. чуствит.";
	const QString Float5 = "Время сраб. ускоренное";
}

MTZ_VNAlgorithm::MTZ_VNAlgorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* MTZ_VNAlgorithm::createWidget() const
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
		em.addListEditor(bytesParam, alg_index(i, 3), Text::Byte3, g_AmperageModeList);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Text::Byte5);
		em.addListEditor(bytesParam, alg_index(i, 6), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);
		em.addCheckEditor(bytesParam, alg_index(i, 7), Text::Byte7);

        em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void MTZ_VNAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;
	QList<DcReportTable::ParamRecord> byte6;
	QList<DcReportTable::ParamRecord> byte7;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), g_ProtectionTypeList.toHash());
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), g_AmperageModeList.toHash());
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true);
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), Algs::DependencyTypeList.toHash());
		byte7 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 7), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Algs::Text::DependencyAmperage, byte6);
	table->addRow(Text::Byte7, byte7);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);
}
