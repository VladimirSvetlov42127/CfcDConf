#include "KCTAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_KCT_WORDIN_PARAM)
}

KCTAlgorithm::KCTAlgorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* KCTAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

    for (int i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;

		em.addLineEditor(floatsParam, alg_index(i, 0), "КЦТ: Нижняя граница тормозного тока", 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 1), "КЦТ: Верхняя граница тормозного тока", 0, std::numeric_limits<float>::max());// 300);
		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void KCTAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;

    for (int i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
        float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1)); }

	table->addRow("КЦТ: Нижняя граница тормозного тока", float0);
	table->addRow("КЦТ: Верхняя граница тормозного тока", float1);
}
