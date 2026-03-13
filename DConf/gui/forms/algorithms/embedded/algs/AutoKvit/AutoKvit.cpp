#include "AutoKvit.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_AQ_WORDIN_PARAM)
}

namespace Text {
	const QString Float0 = "Задержка при востановлении нормально режима (с)";
	const QString Float1 = "Задержка после снижения 3Uo (c)";
	const QString Float2 = "Время суток(от начала суток) (с)";
	const QString Float3 = "Период (с)";
}

AutoKvit::AutoKvit() :
	IAlgorithm(alg_addrs().floats, 0, false)
{
}

QString AutoKvit::name() const
{
	return QString("Автоматическая квитация");
}

QWidget* AutoKvit::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0,  std::numeric_limits<float>::max());//86400);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void AutoKvit::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
	}

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
}