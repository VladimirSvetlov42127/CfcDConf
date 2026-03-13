#include "DcAnalogsForm.h"

#include <qtabwidget.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>
#include <dpc/gui/delegates/PushButtonDelegate.h>

#include <gui/editors/EditorsManager.h>
#include <gui/forms/functions/analogs/conversion/ConversionModel.h>
#include <gui/forms/functions/analogs/conversion/CustomConversionDialog.h>
#include <gui/forms/functions/analogs/comparison/ComparisonModel.h>


using namespace Dpc::Gui;

namespace {
	ComboBoxDelegate g_ConversionTypeDelegate = { "Отсутсвует", "ТСМ50", "Pt1000", "Пользовательский" };
	ComboBoxDelegate g_CompareTypeDelegate = { "Больше", "Меньше" };

    ComboBoxDelegate* g_signalsDelegate(DcController* controller, DefSignalType type, DefSignalSubType subType = DEF_SIG_SUBTYPE_UNDEF, QObject *parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
        for (auto signal : controller->getSignalList(type, subType)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}	
}

namespace Text
{
	namespace Conversion {
		const QString Title = "Преобразование";
	}

	namespace Comparison {
		const QString Title = "Сравнение";
	}
}

DcAnalogsForm::DcAnalogsForm(DcController* controller) :
	DcForm(controller,  "Аналоги", false)
{
	m_analogsDelegate = g_signalsDelegate(controller, DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_UNDEF, this);
	m_virtualAnalogsDelegate = g_signalsDelegate(controller, DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_VIRTUAL, this);
	m_virtualDiscretsDelegate = g_signalsDelegate(controller, DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_VIRTUAL, this);

	QVBoxLayout* formLayout = new QVBoxLayout(centralWidget());

	QTabWidget* tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createConversionTab(), Text::Conversion::Title);
	tabWidget->addTab(createComparisonTab(), Text::Comparison::Title);
}

bool DcAnalogsForm::isAvailableFor(DcController* controller)
{
	static QList<Param> params = {
		{SP_AIN_CONVERS_WORDAIN_PARAM},
		{SP_AINCMP_WORDAIN_PARAM}
	};

	return hasAny(controller, params);
}

void DcAnalogsForm::fillReport(DcIConfigReport* report)
{
	auto device = report->device();

	auto analogsDelegate = std::unique_ptr<ComboBoxDelegate>(g_signalsDelegate(device, DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_UNDEF));
	auto virtualAnalogsDelegate = std::unique_ptr<ComboBoxDelegate>(g_signalsDelegate(device, DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_VIRTUAL));
	auto virtualDiscretsDelegate = std::unique_ptr<ComboBoxDelegate>(g_signalsDelegate(device, DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_VIRTUAL));

    auto param = device->paramsRegistry().parameter(SP_AIN_CONVERS_WORDAIN_PARAM);
	if (param) {
		auto model = std::make_unique<ConversionModel>(device);
		QStringList headers;
		
		auto columnsCount = ConversionModel::ColumnsCount - 1; // Не считаем колонку с кнопкой для настройки пользовательского преобразования
        for (int i = 0; i < columnsCount; i++)
			headers << model->columnName(ConversionModel::Columns(i));

		report->insertSection(Text::Conversion::Title);
		DcReportTable table(device, headers, { 5 });

		for (auto&& it : model->getItems()) {
			QStringList values;
            for (int col = 0; col < it.size(); col++) {
				auto value = it[col];
				if (col == ConversionModel::SettingColumn)
					continue;
				else if (col == ConversionModel::SourceColumn)
					values << analogsDelegate->textFor(value.toUInt()).toString();
				else if (col == ConversionModel::DestinationColumn)
					values << virtualAnalogsDelegate->textFor(value.toUInt()).toString();
				else if (col == ConversionModel::ConversionTypeColumn)
					values << g_ConversionTypeDelegate.textFor(value.toUInt()).toString();
				else
					values << value.toString();
			}

			table.addRow(values);
		}
		report->insertTable(table);
	}

    param = device->paramsRegistry().parameter(SP_AINCMP_WORDAIN_PARAM);
	if (param) {
		auto model = std::make_unique<ComparisonModel>(device);
		QStringList headers;
		for (size_t i = 0; i < ComparisonModel::ColumnsCount; i++)
			headers << model->columnName(ComparisonModel::Columns(i));

		report->insertSection(Text::Comparison::Title);
		DcReportTable table(device, headers, { 5 });

		for (auto&& it : model->getItems()) {
			QStringList values;
            for (int col = 0; col < it.size(); col++) {
				auto value = it[col];
				if (col == ComparisonModel::SourceColumn)
					values << analogsDelegate->textFor(value.toUInt()).toString();
				else if (col == ComparisonModel::DestinationColumn)
					values << virtualDiscretsDelegate->textFor(value.toUInt()).toString();
				else if (col == ComparisonModel::CompareTypeColumn)
					values << g_CompareTypeDelegate.textFor(value.toUInt()).toString();
				else
					values << value.toString();
			}

			table.addRow(values);
		}
		report->insertTable(table);
	}	
}

