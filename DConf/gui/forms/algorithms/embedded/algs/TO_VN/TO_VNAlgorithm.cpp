#include "TO_VNAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_TO_HV_WORDIN_PARAM)
		
	const ListEditorContainer g_ProtectionTypeList = { "На сигнал", "На выключатель" };
	const ListEditorContainer g_AmperageModeList = { "Измереные", "Приведённые" };
}

namespace Text {
	const QString Byte1 = "Действие на выкл. стороны ВН";
	const QString Byte3 = "Токи";
}

TO_VNAlgorithm::TO_VNAlgorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* TO_VNAlgorithm::createWidget() const
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

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0);		
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void TO_VNAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), g_ProtectionTypeList.toHash());
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), g_AmperageModeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Algs::Text::ActivateUrovTimer, byte2);
	table->addRow(Text::Byte3, byte3);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
}
