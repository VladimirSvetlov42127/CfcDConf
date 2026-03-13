#include "BKIAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_BCI_WORDIN_PARAM);
}

namespace Text {
	const QString Byte0 = "Ввести БКИ в работу";
	const QString Byte1 = "Блокировать включение по БКИ";
	const QString Byte2 = "Блокировать по БКИ до квитирования";
	const QString Byte3 = "БКИ цепей питания 127В";
	const QString Byte4 = "Блокировать включение по БКИ 127В";
	const QString Byte5 = "Блокировать по БКИ 127 до квитирования";

	const QString Float0 = "БКИ: Порог сопротивления изоляции";
	const QString Float1 = "БКИ: К возврата по сопротивлению изоляции";
	const QString Float2 = "БКИ: Время срабатывания (с)";
	const QString Float3 = "БКИ: Задержка диагностики после отключения (с)";
	const QString Float4 = "БКИ: Сопротивление делителя измерения";
	const QString Float5 = "БКИ: Сопротивление верхнего плеча";
	const QString Float6 = "БКИ: Сопротивление нижнего плеча";
	const QString Float7 = "БКИ_127: Сопротивление верхнего плеча";
	const QString Float8 = "БКИ_127: Сопротивление нижнего плеча";
	const QString Float9 = "БКИ_127: Сопротивление оптрона";
	const QString Float10 = "БКИ_127: Тестовое напряжение";
	const QString Float11 = "БКИ_127: Сопротивление утечки по цепям 127В";
	const QString Float12 = "БКИ_127: К возврата по сопротивлению утечки 127В";
}

BKIAlgorithm::BKIAlgorithm() :
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes, 0, false)
{
}

QWidget* BKIAlgorithm::createWidget() const
{
	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		em.addCheckEditor(bytesParam, alg_index(i, 0), Text::Byte0);
		em.addCheckEditor(bytesParam, alg_index(i, 1), Text::Byte1);
		em.addCheckEditor(bytesParam, alg_index(i, 2), Text::Byte2);
		em.addCheckEditor(bytesParam, alg_index(i, 3), Text::Byte3);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);
		em.addCheckEditor(bytesParam, alg_index(i, 5), Text::Byte5);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0);
		em.addLineEditor(floatsParam, alg_index(i, 6), Text::Float6, 0);
		em.addLineEditor(floatsParam, alg_index(i, 7), Text::Float7, 0);
		em.addLineEditor(floatsParam, alg_index(i, 8), Text::Float8, 0);
		em.addLineEditor(floatsParam, alg_index(i, 9), Text::Float9, 0);
		em.addLineEditor(floatsParam, alg_index(i, 10), Text::Float10, 0);
		em.addLineEditor(floatsParam, alg_index(i, 11), Text::Float11, 0);
		em.addLineEditor(floatsParam, alg_index(i, 12), Text::Float12, 0);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void BKIAlgorithm::fillReport(DcReportTable * table) const
{
	QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;
	QList<DcReportTable::ParamRecord> byte5;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;
	QList<DcReportTable::ParamRecord> float6;
	QList<DcReportTable::ParamRecord> float7;
	QList<DcReportTable::ParamRecord> float8;
	QList<DcReportTable::ParamRecord> float9;
	QList<DcReportTable::ParamRecord> float10;
	QList<DcReportTable::ParamRecord> float11;
	QList<DcReportTable::ParamRecord> float12;

	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;

		byte0 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 0), "", true);
		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1), "", true);
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2), "", true);
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3), "", true);
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);
		byte5 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 5), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));
		float6 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 6));
		float7 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 7));
		float8 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 8));
		float9 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 9));
		float10 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 10));
		float11 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 11));
		float12 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 12));
	}

	table->addRow(Text::Byte0, byte0);
	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);
	table->addRow(Text::Byte5, byte5);

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);
	table->addRow(Text::Float6, float6);
	table->addRow(Text::Float7, float7);
	table->addRow(Text::Float8, float8);
	table->addRow(Text::Float9, float9);
	table->addRow(Text::Float10, float10);
	table->addRow(Text::Float11, float11);
	table->addRow(Text::Float12, float12);
}
