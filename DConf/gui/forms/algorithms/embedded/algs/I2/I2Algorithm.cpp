#include "I2Algorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_I2_WORDIN_PARAM);
}

namespace Text {
	const QString Unblock = "Ток разблокировки (А)";
}

I2Algorithm::I2Algorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* I2Algorithm::createWidget() const
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
		em.addListEditor(bytesParam, alg_index(i, 3), Algs::Text::APV, Algs::ApvTypeList);
		em.addListEditor(bytesParam, alg_index(i, 4), Algs::Text::ActuationType, Algs::ThresholdTypeList);
		em.addListEditor(bytesParam, alg_index(i, 5), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 58);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Unblock, 0, std::numeric_limits<float>::max());// 600);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void I2Algorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), Algs::ApvTypeList.toHash());
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), Algs::ThresholdTypeList.toHash());
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), Algs::DependencyTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Algs::Text::APV, byte3);
	table->addRow(Algs::Text::ActuationType, byte4);
	table->addRow(Algs::Text::DependencyAmperage, byte5);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);
	table->addRow(Text::Unblock, float3);
}
