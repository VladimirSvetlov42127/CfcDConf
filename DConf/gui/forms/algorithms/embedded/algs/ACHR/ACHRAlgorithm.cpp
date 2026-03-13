#include "ACHRAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_ACHR_1_WORDIN_PARAM, SP_ACHR_2_WORDIN_PARAM, SP_ACHR_C_WORDIN_PARAM)
}

namespace Text {
	const QString CHAPVTime = "Время ЧАПВ (с)";
	const QString KvBlockVoltage = "Кв блок. по напряжению";

	namespace T1 {
		const QString Byte2 = "Блокировка по dF/dt";
		const QString Byte3 = "Блокировка по фазному напряжению";

		const QString Float5 = "Уровень блокировки dF/dt (Гц/с)";
		const QString Float6 = "Кв блок. по dF/dt";
	}

	namespace T2 {
		const QString Byte3 = "Разрешение ускорения по напряжению";

		const QString Float2 = "Время до пуска (c)";
		const QString Float4 = "Напряжение ускорения (В)";
		const QString Float5 = "Tср при ускорении по напряжению (с)";
	}

	namespace TC {
		const QString Byte3 = "Разрешение блокировки по частоте";

		const QString Float0 = "Скорость срабатывания (Гц/с)";
		const QString Float2 = "Скорость возврата (Гц/с)";
		const QString Float4 = "Частота блокировки (Гц)";
	}

	const QString Byte2 = "Блокировка по напряжению";
	const QString Byte4 = "Разрешение ЧАПВ";
	const QString Byte5 = "Блокировка по нагрузке";
	const QString Byte6 = "Блокировка по направлению";

	const QString Float0 = "Частота срабатывания (Гц)";
	const QString Float1 = "Частота возврата (Гц)";
}

ACHRAlgorithm::ACHRAlgorithm(Type t) :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs(t).bytes),
	m_t(t),
	m_name(QString("АЧР-%1").arg(t == T_C ? "С" : QString::number(t + 1)))
{
}

