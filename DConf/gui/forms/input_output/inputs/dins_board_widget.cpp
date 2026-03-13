#include <gui/forms/input_output/inputs/dins_board_widget.h>

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
#include <QDebug>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFont>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================
#include <gui/editors/EditorsManager.h>
#include <gui/forms/input_output/inputs/dins_model.h>

//===================================================================================================================================================
//	namespases и списки данных
//===================================================================================================================================================
namespace {
	const QString widget_style = "QGroupBox {border: 2px solid darkgrey; border-radius: 4px; background-color: %1;}";
	const ListEditorContainer amperage_list = { "DC", "AC" };
}


//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DinsBoardWidget::DinsBoardWidget(DcController* controller, DcBoard* board, int start_signal, QWidget* parent) : QWidget(parent)
{
	//	Формирование виджета
	QVBoxLayout* main_layout = new QVBoxLayout;

	//	Заголовок формы
	QLabel* title_label = new QLabel(board->type());
	QFont font = title_label->font();
	font.setBold(true);
	title_label->setFont(font);
	main_layout->addWidget(title_label, 0, Qt::AlignCenter);

    auto param = board->paramsRegistry().element(SP_DIN_ALG, 0);
	if (param) {
		QGridLayout* grid_layout = new QGridLayout;
		EditorsManager* editors = new EditorsManager(controller, grid_layout, this);
		editors->addListEditor(param, "Оперативный ток", amperage_list);
		editors->addStretch();
		main_layout->addLayout(grid_layout); }

	DinsModel* model = new DinsModel(controller, board, this);
	model->SetItems(GetBoardItems(controller, board, start_signal));
	Dpc::Gui::TableView* tableView = new Dpc::Gui::TableView(model, this);

	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	tableView->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked | QAbstractItemView::CurrentChanged);

	tableView->horizontalHeader()->setStretchLastSection(false);
	tableView->horizontalHeader()->setHighlightSections(false);
	tableView->horizontalHeader()->setFixedHeight(45);
	tableView->horizontalHeader()->setHighlightSections(false);
	tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tableView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

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
	main_layout->addWidget(tableView);

	//	Формирование виджета
	QGroupBox* widget_group = new QGroupBox;
	widget_group->setLayout(main_layout);
    QString color = palette().color(QPalette::Window).name();
    widget_group->setStyleSheet(widget_style.arg(color));
	QVBoxLayout* widget_layout = new QVBoxLayout(this);
	widget_layout->setContentsMargins(0, 0, 0, 0);
	widget_layout->addWidget(widget_group);
}

DinsBoardWidget::~DinsBoardWidget()
{
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
QList<DinsItem> DinsBoardWidget::GetBoardItems(DcController* controller, DcBoard* board, int start_signal)
{
	QList<DinsItem> items;

	int count = start_signal;
	auto signals_list = controller->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS);
	for (int i = 0; i < board->DinsCount(); i++) {
		auto signal = signals_list.at(start_signal + i);
		DinsItem item = { signal, QVariantList(), -1 };
		item.subTypeIdx = count++;
		item.data.append(signal->internalId());
		item.data.append(signal->name());
		item.data.append(controller->getBitValue(SP_DIN_DINOSCMASK, signal->internalId()));
		item.data.append(controller->getBitValue(SP_TREND_INITIALISE, signal->internalId()));
		item.data.append(controller->getBitValue(SP_DIN_CASH_REQMASK, signal->internalId()));

        board->ToBoard() ? item.data.append(board->paramsRegistry().element(SP_DIN_INT_CNT, i)->value()) :
			item.data.append(controller->getValue(SP_DIN_INT_CNT, item.subTypeIdx).toInt());			
		
		item.data.append(controller->getValue(SP_DIN_DEBPARS_DOUBLE, item.subTypeIdx).toInt());

        board->ToBoard() ? item.data.append(board->paramsRegistry().element(SP_DIN_INT_WDT, i)->value()) :
			item.data.append(controller->getValue(SP_DIN_INT_WDT, item.subTypeIdx).toInt());
        board->ToBoard() ? item.data.append(board->paramsRegistry().element(SP_DIN_NOISE_WDT, i)->value()) :
			item.data.append(controller->getValue(SP_DIN_NOISE_WDT, item.subTypeIdx).toInt());
		QVariant inversionState;
        board->ToBoard() ? inversionState = board->paramsRegistry().element(SP_DIN_INVERS, i)->value() :
			inversionState = controller->getValue(SP_DIN_INVERS, item.subTypeIdx);
		if (inversionState.toBool()) inversionState = Qt::Checked;
		item.data.append(inversionState);

		item.data.append(QVariant());
		item.data.append(QVariant());
		item.data.append(controller->getBitValue(SP_DIN_DPSREPRESENT, signal->internalId()));
		items.append(item);	}

	return items;
}
