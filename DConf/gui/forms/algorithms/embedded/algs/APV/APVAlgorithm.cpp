#include "APVAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_APV_WORDIN_PARAM);
}

APVAlgorithm::APVAlgorithm() :
	IAlgorithm(alg_addrs().bytes, 0, false)
{
}

namespace Text {
	const QString Byte0 = "Количество циклов АПВ";
	const QString Byte1 = "АПВ по несоответствию";

	const QString Float0 = "Время готовности (с)";
	const QString Float1 = "Время АПВ, цикл 1 (с)";
	const QString Float2 = "Время АПВ, цикл 2 (с)";
}

QWidget* APVAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addLineEditor(bytesParam, alg_index(i, 0), Text::Byte0, 1, 2);
		em.addCheckEditor(bytesParam, alg_index(i, 1), Text::Byte1);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0,  std::numeric_limits<float>::max());//300);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0,  std::numeric_limits<float>::max());//300);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0,  std::numeric_limits<float>::max());//300);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void APVAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte0 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 0));
		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
	}

	table->addRow(Text::Byte0, byte0);
	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
}
