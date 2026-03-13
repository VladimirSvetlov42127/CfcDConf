#include "LZSHAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_LZSH_WORDIN_PARAM);
}

namespace Text {
	const QString Byte4 = "Разрешить при неисправной блокировке";

	const QString Float3 = "Время диагностики неисправности ЛЗШ (с)";
}

LZSHAlgorithm::LZSHAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* LZSHAlgorithm::createWidget() const
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
		em.addCheckEditor(bytesParam, alg_index(i, 3), Algs::Text::LZSH);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 5);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0, std::numeric_limits<float>::max());// 60);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void LZSHAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;

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
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Algs::Text::LZSH, byte3);
	table->addRow(Text::Byte4, byte4);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);	
	table->addRow(Text::Float3, float3);
}
