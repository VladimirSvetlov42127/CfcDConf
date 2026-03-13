#include "SectionVoltageAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_U_SECT_WORDIN_PARAM);
}

namespace Text {
	const QString Byte0 = "Учитывать чередование фаз";

	const QString Float0 = "Минимум напряжения секции (В)";
}

SectionVoltageAlgorithm::SectionVoltageAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes, 0, false)
{
}

QWidget* SectionVoltageAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addCheckEditor(bytesParam, alg_index(i, 0), Text::Byte0);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0,  10000);//);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void SectionVoltageAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> float0;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte0 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 0), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
	}

	table->addRow(Text::Byte0, byte0);
	table->addRow(Text::Float0, float0);
}
