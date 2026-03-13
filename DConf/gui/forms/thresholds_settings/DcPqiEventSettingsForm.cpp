#include "DcPqiEventSettingsForm.h"

#include <qtabwidget.h>
#include <gui/editors/EditorsManager.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <gui/forms/thresholds_settings/pqi_event_model.h>


DcPqiEventSettingsForm::DcPqiEventSettingsForm(DcController *controller) :
	DcForm(controller,  "Уставки событий ПКЭ", false)
{
    //  Создание модели
    PqiEventModel* model  = new PqiEventModel(controller, this);
    Dpc::Gui::TableView* tableView = new Dpc::Gui::TableView(model, this);

    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);

    tableView->horizontalHeader()->setStretchLastSection(false);
    tableView->horizontalHeader()->setHighlightSections(false);
    tableView->horizontalHeader()->setFixedHeight(45);

    tableView->horizontalHeader()->resizeSection(PqiEventModel::NUMBER_COLUMN, 25);
    tableView->horizontalHeader()->resizeSection(PqiEventModel::NAME_COLUMN, 350);
    tableView->horizontalHeader()->resizeSection(PqiEventModel::TIME_COLUMN, 220);
    tableView->horizontalHeader()->resizeSection(PqiEventModel::THRESHOLD_COLUMN, 160);
    tableView->horizontalHeader()->resizeSection(PqiEventModel::GISTERESIS_COLUMN, 180);

    tableView->setItemDelegateForColumn(PqiEventModel::TIME_COLUMN, new Dpc::Gui::UIntSpinBoxDelegate(this));
    tableView->setItemDelegateForColumn(PqiEventModel::THRESHOLD_COLUMN, new Dpc::Gui::DoubleSpinBoxDelegate(0, 1000, this));
    tableView->setItemDelegateForColumn(PqiEventModel::GISTERESIS_COLUMN, new Dpc::Gui::DoubleSpinBoxDelegate(0, 1000, this));

    QVBoxLayout* layout = new QVBoxLayout(centralWidget());
    layout->addWidget(tableView);
}

DcPqiEventSettingsForm::~DcPqiEventSettingsForm()
{
}


bool DcPqiEventSettingsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_FIO_Porog},
	};

	return hasAny(controller, params);
}

void DcPqiEventSettingsForm::fillReport(DcIConfigReport * report)
{
    report->insertSection();
    QStringList headers = {
        Text::ReportTable::Name,
        PqiEventModel::headerNames().at(PqiEventModel::TIME_COLUMN),
        PqiEventModel::headerNames().at(PqiEventModel::THRESHOLD_COLUMN),
        PqiEventModel::headerNames().at(PqiEventModel::GISTERESIS_COLUMN), };

     DcController* device = report->device();
     DcReportTable table(device, headers, { 40 });
     auto param = device->paramsRegistry().parameter(SP_FIO_Porog);
     for (size_t i = 0; i < param->profilesCount(); i++) {
     	auto baseIdx = PROFILE_SIZE * i;
     	QList<DcReportTable::ParamRecord> recs;
     	recs.append(DcReportTable::ParamRecord(SP_FIO_Porog, baseIdx + 0, PqiEventModel::headerNames().at(PqiEventModel::TIME_COLUMN)));
     	recs.append(DcReportTable::ParamRecord(SP_FIO_Porog, baseIdx + 1, PqiEventModel::headerNames().at(PqiEventModel::THRESHOLD_COLUMN)));
     	recs.append(DcReportTable::ParamRecord(SP_FIO_Porog, baseIdx + 2, PqiEventModel::headerNames().at(PqiEventModel::GISTERESIS_COLUMN)));
     	table.addRow(PqiEventModel::paramsList().value(i), recs); }
     report->insertTable(table);
}

