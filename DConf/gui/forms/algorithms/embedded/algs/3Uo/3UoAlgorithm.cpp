#include "3UoAlgorithm.h"

#include <qtableview.h>

#include <dpc/gui/widgets/CheckableHeaderView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

#include <gui/editors/EditorsManager.h>

using namespace Dpc::Gui;

namespace {
	ALG_ADDRS_FUNC(SP_OZZ_WORDIN_PARAM);
}

T3UoAlgorithm::T3UoAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* T3UoAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addListEditor(bytesParam, alg_index(i, 2), Algs::Text::APV, Algs::ApvTypeList);

        if (controller()->paramsRegistry().element(SP_AQ_FLOAT_PARAM, 0)) {
            em.addListEditor(bytesParam, alg_index(i, 3), Algs::Text::AvtoKvitType, Algs::AutoKvitTypeList);
        }

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationVoltage, 0, std::numeric_limits<float>::max());// 70);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

bool T3UoAlgorithm::checkForAvailable() const
{
	if (DcController::P_SC == controller()->type())
		return false;

	return Board_Algorithm::checkForAvailable();
}

void T3UoAlgorithm::fillReport(DcReportTable * table) const
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

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), Algs::ApvTypeList.toHash());
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), Algs::AutoKvitTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::APV, byte2);
	table->addRow(Algs::Text::AvtoKvitType, byte3);

	table->addRow(Algs::Text::ActuationVoltage, float0);
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);
}
