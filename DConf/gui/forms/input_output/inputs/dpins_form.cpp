#include "dpins_form.h"

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include "gui/models/dpin_model.h"

using namespace Dpc::Gui;

namespace {
    const uint32_t VALUE_NOTUSE = 0xFFFF;

    ComboBoxDelegate g_StateDelegate = {
        {"2 Состояния", DPinSignal::State2},
        {"3 Состояния", DPinSignal::State3},
        {"4 Состояния", DPinSignal::State4}
    };

	ComboBoxDelegate* g_ChannelsDelegate(DcController *contr, QObject *parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , VALUE_NOTUSE });
        for (auto signal: contr->signalManager().getSignals<DinSignal>(Signal::Subtype::Physical)) {
            result->append({ signal->text(), signal->internalID() });
		}

		return result;
    }

} // namespace

DPinSignalsForm::DPinSignalsForm(DcController *controller)
    : DcForm(controller,  "Двухпозиционные сигналы", false)
{
    auto dpins = controller->signalManager().getDPinSignals();
    auto model = new DPinModel(dpins, this);
    auto view = new TableView(model, this);
	view->horizontalHeader()->setMinimumHeight(60);
	view->horizontalHeader()->setStretchLastSection(true);
    view->setColumnWidth(DPinModel::Number, 25);
    view->setColumnWidth(DPinModel::RPO, 200);
    view->setColumnWidth(DPinModel::RPV, 200);
    view->setColumnWidth(DPinModel::IntermediateTime, 160);
    view->setColumnWidth(DPinModel::NotGenUndef, 220);
    view->setColumnWidth(DPinModel::IntermediateTimeRatio, 220);
    view->setColumnWidth(DPinModel::Active, 100);
    view->setColumnWidth(DPinModel::State, 100);
    view->setColumnWidth(DPinModel::Inversion, 150);

	auto rpoDelegate = g_ChannelsDelegate(controller, this);
	auto rpvDelegate = g_ChannelsDelegate(controller, this);
    view->setItemDelegateForColumn(DPinModel::RPO, rpoDelegate);
    view->setItemDelegateForColumn(DPinModel::RPV, rpvDelegate);
    view->setItemDelegateForColumn(DPinModel::IntermediateTime, new IntSpinBoxDelegate(0, UCHAR_MAX, this));
    view->setItemDelegateForColumn(DPinModel::IntermediateTimeRatio, new IntSpinBoxDelegate(1, UCHAR_MAX, this));
    view->setItemDelegateForColumn(DPinModel::State, &g_StateDelegate);

    connect(view, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(view, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(view);
}

bool DPinSignalsForm::isAvailableFor(DcController * controller)
{
    return controller->signalManager().dpinSize();
}

void DPinSignalsForm::fillReport(DcIConfigReport * report)
{
//	report->insertSection();

//	QStringList headers;
//	for (size_t i = 0; i < ColumnsCount; i++)
//		headers << columnName(Columns(i));

//	auto channelsDelegate = std::shared_ptr<ComboBoxDelegate>(g_ChannelsDelegate(report->device()));

//	DcReportTable table(report->device(), headers, {30});
//	for (auto &&it : getItems(report->device())) {
//		QStringList values;
//		for (size_t col = 0; col < it.size(); col++) {
//			if (Columns::ActiveColumn == col || Columns::InversionColumn == col || Columns::NotGenerateUntrue == col)
//				values << DcReportTable::checkedSign(it[col].toBool());
//			else {
//				if (col == Columns::NameColumn || col == Columns::IntermediateTimeColumn || col == Columns::IntermediateTimeRatioColumn)
//					values << it[col].toString();

//				if (Columns::RPOColumn == col || Columns::RPVColumn == col)
//					values << channelsDelegate->textFor(it[col].toUInt()).toString();

//				if (Columns::StateColumn == col)
//					values << g_StateDelegate.textFor(it[col].toUInt()).toString();
//			}
//		}

//		table.addRow(values);
//	}

//	report->insertTable(table);
}
