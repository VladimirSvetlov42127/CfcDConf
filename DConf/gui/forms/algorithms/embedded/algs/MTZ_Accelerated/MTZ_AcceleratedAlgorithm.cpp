#include "MTZ_AcceleratedAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_ACS_WORDIN_PARAM)
}

namespace Text {
	const QString Byte3 = "Блокир. по 2-й гарм.";

	const QString Float3 = "Время активности (с)";
	const QString Float4 = "Уровень блокир. по 2-й гарм.";
}

MTZ_AcceleratedAlgorithm::MTZ_AcceleratedAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* MTZ_AcceleratedAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::ActivateUrovTimer);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Text::Byte3);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Algs::Text::LZSH);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, 5);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0, 3);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void MTZ_AcceleratedAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Algs::Text::LZSH, byte4);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
}
