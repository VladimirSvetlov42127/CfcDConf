#include "AUVAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_XCBR_CSWI_WORDIN_PARAM);
}

namespace Text {
	const QString Float0 = "Время диагностики НЦУ (с)";
	const QString Float1 = "Время неуспешного ВКЛ/ОТКЛ (с)";
}

AUVAlgorithm::AUVAlgorithm() :
	IAlgorithm(alg_addrs().floats, 0, false)
{
}

QWidget* AUVAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		
		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0, std::numeric_limits<float>::max());//);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0, std::numeric_limits<float>::max());//);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void AUVAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
	}

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
}
