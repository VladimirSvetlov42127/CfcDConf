#include <gui/forms/input_output/inputs/dins_external_form.h>

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/widgets/TableView.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================


//===================================================================================================================================================
//	Описание класса
//===================================================================================================================================================
DinsExternalForm::DinsExternalForm(DcController* controller) : DcForm(controller,  "Настройки внешних дискретных входов", false)
{
	DinsModel* model = new DinsModel(controller, nullptr, this);
	model->SetItems(GetItems(controller));
	Dpc::Gui::TableView* tableView = new Dpc::Gui::TableView(model, this);

	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);

	tableView->horizontalHeader()->setStretchLastSection(false);
	tableView->horizontalHeader()->setHighlightSections(false);
	tableView->horizontalHeader()->setFixedHeight(45);
	tableView->horizontalHeader()->setHighlightSections(false);

	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::NumberColumn, 25);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::NameColumn, 250);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::OscillColumn, 150);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::JournalColumn, 80);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::ArhciveColumn, 70);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::DPColumn, 50);

	tableView->horizontalHeader()->hideSection(DinsModel::Columns::DrebezgColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::DrebezgRatio);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::TimerColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::InterferenceColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::InversionColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::SaveColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::Fix1Column);

    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DinsExternalForm::isAvailableFor(DcController* controller)
{
	static QList<Param> params = {
		{SP_DIN_DINOSCMASK},
		{SP_TREND_INITIALISE},
		{SP_DIN_CASH_REQMASK},
		{SP_DIN_INT_CNT},
		{SP_DIN_DEBPARS_DOUBLE},
		{SP_DIN_INT_WDT},
		{SP_DIN_NOISE_WDT},
		{SP_DIN_INVERS},
		{SP_DIN_VDINSAVED},
		{SP_DIN_VDINFIXED},
		{SP_DIN_DPSREPRESENT}
	};

	return hasAny(controller, params);
}

void DinsExternalForm::fillReport(DcIConfigReport* report)
{
}

QList<DinsItem> DinsExternalForm::GetItems(DcController* controller)
{
	QList<DinsItem> items;
	for (auto signal : controller->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_REMOTE)) {
		DinsItem item = { signal, QVariantList(), -1 };
		item.data.append(signal->internalId());
		item.data.append(signal->name());
		item.data.append(controller->getBitValue(SP_DIN_DINOSCMASK, signal->internalId()));
		item.data.append(controller->getBitValue(SP_TREND_INITIALISE, signal->internalId()));
		item.data.append(controller->getBitValue(SP_DIN_CASH_REQMASK, signal->internalId()));
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(controller->getBitValue(SP_DIN_DPSREPRESENT, signal->internalId()));
		items.append(item); }

	return items;
}
