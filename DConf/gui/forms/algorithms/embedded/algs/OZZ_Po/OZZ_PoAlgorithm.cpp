#include "OZZ_PoAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_OZZ_Po_WORDIN_PARAM, SP_OZZ_Po2_WORDIN_PARAM);

	const ListEditorContainer g_SignList = { "+", "-" };
}

namespace Text {
	const QString Byte2 = "Знак активной мощности";

	const QString Float0 = "Порог Po начала срабатывания (Вт)";
	const QString Float1 = "Порог 3Uo начала срабатывания (В)";
	const QString Float2 = "Кф-т срабатывания по мощности Ро";
	const QString Float4 = "Кф-т возврата по Ро";
	const QString Float5 = "Кф-т возврата по Uo";
}

OZZ_PoAlgorithm::OZZ_PoAlgorithm(int idx) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(idx).bytes),
	m_idx(idx),
	m_name(QString("ОЗЗ по Po %1-я ступень").arg(idx + 1))
{
}

QWidget* OZZ_PoAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addListEditor(bytesParam, alg_index(i, 2), Text::Byte2, g_SignList);

        if (controller()->paramsRegistry().element(SP_AQ_FLOAT_PARAM, 0)) {
            em.addListEditor(bytesParam, alg_index(i, 3), Algs::Text::AvtoKvitType, Algs::AutoKvitTypeList);
        }

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0, std::numeric_limits<float>::max());// 100000);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0, std::numeric_limits<float>::max());// 100000);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 200);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void OZZ_PoAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), g_SignList.toHash());
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), Algs::AutoKvitTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Algs::Text::AvtoKvitType, byte3);

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Algs::Text::ActuationTime, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);
}
