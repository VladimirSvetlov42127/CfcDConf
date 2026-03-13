#include "ZPNAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_ZPN_1_WORDIN_PARAM, SP_ZPN_2_WORDIN_PARAM);
}

namespace Text {
	const QString Byte3 = "Напряжение";
}

ZPNAlgorithm::ZPNAlgorithm(int idx) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(idx).bytes),
	m_idx(idx),
	m_name(QString("ЗПН %1-я ступень").arg(idx + 1))
{
}

QWidget* ZPNAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::ActivateUrovTimer);
		em.addListEditor(bytesParam, alg_index(i, 3), Text::Byte3, Algs::VoltageTypeList);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationVoltage, 0, std::numeric_limits<float>::max());// 350000);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 100);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void ZPNAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), Algs::VoltageTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);

	table->addRow(Algs::Text::ActuationVoltage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
}
