#include "AMTZAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_AMTZ_WORDIN_PARAM, SP_AMTZIo_WORDIN_PARAM)
}

namespace Text {
	const QString Float0 = "Время усреднения (с)";
	const QString Float1 = "Порог dI срабатывания (A)";
	const QString Float2 = "Время авт. возврата (с)";
	const QString Float3 = "Ток срабатывания (А)";
}

AMTZAlgorithm::AMTZAlgorithm(bool Io) :
	IAlgorithm(alg_addrs(Io).bytes), 
	m_Io(Io)
{
}

QString AMTZAlgorithm::name() const
{
	return m_Io ? "АМТЗ Io" : "АМТЗ";
}

QWidget* AMTZAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_Io).floats;
		uint16_t bytesParam = alg_addrs(m_Io).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

bool AMTZAlgorithm::checkForAvailable() const
{
	if (controller()->type() == DcController::P_SC)
		return false;

	return true;
}

void AMTZAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_Io).floats;
		uint16_t bytesParam = alg_addrs(m_Io).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
}
