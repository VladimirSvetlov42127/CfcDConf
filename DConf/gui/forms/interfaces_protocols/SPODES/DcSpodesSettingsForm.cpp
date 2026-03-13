#include "DcSpodesSettingsForm.h"

#include <gui/editors/EditorsManager.h>

namespace {
	const ListEditorContainer g_AddressLengthList = { {"1 байт", 1}, {"2 байта", 2}, {"4 байта", 4} };
}

namespace Text {
	const QString AddressLength = "Длина адреса";
	const QString LogicalAddress = "Логический адрес";
	const QString Password = "Пароль";

}

DcSpodesSettingsForm::DcSpodesSettingsForm(DcController *controller) :
	DcForm(controller,  "Параметры СПОДЭС")
{
	QGridLayout *layout = new QGridLayout(centralWidget());

	EditorsManager manager(controller, layout);
	manager.addListEditor(SP_SPODES_PARAMS, 0, Text::AddressLength, g_AddressLengthList);
	manager.addLineEditor(SP_SPODES_PARAMS, 1, Text::LogicalAddress);
	manager.addLineEditor(SP_SPODES_PASSW, 0, Text::Password);

	layout->setRowStretch(layout->rowCount(), 1);
	layout->setColumnStretch(layout->columnCount(), 1);
}

bool DcSpodesSettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_SPODES_PARAMS},
		{SP_SPODES_PASSW}
	};

	return hasAny(controller, params);
}

void DcSpodesSettingsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	auto device = report->device();
	DcReportTable table(device);
	table.addRow(DcReportTable::ParamRecord(SP_SPODES_PARAMS, 0, g_AddressLengthList.toHash(), Text::AddressLength));
	table.addRow(DcReportTable::ParamRecord(SP_SPODES_PARAMS, 1, Text::LogicalAddress));
	table.addRow(DcReportTable::ParamRecord(SP_SPODES_PASSW, 0, Text::Password));

	report->insertTable(table);
}
