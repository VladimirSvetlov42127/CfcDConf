#include "NBLNSAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_UBLNS_WORDIN_PARAM)
}

NBLNSAlgorithm::NBLNSAlgorithm() :
    Board_Algorithm(BoardType::AFE7 | BoardType::AFE11, alg_addrs().bytes)
{
}

QWidget* NBLNSAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

    for (int i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
        EditorsManager em(controller(), new QGridLayout(group));

        uint16_t floatsParam = alg_addrs().floats;
        em.addLineEditor(floatsParam, alg_index(i, 0), "НБЛНС: Порог срабатывания", 0, std::numeric_limits<float>::max());// 300);
        em.addLineEditor(floatsParam, alg_index(i, 1), "НБЛНС: Время срабатывания, с", 0, std::numeric_limits<float>::max());// 300);
        em.addLineEditor(floatsParam, alg_index(i, 2), "НБЛНС: Коэффициент возврата", 0, 1);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);

    return widget;
}

void NBLNSAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;

    for (int i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));	}

    table->addRow("НБЛНС: Порог срабатывания", float0);
    table->addRow("НБЛНС: Время срабатывания, с", float1);
    table->addRow("НБЛНС: Коэффициент возврата", float2);
}
