#include "AVRAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_AVR_WORDIN_PARAM);

	const ListEditorContainer g_AVRModeList = { "АВР Выведен", "АВР СВ", "АВР ВВОД" };
}

namespace Text {
	const QString Byte0 = "Режим работы АВР";

	const QString Float0 = "Время АВР (с)";
}

AVRAlgorithm::AVRAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes, 0, false)
{
}

QWidget* AVRAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addListEditor(bytesParam, alg_index(i, 0), Text::Byte0, g_AVRModeList);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0,  std::numeric_limits<float>::max());//300);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void AVRAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> float0;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte0 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 0), g_AVRModeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
	}

	table->addRow(Text::Byte0, byte0);
	table->addRow(Text::Float0, float0);
}
