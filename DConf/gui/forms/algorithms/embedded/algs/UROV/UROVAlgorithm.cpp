#include "UROVAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_UROV_WORDIN_PARAM);
}

namespace Text {
	const QString Byte0 = "Контроль пуска МТЗ";

	const QString Float3 = "Задержка по входу 1 (с)";
	const QString Float4 = "Задержка по входу 2 (с)";
    const QString Float5 = "Задержка фиксации неисправности (с)";
}

UROVAlgorithm::UROVAlgorithm() :
	IAlgorithm(alg_addrs().bytes, 0, false)
{
}

QWidget* UROVAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addCheckEditor(bytesParam, alg_index(i, 0), Text::Byte0, 0);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);		
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0, std::numeric_limits<float>::max());// 300);
        em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0, std::numeric_limits<float>::max());// 300);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void UROVAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte0 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 0), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
	}

	table->addRow(Text::Byte0, byte0);
	table->addRow(Algs::Text::ActuationTime, float0);
	table->addRow(Algs::Text::ActuationAmperage, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
}
