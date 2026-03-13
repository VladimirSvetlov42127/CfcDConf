#include "MTZ_3IoAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_MTZIo_WORDIN_PARAM, SP_MTZ2Io_WORDIN_PARAM, SP_MTZ3Io_WORDIN_PARAM);
}

namespace Text {
	const QString Float5 = "Уровень срабатывания по 3Uo (В)";
}

MTZ_3IoAlgorithm::MTZ_3IoAlgorithm(int idx) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(idx).bytes),
	m_idx(idx),
	m_name(QString("ТЗНП %1-я ступень").arg(idx + 1))
{
}

QWidget* MTZ_3IoAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::DirectionalMode);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Algs::Text::ActivateUrovTimer);
		em.addListEditor(bytesParam, alg_index(i, 4), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);
		em.addListEditor(bytesParam, alg_index(i, 5), Algs::Text::APV, Algs::ApvTypeList);

        if (controller()->paramsRegistry().element(SP_AQ_FLOAT_PARAM, 0)) {
            em.addListEditor(bytesParam, alg_index(i, 6), Algs::Text::AvtoKvitType, Algs::AutoKvitTypeList);
        }

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 75);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::AngleMaxSensitivity, 0, 359.999);
		em.addLineEditor(floatsParam, alg_index(i, 4), Algs::Text::ActuationAngle, 0, 359.999);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0, std::numeric_limits<float>::max());// 70);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

bool MTZ_3IoAlgorithm::checkForAvailable() const
{
	if (DcController::P_SC == controller()->type())
		return false;

	return Board_Algorithm::checkForAvailable();
}

void MTZ_3IoAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;
	QList<DcReportTable::ParamRecord> byte6;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_idx).floats;
		uint16_t bytesParam = alg_addrs(m_idx).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), Algs::DependencyTypeList.toHash());
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), Algs::ApvTypeList.toHash());
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), Algs::AutoKvitTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0)); 
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1)); 
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2)); 
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3)); 
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4)); 
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5)); 
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::DirectionalMode, byte2);
	table->addRow(Algs::Text::ActivateUrovTimer, byte3);
	table->addRow(Algs::Text::DependencyAmperage, byte4);
	table->addRow(Algs::Text::APV, byte5);
	table->addRow(Algs::Text::AvtoKvitType, byte6);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ReturnRate, float1);
	table->addRow(Algs::Text::ActuationTime, float2);
	table->addRow(Algs::Text::AngleMaxSensitivity, float3);
	table->addRow(Algs::Text::ActuationAngle, float4);
	table->addRow(Text::Float5, float5);
}
