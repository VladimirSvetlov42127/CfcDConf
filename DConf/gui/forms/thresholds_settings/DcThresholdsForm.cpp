#include "DcThresholdsForm.h"

#include <qpushbutton.h>

#include <gui/editors/EditorsManager.h>

namespace {	
} // namespace

namespace Text {
	const QString Voltage = "Напряжения (В)";
	const QString AmperagePhase = "Токи фазные (А)";
	const QString Amperage3Io = "Ток 3Io (А)";
}


DcThresholdsForm::DcThresholdsForm(DcController *controller) :
	DcForm(controller,  "Пороги")
{
	QGridLayout* layout = new QGridLayout(centralWidget());
	EditorsManager mg(controller, layout);

	auto createEditorsForParam = [controller, &mg](uint32_t addr) {
        auto param = controller->paramsRegistry().parameter(addr);
		if (!param)
			return;

        for (size_t i = 0; i < param->elementsCount(); i++) {
            auto p = controller->paramsRegistry().element(addr, i);
			mg.addLineEditor(p, p->name());
		}
	};

	createEditorsForParam(SP_DIN_TO_LOW);
	createEditorsForParam(SP_DIN_TO_HIGH);
	createEditorsForParam(SP_PSCI_FLOAT_PARAM);

	mg.addStretch();
}

bool DcThresholdsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DIN_TO_HIGH},
		{SP_PSCI_FLOAT_PARAM}
	};

	return hasAny(controller, params);
}

void DcThresholdsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	auto device = report->device();
	DcReportTable table(device);


	auto createRecordsForParam = [device, &table ](uint32_t addr) {
        auto param = device->paramsRegistry().parameter(addr);
		if (!param)
			return;

        for (size_t i = 0; i < param->elementsCount(); i++) {
            auto p = device->paramsRegistry().element(addr, i);
			table.addRow(DcReportTable::ParamRecord(p, p->name()));
		}
	};

	createRecordsForParam(SP_DIN_TO_LOW);
	createRecordsForParam(SP_DIN_TO_HIGH);
	createRecordsForParam(SP_PSCI_FLOAT_PARAM);
	report->insertTable(table);
}
