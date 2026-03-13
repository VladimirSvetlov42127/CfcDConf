#include <gui/forms/input_output/inputs/dins_physical_form.h>

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QHash>
#include <QDebug>
#include <QTableWidget>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/forms/input_output/inputs/dins_board_widget.h>


//===================================================================================================================================================
//	Коструктор и деструктор класса
//===================================================================================================================================================
DinsPhysicalForm::DinsPhysicalForm(DcController* controller) : DcForm(controller,  "Настройки физических дискретных входов", false)
{
	//	Свойства класса
    int boards_count = controller->boards().size();
	boards_count > 0 ? MultiBoardForm(controller) : OneBoardForm(controller);

}


//===================================================================================================================================================
//	Открытые методы класса
//===================================================================================================================================================
bool DinsPhysicalForm::isAvailableFor(DcController* controller)
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
		{SP_DIN_DPSREPRESENT} };

	return hasAny(controller, params);
}

void DinsPhysicalForm::fillReport(DcIConfigReport* report)
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QList<DinsItem> DinsPhysicalForm::GetOneBoardItems(DcController* controller)
{
	QList<DinsItem> items;
	int count = 0;
	for (auto signal : controller->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS)) {
		DinsItem item = { signal, QVariantList(), -1 };
		item.subTypeIdx = count++;
		item.data.append(signal->internalId());
		item.data.append(signal->name());
		item.data.append(controller->getBitValue(SP_DIN_DINOSCMASK, signal->internalId()));
		item.data.append(controller->getBitValue(SP_TREND_INITIALISE, signal->internalId()));
		item.data.append(controller->getBitValue(SP_DIN_CASH_REQMASK, signal->internalId()));

		item.data.append(controller->getValue(SP_DIN_INT_CNT, item.subTypeIdx).toInt());
		item.data.append(controller->getValue(SP_DIN_DEBPARS_DOUBLE, item.subTypeIdx).toInt());
		item.data.append(controller->getValue(SP_DIN_INT_WDT, item.subTypeIdx).toInt());
		item.data.append(controller->getValue(SP_DIN_NOISE_WDT, item.subTypeIdx).toInt());
		
		QVariant inversionState = controller->getValue(SP_DIN_INVERS, item.subTypeIdx);
		if (inversionState.toBool()) inversionState = Qt::Checked;
		item.data.append(inversionState);

		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(controller->getBitValue(SP_DIN_DPSREPRESENT, signal->internalId()));
		items.append(item);	}

	return items;
}

void DinsPhysicalForm::OneBoardForm(DcController* controller)
{
	DinsModel* model = new DinsModel(controller, nullptr, this);
	model->SetItems(GetOneBoardItems(controller));
	Dpc::Gui::TableView* tableView = new Dpc::Gui::TableView(model, this);

	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);

	tableView->horizontalHeader()->setStretchLastSection(false);
	tableView->horizontalHeader()->setHighlightSections(false);
	tableView->horizontalHeader()->setFixedHeight(45);
	tableView->horizontalHeader()->setHighlightSections(false);

	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::NumberColumn, 25);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::NameColumn, 200);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::OscillColumn, 150);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::JournalColumn, 80);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::ArhciveColumn, 70);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::DrebezgColumn, 160);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::DrebezgRatio, 160);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::TimerColumn, 160);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::InterferenceColumn, 160);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::InversionColumn, 80);
	tableView->horizontalHeader()->resizeSection(DinsModel::Columns::DPColumn, 50);

	tableView->horizontalHeader()->hideSection(DinsModel::Columns::SaveColumn);
	tableView->horizontalHeader()->hideSection(DinsModel::Columns::Fix1Column);

	tableView->setItemDelegateForColumn(DinsModel::Columns::DrebezgColumn, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
	tableView->setItemDelegateForColumn(DinsModel::Columns::DrebezgRatio, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
	tableView->setItemDelegateForColumn(DinsModel::Columns::TimerColumn, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));
	tableView->setItemDelegateForColumn(DinsModel::Columns::InterferenceColumn, new Dpc::Gui::IntSpinBoxDelegate(0, UCHAR_MAX, this));

    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &Dpc::Gui::TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout* layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);

	return;
}

void DinsPhysicalForm::MultiBoardForm(DcController* controller)
{
	QTableWidget* table_widget = new QTableWidget(this);
	table_widget->setColumnCount(1);
	table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table_widget->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table_widget->horizontalHeader()->hide();
	table_widget->verticalHeader()->hide();

	int widget_count = 0;
	int start_signal = 0;
    for (auto &board: controller->boards()) {
        if (board->DinsCount() < 1)
            continue;

		table_widget->insertRow(widget_count);
        table_widget->setCellWidget(widget_count, 0, new DinsBoardWidget(controller, board.get(), start_signal, this));
        start_signal = start_signal + board->DinsCount();
		widget_count++;	}

	QVBoxLayout* layout = new QVBoxLayout(centralWidget());
	layout->addWidget(table_widget);

	return;
}

