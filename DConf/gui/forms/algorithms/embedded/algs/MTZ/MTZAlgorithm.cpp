#include "MTZAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC_HASH( 
		{ MTZAlgorithm::N1, SP_MTZ_WORDIN_PARAM },
		{ MTZAlgorithm::N2, SP_MTZ2_WORDIN_PARAM },
		{ MTZAlgorithm::N3, SP_MTZ3_WORDIN_PARAM },
		{ MTZAlgorithm::N4, SP_MTZ4_WORDIN_PARAM }
	)
}

namespace Text {
	const QString Byte3 = "Формирует сигнал УРОВ";
	const QString Byte6 = "Вольтметровая блокировка";
    const QString Byte9 = "Ускорение по включению ВВ";

    const QString Float6 = "Время ускоренного срабатывания, с";
}

MTZAlgorithm::MTZAlgorithm(Type t) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(t).bytes),
	m_type(t)
{
}

QString MTZAlgorithm::name() const
{
	return QString("МТЗ %1-я ступень").arg(m_type + 1);
}

QWidget* MTZAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs(m_type).floats;
		uint16_t bytesParam = alg_addrs(m_type).bytes;
		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Algs::Text::LZSH);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Text::Byte3);
		em.addListEditor(bytesParam, alg_index(i, 4), Algs::Text::APV, Algs::ApvTypeList);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Algs::Text::DirectionalMode);
		em.addCheckEditor(bytesParam, alg_index(i, 6), Text::Byte6);
		em.addListEditor(bytesParam, alg_index(i, 7), Algs::Text::DependencyAmperage, Algs::DependencyTypeList);

        if (controller()->paramsRegistry().element(SP_AQ_FLOAT_PARAM, 0)) {
            em.addListEditor(bytesParam, alg_index(i, 8), Algs::Text::AvtoKvitType, Algs::AutoKvitTypeList);
        }
        em.addCheckEditor(bytesParam, alg_index(i, 9), Text::Byte9);

		em.addLineEditor(floatsParam, alg_index(i, 0), Algs::Text::ActuationAmperage, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0, std::numeric_limits<float>::max());// 300);
		em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ReturnRate, 0, 1);
		em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::AngleMaxSensitivity, 0, 359);
		em.addLineEditor(floatsParam, alg_index(i, 4), Algs::Text::ActuationAngle, 0, 359);
		em.addLineEditor(floatsParam, alg_index(i, 5), Algs::Text::BlockVoltage, 0, std::numeric_limits<float>::max());// 70);
        em.addLineEditor(floatsParam, alg_index(i, 6), Text::Float6, 0, std::numeric_limits<float>::max());// 70);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

bool MTZAlgorithm::checkForAvailable() const
{
	if (DcController::P_SC == controller()->type())
		return false;

	return Board_Algorithm::checkForAvailable();
}

void MTZAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;
	QList<DcReportTable::ParamRecord> byte6;
	QList<DcReportTable::ParamRecord> byte7;
	QList<DcReportTable::ParamRecord> byte8;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs(m_type).floats;
		uint16_t bytesParam = alg_addrs(m_type).bytes;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true); 
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true); 
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true); 
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), Algs::ApvTypeList.toHash());
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true); 
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), "", true); 
		byte7 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 7), Algs::DependencyTypeList.toHash());
		byte8 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 8), Algs::AutoKvitTypeList.toHash());

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(Algs::Text::LZSH, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Algs::Text::APV, byte4);
	table->addRow(Algs::Text::DirectionalMode, byte5);
	table->addRow(Text::Byte6, byte6);
	table->addRow(Algs::Text::DependencyAmperage, byte7);
	table->addRow(Algs::Text::AvtoKvitType, byte8);

	table->addRow(Algs::Text::ActuationAmperage, float0);
	table->addRow(Algs::Text::ActuationTime, float1);
	table->addRow(Algs::Text::ReturnRate, float2);
	table->addRow(Algs::Text::AngleMaxSensitivity, float3);
	table->addRow(Algs::Text::ActuationAngle, float4);
	table->addRow(Algs::Text::BlockVoltage, float5);
}