QWidget* DcAnalogsForm::createConversionTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_AIN_CONVERS_WORDAIN_PARAM);
	if (!param)
		return nullptr;

	auto view = new TableView(new ConversionModel(controller(), this), this);
	view->setColumnWidth(ConversionModel::NumColumn, 20);
	view->setColumnWidth(ConversionModel::SourceColumn, 350);
	view->setColumnWidth(ConversionModel::DestinationColumn, 250);
	view->setColumnWidth(ConversionModel::ConversionTypeColumn, 200);

	view->setItemDelegateForColumn(ConversionModel::SourceColumn, m_analogsDelegate);
	view->setItemDelegateForColumn(ConversionModel::DestinationColumn, m_virtualAnalogsDelegate);
	view->setItemDelegateForColumn(ConversionModel::ConversionTypeColumn, &g_ConversionTypeDelegate);

	auto buttonDelegate = new PushButtonDelegate("...", view);
	connect(buttonDelegate, &PushButtonDelegate::clicked, [=](const QModelIndex& index) {
		CustomConversionDialog dlg(controller(), index.row(), this);
		dlg.exec();
	});
	view->setItemDelegateForColumn(ConversionModel::SettingColumn, buttonDelegate);

	QWidget* tab = new QWidget;
	QGridLayout* tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);
	manager.addWidget(view);
	return tab;
}

QWidget* DcAnalogsForm::createComparisonTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_AINCMP_WORDAIN_PARAM);
	if (!param)
		return nullptr;

    AlgService* analogService = controller()->serviceManager()->algManager().alg(SP_AINCMP_WORDIN_PARAM);
    if (!analogService)
        return nullptr;

    //	Создание модели и делегата
    Dpc::Gui::ComboBoxDelegate* delegate = new Dpc::Gui::ComboBoxDelegate(this);
    delegate->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
    for (auto vdin: controller()->serviceManager()->vdins()) {
        if (vdin->source() && vdin->source()->service() != analogService)
            continue;

        delegate->append( {vdin->baseText(), vdin->internalID()} );
    }

    ComparisonModel* model = new ComparisonModel(controller(), this);

    //	Лямбда функция для обновления делегата
    auto delegateUpdate = [=]() {
        QList<uint32_t> list;
        for (int i = 0; i < model->rowCount(); i++) {
            uint32_t value = model->data(model->index(i, ComparisonModel::DestinationColumn)).toInt();
            if (value != 0xFFFF)
                list.append(value);

            auto ser_output = analogService->outputs().at(i).get();
            if (ser_output)
                ser_output->setTarget(controller()->serviceManager()->vdin(value));
        }

        delegate->setExcluding(list);
    };

    delegateUpdate();
	connect(model, &QAbstractItemModel::dataChanged, this, delegateUpdate);
	TableView* view = new TableView(model, this);

	view->setColumnWidth(ComparisonModel::NumColumn, 20);
	view->setColumnWidth(ComparisonModel::SourceColumn, 350);
	view->setColumnWidth(ComparisonModel::DestinationColumn, 250);
	view->setColumnWidth(ComparisonModel::CompareTypeColumn, 150);
	view->setColumnWidth(ComparisonModel::ThresholdColumn, 150);
	view->setColumnWidth(ComparisonModel::HysteresisColumn, 150);
	view->setColumnWidth(ComparisonModel::ResponseTimeColumn, 150);

	view->setItemDelegateForColumn(ComparisonModel::SourceColumn, m_analogsDelegate);
	view->setItemDelegateForColumn(ComparisonModel::DestinationColumn, delegate);
	view->setItemDelegateForColumn(ComparisonModel::CompareTypeColumn, &g_CompareTypeDelegate);
	view->setItemDelegateForColumn(ComparisonModel::ThresholdColumn, new DoubleSpinBoxDelegate(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), view));
	auto positiveDoubleDelegate = new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), view);
	view->setItemDelegateForColumn(ComparisonModel::HysteresisColumn, positiveDoubleDelegate);
	view->setItemDelegateForColumn(ComparisonModel::ResponseTimeColumn, positiveDoubleDelegate);

	QWidget* tab = new QWidget;
	QGridLayout* tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);
	manager.addWidget(view);
	return tab;
}