QWidget* ACHRAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);
	
	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t bytesParam = alg_addrs(m_t).bytes;
		uint16_t floatsParam = alg_addrs(m_t).floats;

		em.addCheckEditor(bytesParam, alg_index(i, 1), Algs::Text::ActionOnSwitcher);
		em.addCheckEditor(bytesParam, alg_index(i, 2), m_t == T_1 ? Text::T1::Byte2 : Text::Byte2);
		em.addCheckEditor(bytesParam, alg_index(i, 3), m_t == T_1 ? Text::T1::Byte3 : (m_t == T_2 ? Text::T2::Byte3 : Text::TC::Byte3));
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Text::Byte5);
		em.addCheckEditor(bytesParam, alg_index(i, 6), Text::Byte6);

		
		if (m_t == T_1) {
			em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 45, 49, 1);
			em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 45, 50, 1);
			em.addLineEditor(floatsParam, alg_index(i, 2), Algs::Text::ActuationTime, 0.1, 5, 1);
			em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::BlockVoltage, 10, std::numeric_limits<float>::max() /*70*/, 0, "Фазное");
			em.addLineEditor(floatsParam, alg_index(i, 4), Text::KvBlockVoltage, 0.05, 1.5);
			em.addLineEditor(floatsParam, alg_index(i, 5), Text::T1::Float5, 0.01, 20);
			em.addLineEditor(floatsParam, alg_index(i, 6), Text::T1::Float6, 0.1, 0.9, 1);
			em.addLineEditor(floatsParam, alg_index(i, 7), Text::CHAPVTime, 60, std::numeric_limits<float>::max() /*300*/, 0);
		}
		else if (m_t == T_2) {
			em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 45, 49, 1);
			em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 45, 50, 1);
			em.addLineEditor(floatsParam, alg_index(i, 2), Text::T2::Float2, 1, 65, 1);
			em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::ActuationTime, 1, std::numeric_limits<float>::max() /*600*/, 1);
			em.addLineEditor(floatsParam, alg_index(i, 4), Text::T2::Float4, 10, std::numeric_limits<float>::max() /*1000000*/, 0, "Фазное");
			em.addLineEditor(floatsParam, alg_index(i, 5), Text::T2::Float5, 0.1, 5, 1);
			em.addLineEditor(floatsParam, alg_index(i, 6), Algs::Text::BlockVoltage, 10, std::numeric_limits<float>::max() /*1000000*/, 0, "Фазное");
			em.addLineEditor(floatsParam, alg_index(i, 7), Text::KvBlockVoltage, 1.05, 1.5);
			em.addLineEditor(floatsParam, alg_index(i, 8), Text::CHAPVTime, 60, std::numeric_limits<float>::max() /*300*/, 0);
		}
		else if (m_t == T_C) {
			em.addLineEditor(floatsParam, alg_index(i, 0), Text::TC::Float0, 0.1, 5, 1);
			em.addLineEditor(floatsParam, alg_index(i, 1), Algs::Text::ActuationTime, 0.04, 10);
			em.addLineEditor(floatsParam, alg_index(i, 2), Text::TC::Float2, 0.05, 5);
			em.addLineEditor(floatsParam, alg_index(i, 3), Algs::Text::BlockVoltage, 10, std::numeric_limits<float>::max() /*1000000*/, 0, "Фазное");
			em.addLineEditor(floatsParam, alg_index(i, 4), Text::TC::Float4, 40, 50, 1);
			em.addLineEditor(floatsParam, alg_index(i, 5), Text::KvBlockVoltage, 1.05, 2);
			em.addLineEditor(floatsParam, alg_index(i, 6), Text::CHAPVTime, 60, std::numeric_limits<float>::max() /*300*/, 0);
		}

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void ACHRAlgorithm::fillReport(DcReportTable * table) const
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
	QList<DcReportTable::ParamRecord> float6;
	QList<DcReportTable::ParamRecord> float7;
	QList<DcReportTable::ParamRecord> float8;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t bytesParam = alg_addrs(m_t).bytes;
		uint16_t floatsParam = alg_addrs(m_t).floats;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true);
		byte6 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 6), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
		float6 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 6));
		
		if (m_t == T_1 || m_t == T_2) {
			float7 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 7));
			if (m_t == T_2)
				float8 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 8));
		}
	}

	table->addRow(Algs::Text::ActionOnSwitcher, byte1);
	table->addRow(m_t == T_1 ? Text::T1::Byte2 : Text::Byte2, byte2);
	table->addRow(m_t == T_1 ? Text::T1::Byte3 : (m_t == T_2 ? Text::T2::Byte3 : Text::TC::Byte3), byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);
	table->addRow(Text::Byte6, byte6);

	table->addRow(m_t == T_C ? Text::TC::Float0 : Text::Float0, float0);
	table->addRow(m_t == T_C ? Algs::Text::ActuationTime : Text::Float1, float1);
	table->addRow(m_t == T_1 ? Algs::Text::ActuationTime : (m_t == T_2 ? Text::T2::Float2 : Text::TC::Float2), float2);
	table->addRow(m_t == T_2 ? Algs::Text::ActuationTime : Algs::Text::BlockVoltage, float3);
	table->addRow(m_t == T_1 ? Text::KvBlockVoltage : (m_t == T_2 ? Text::T2::Float4 : Text::TC::Float4), float4);
	table->addRow(m_t == T_1 ? Text::T1::Float5 : (m_t == T_2 ? Text::T2::Float5 : Text::KvBlockVoltage), float5);
	table->addRow(m_t == T_1 ? Text::T1::Float6 : (m_t == T_2 ? Algs::Text::BlockVoltage : Text::CHAPVTime), float6);

	if (m_t == T_1 || m_t == T_2) {
		table->addRow(m_t == T_1 ? Text::CHAPVTime : Text::KvBlockVoltage, float7);
		if (m_t == T_2)
			table->addRow(Text::CHAPVTime, float8);
	}
}
