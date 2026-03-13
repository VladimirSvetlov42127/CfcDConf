#include "ZMNAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_ZMN_1_WORDIN_PARAM, SP_ZMN_MKC_WORDIN_PARAM);
}

namespace Text {	
	const QString Byte3 = "Напряжение";
	const QString Byte4 = "Контроль РПВ для пуска";
	const QString Byte5 = "Контроль U соседней секции";
	const QString Byte6 = "Кол-во фаз срабатывания";
}

ZMNAlgorithm::ZMNAlgorithm(bool mks) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(mks).bytes),
	m_mks(mks),
	m_name(QString("ЗМН").append(mks ? "-МКС": ""))
{
}

QWidget* ZMNAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_mks).floats;
		uint16_t bytesParam = alg_addrs(m_mks).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::ActivateUrovTimer);
		em.addListEditor(bytesParam, alg_index(i, 3), Text::Byte3, Algs::VoltageTypeList);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Text::Byte5);
		em.addLineEditor(bytesParam, alg_index(i, 6), Text::Byte6, 1, 3);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationVoltage, 0, std::numeric_limits<float>::max());// 70);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, std::numeric_limits<float>::max());// 2);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);		

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void ZMNAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;
	QList<DcReportTable::ParamRecord> byte6;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_mks).floats;
		uint16_t bytesParam = alg_addrs(m_mks).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), Algs::VoltageTypeList.toHash());
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true);
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6));

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Text::Byte6, byte6);

	table->addRow(Algs::Text::ActuationVoltage, float0);	
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);
}
