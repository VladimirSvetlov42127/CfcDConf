#include "VNRAlgorithm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	ALG_ADDRS_FUNC(SP_AVR_VNR_WORDIN_PARAM);

	ListEditorContainer analogList(DcController *device) {
		ListEditorContainer res = { {"Не используется", 0xFFFF} };
		for (auto signal: device->getSignalList(DEF_SIG_TYPE_ANALOG)) {
			int value = signal->internalId();
			QString text = QString("(%1) %2").arg(value).arg(signal->name());
			res.append({ text, value });
		}

		return res;
	}
}

namespace Text {
	const QString Byte0 = "Алгоритм ВНР включен";
	const QString Byte1 = "Число попыток вкл. привода СВН";
	const QString Byte2 = "Число попыток вкл. привода ШВН-А";
	const QString Byte3 = "Число попыток вкл. привода ШВН-Б";
	const QString Byte4 = "Наличие напряжения по аналогам";

	const QString Float0 = "Макс. время хода привода СВН, с";
	const QString Float1 = "Макс. время хода привода ШВН-А, с";
	const QString Float2 = "Макс. время хода привода ШВН-Б, с";
	const QString Float3 = "Время задержки до вкл. цепей 'ОЦ RM6'";
	const QString Float4 = "Период опроса аналогов, с";
	const QString Float5 = "Уровень наличия напряжения, В";

	const QString AInputs0 = "AI - индекс Uоц";
	const QString AInputs1 = "AI - индекс Ua1";
	const QString AInputs2 = "AI - индекс Ub1";
	const QString AInputs3 = "AI - индекс Uc1";
	const QString AInputs4 = "AI - индекс Ua2";
	const QString AInputs5 = "AI - индекс Ub2";
	const QString AInputs6 = "AI - индекс Uc2";
}

VNRAlgorithm::VNRAlgorithm() : 
    Board_Algorithm(BoardType::NoBoard | BoardType::AFE, alg_addrs().bytes)
{
}

QWidget* VNRAlgorithm::createWidget() const
{
	auto analogs = analogList(controller());

	auto widget = new QWidget;
	QGridLayout *layout = new QGridLayout(widget);

	for (size_t i = 0; i < groupCount(); i++) {
		auto group = widgetForProfile(i);
		EditorsManager em(controller(), new QGridLayout(group));

		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		uint16_t AinputsParam = alg_addrs().words;

		em.addCheckEditor(bytesParam, alg_index(i, 0), Text::Byte0);
		em.addLineEditor(bytesParam, alg_index(i, 1), Text::Byte1);
		em.addLineEditor(bytesParam, alg_index(i, 2), Text::Byte2);
		em.addLineEditor(bytesParam, alg_index(i, 3), Text::Byte3);
		em.addCheckEditor(bytesParam, alg_index(i, 4), Text::Byte4);

		em.addLineEditor(floatsParam, alg_index(i, 0), Text::Float0, 0, 10000);
		em.addLineEditor(floatsParam, alg_index(i, 1), Text::Float1, 0, 10000);
		em.addLineEditor(floatsParam, alg_index(i, 2), Text::Float2, 0, 10000);
		em.addLineEditor(floatsParam, alg_index(i, 3), Text::Float3, 0, 10000);
		em.addLineEditor(floatsParam, alg_index(i, 4), Text::Float4, 0, 10000);
		em.addLineEditor(floatsParam, alg_index(i, 5), Text::Float5, 0, 10000);

		em.addListEditor(AinputsParam, alg_index(i, 0), Text::AInputs0, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 1), Text::AInputs1, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 2), Text::AInputs2, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 3), Text::AInputs3, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 4), Text::AInputs4, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 5), Text::AInputs5, analogs);
		em.addListEditor(AinputsParam, alg_index(i, 6), Text::AInputs6, analogs);

		layout->addWidget(group, 0, i);
	}

	layout->setColumnStretch(layout->columnCount(), 1);
	layout->setRowStretch(layout->rowCount(), 1);
	return widget;
}

void VNRAlgorithm::fillReport(DcReportTable * table) const
{
	//QList<DcReportTable::ParamRecord> byte0;
	QList<DcReportTable::ParamRecord> byte1;
	QList<DcReportTable::ParamRecord> byte2;
	QList<DcReportTable::ParamRecord> byte3;
	QList<DcReportTable::ParamRecord> byte4;

	QList<DcReportTable::ParamRecord> float0;
	QList<DcReportTable::ParamRecord> float1;
	QList<DcReportTable::ParamRecord> float2;
	QList<DcReportTable::ParamRecord> float3;
	QList<DcReportTable::ParamRecord> float4;
	QList<DcReportTable::ParamRecord> float5;

	QList<DcReportTable::ParamRecord> input0;
	QList<DcReportTable::ParamRecord> input1;
	QList<DcReportTable::ParamRecord> input2;
	QList<DcReportTable::ParamRecord> input3;
	QList<DcReportTable::ParamRecord> input4;
	QList<DcReportTable::ParamRecord> input5;
	QList<DcReportTable::ParamRecord> input6;
	

	auto analogs = analogList(controller()).toHash();
	for (size_t i = 0; i < groupCount(); i++) {
		uint16_t floatsParam = alg_addrs().floats;
		uint16_t bytesParam = alg_addrs().bytes;
		uint16_t inputsParam = alg_addrs().words;

		byte1 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 1));
		byte2 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 2));
		byte3 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 3));
		byte4 << DcReportTable::ParamRecord(bytesParam, alg_index(i, 4), "", true);

		float0 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 0));
		float1 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 1));
		float2 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 2));
		float3 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 3));
		float4 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 4));
		float5 << DcReportTable::ParamRecord(floatsParam, alg_index(i, 5));

		input0 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 0), analogs);
		input1 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 1), analogs);
		input2 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 2), analogs);
		input3 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 3), analogs);
		input4 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 4), analogs);
		input5 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 5), analogs);
		input6 << DcReportTable::ParamRecord(inputsParam, alg_index(i, 6), analogs);		
	}

	table->addRow(Text::Byte1, byte1);
	table->addRow(Text::Byte2, byte2);
	table->addRow(Text::Byte3, byte3);
	table->addRow(Text::Byte4, byte4);

	table->addRow(Text::Float0, float0);
	table->addRow(Text::Float1, float1);
	table->addRow(Text::Float2, float2);
	table->addRow(Text::Float3, float3);
	table->addRow(Text::Float4, float4);
	table->addRow(Text::Float5, float5);

	table->addRow(Text::AInputs0, input0);
	table->addRow(Text::AInputs1, input1);
	table->addRow(Text::AInputs2, input2);
	table->addRow(Text::AInputs3, input3);
	table->addRow(Text::AInputs4, input4);
	table->addRow(Text::AInputs5, input5);
	table->addRow(Text::AInputs6, input6);
}
