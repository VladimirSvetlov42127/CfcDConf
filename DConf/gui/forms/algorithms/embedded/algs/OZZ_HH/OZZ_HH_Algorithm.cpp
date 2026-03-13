#include "OZZ_HH_Algorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_OZZ_HH_WORDIN_PARAM);

}

namespace Text {
	const QString Byte1 = "Действие на выключатель";
	const QString Byte2 = "Учёт направления мощности";
	const QString Byte3 = "Учёт уровня основной гармоники";

	const QString Float3 = "Ток блокировки по основной гармонике";
	const QString Float4 = "Кф-т возврата блокир. по основной гармонике";
}

OZZ_HH_Algorithm::OZZ_HH_Algorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* OZZ_HH_Algorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Text::Byte1);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Text::Byte2);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Text::Byte3);

        if (controller()->paramsRegistry().element(SP_AQ_FLOAT_PARAM, 0)) {
            em.addListEditor(bytesParam, alg_index(i, 4), Algs::Text::AvtoKvitType, Algs::AutoKvitTypeList);
        }

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void OZZ_HH_Algorithm::fillReport(DcReportTable * table) const
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
	QList<DcReportTable::ParamRecord> float5;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), Algs::AutoKvitTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Algs::Text::AvtoKvitType, byte4);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
}
