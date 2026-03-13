#include "DcOscillographyForm.h"

#include <QTabWidget>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>


#include <gui/editors/EditorsManager.h>
#include "gui/forms/functions/oscillography/AnalogsModel.h"

using namespace Dpc::Gui;

namespace {
    const ListEditorContainer g_FullMemoryMode = { "Перезапись", "Остановка" };

    ComboBoxDelegate* g_signalsDelegate(DcController *controller, DefSignalType type, DefSignalSubType subType = DEF_SIG_SUBTYPE_UNDEF, QObject *parent = nullptr)
    {
        ComboBoxDelegate* result = new ComboBoxDelegate(parent);
        result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
        for (auto signal : controller->getSignalList(type, subType)) {
            result->append({QString("(%1) %2").arg(static_cast<uint>(signal->internalId())).arg(signal->name())
                            , static_cast<uint>(signal->internalId()) });
        }
    return result;
    }
} //namespace

namespace TabNames
{
    namespace Common
    {
        const QString Title = "Общие";
    }

    namespace Analogs
    {
        const QString Title = "Дополнительные аналоги";
    }
} //TabNames

namespace Text {
	const QString PKE_Oscl0 = "От внешнего источника (команда)";
	const QString PKE_Oscl1 = "От входного дискрета";

	const QString PKE_OsclPol0_PSC = "Интервал времени в осциллограмме до события (Сек)";
	const QString PKE_OsclPol0 = "Интервал разрешения(мc)";

	const QString PKE_OsclPol1 = "Интервал нечувствительности(мc)";
	const QString PKE_OsclPol2 = "Число однотипных событий";
	const QString PKE_OsclPol3 = "Общее число событий";
	const QString PKE_OsclPol4 = "Период до события(мс)";
	const QString PKE_OsclPol5 = "Период после события(мс)";
	const QString MMSRCDMOD0 = "При переполнении памяти";

	const QString SP_TIMEZONE_HOUR1 = "Смещение относительно часового пояса Москвы";
    const QString PKE_OsclPol6 = "Восстановить при форматировании (кол-во)";
}

DcOscillographyForm::DcOscillographyForm(DcController *controller) :
    DcForm(controller,  "Осциллографирование", false)
{
    QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

    QTabWidget *tabWidget = new QTabWidget;
    formLayout->addWidget(tabWidget);

    tabWidget->addTab(createCommonTab(), TabNames::Common::Title);
    tabWidget->addTab(createAnalogTab(), TabNames::Analogs::Title);
}

bool DcOscillographyForm::isAvailableFor(DcController *controller)
{
	static QList<Param> params = {
		{SP_PKE_Oscl},
		{SP_PKE_OsclPol},
        {SP_MMSRCDMOD},
        {SP_AINS_TO_OSC}
	};

	return hasAny(controller, params);
}

void DcOscillographyForm::fillReport(DcIConfigReport *report)
{
	report->insertSection();

	DcReportTable table(report->device());
	table.addRow(DcReportTable::ParamRecord(SP_PKE_Oscl, 0, Text::PKE_Oscl0, true));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_Oscl, 1, Text::PKE_Oscl1, true));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 0, DcController::P_SC == report->device()->type() ? Text::PKE_OsclPol0_PSC : Text::PKE_OsclPol0));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 1, Text::PKE_OsclPol1));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 2, Text::PKE_OsclPol2));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 3, Text::PKE_OsclPol3));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 4, Text::PKE_OsclPol4));
	table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 5, Text::PKE_OsclPol5));
	table.addRow(DcReportTable::ParamRecord(SP_MMSRCDMOD, 0, g_FullMemoryMode.toHash(), Text::MMSRCDMOD0));
    table.addRow(DcReportTable::ParamRecord(SP_PKE_OsclPol, 6, Text::PKE_OsclPol6));

	report->insertTable(table);
}

QWidget *DcOscillographyForm::createCommonTab()
{
    static QList<Param> params = {
        {SP_PKE_Oscl},
        {SP_PKE_OsclPol},
        {SP_MMSRCDMOD},
    };

    if(!hasAny(controller(), params))
        return nullptr;

    QWidget* tab = new QWidget;
    QGridLayout *tabLayout = new QGridLayout(tab);
    EditorsManager mg(controller(), tabLayout);

    mg.addCheckEditor(SP_PKE_Oscl, 0, Text::PKE_Oscl0);
    mg.addCheckEditor(SP_PKE_Oscl, 1, Text::PKE_Oscl1);

    if (DcController::P_SC == controller()->type())
        mg.addLineEditor(SP_PKE_OsclPol, 0, Text::PKE_OsclPol0_PSC, 0.1, 5);
    else
        mg.addLineEditor(SP_PKE_OsclPol, 0, Text::PKE_OsclPol0);

    mg.addLineEditor(SP_PKE_OsclPol, 1, Text::PKE_OsclPol1);
    mg.addLineEditor(SP_PKE_OsclPol, 2, Text::PKE_OsclPol2);
    mg.addLineEditor(SP_PKE_OsclPol, 3, Text::PKE_OsclPol3);

    auto oscLimitsParam2 = controller()->paramsRegistry().element(SP_OSC_LIMITS, 2);
    if(oscLimitsParam2)
        mg.addLineEditor(SP_PKE_OsclPol, 4, Text::PKE_OsclPol4, 0, oscLimitsParam2->value().toUInt());
    else
        mg.addLineEditor(SP_PKE_OsclPol, 4, Text::PKE_OsclPol4);

    mg.addLineEditor(SP_PKE_OsclPol, 5, Text::PKE_OsclPol5);
    mg.addListEditor(SP_MMSRCDMOD, 0, Text::MMSRCDMOD0, g_FullMemoryMode);
    mg.addLineEditor(SP_TIMEZONE_HOUR, 1, Text::SP_TIMEZONE_HOUR1, -12, 14);

    auto oscLimitsParam3 = controller()->paramsRegistry().element(SP_OSC_LIMITS, 3);
    if(oscLimitsParam3)
        mg.addLineEditor(SP_PKE_OsclPol, 6, Text::PKE_OsclPol6, 0, oscLimitsParam3->value().toUInt());
    else
        mg.addLineEditor(SP_PKE_OsclPol, 6, Text::PKE_OsclPol6);

    mg.addStretch();

    return tab;
}

QWidget *DcOscillographyForm::createAnalogTab()
{
    auto param = controller()->paramsRegistry().element(SP_AINS_TO_OSC, 0);

    if(!param)
        return nullptr;

    Dpc::Gui::ComboBoxDelegate* analogsDelegate = g_signalsDelegate(controller(), DEF_SIG_TYPE_ANALOG, DEF_SIG_SUBTYPE_UNDEF, this);

    auto view = new TableView(new AnalogsModel(controller(), this), this);

    view->setColumnWidth(AnalogsModel::Columns::Trand, 80);
    view->setColumnWidth(AnalogsModel::Columns::Analog, 350);

    view->setItemDelegateForColumn(AnalogsModel::Columns::Analog, analogsDelegate);

    QWidget *tab = new QWidget;
    QGridLayout *tabLayout = new QGridLayout(tab);
    EditorsManager manager(controller(), tabLayout);
    manager.addWidget(view);
    return tab;
}
