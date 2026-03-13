#include "main_form.h"

#include <QTabWidget>

#include "gui/forms/main/common_data_tab.h"
#include "gui/forms/main/date_time_tab.h"
#include "gui/forms/main/flx_logic_tab.h"
#include "gui/forms/main/layers_state_tab.h"
#include "gui/forms/main/signals_tab.h"
#include "gui/forms/main/sm_flags_tab.h"
#include "gui/forms/main/system_stat_tab.h"
#include "gui/forms/main/critical_error_tab.h"
#include "gui/forms/main/fatal_error_tab.h"
#include "gui/forms/main/dynamic_data_tab.h"

namespace {

constexpr const char *TEXT_COMMON_TAB = "Общие";
constexpr const char *TEXT_DATETIME_TAB = "Время";
constexpr const char *TEXT_SIGNALS_TAB = "Состав сигналов";
constexpr const char *TEXT_FLAGS_SM_TAB = "Флаги состояния SM";
constexpr const char *TEXT_LAYERS_STATE_TAB = "Состояние слоев ПО";
constexpr const char *TEXT_SYSTEM_STAT_TAB = "Системная статистика";
constexpr const char *TEXT_FLX_TAB = "Статистика задач гибкой логики";
constexpr const char *TEXT_DUMP_FATAL_ERROR_TAB = "Дамп фатальных ошибок";
constexpr const char *TEXT_CRITICAL_ERROR_MESSAGE_TAB = "Сообщение о критических ошибках";
constexpr const char *TEXT_STORED_DYNAMIC_DATA_TAB = "Сохраняемые динамические данные";

} //namespace

MainForm::MainForm(DcController *device)
    : DcForm(device, "Управление устройством", false)
    , m_tabWidget(new QTabWidget)
{
    auto formLayout = new QVBoxLayout(centralWidget());
    formLayout->addWidget(m_tabWidget);

    auto dateTimeTab = new DateTimeTab(device);
    connect(dateTimeTab, &AbstractDeviceTab::operationRequest, this, &MainForm::operationRequest);

    m_tabWidget->addTab(new CommonDataTab(device), TEXT_COMMON_TAB);
    m_tabWidget->addTab(dateTimeTab, TEXT_DATETIME_TAB);
    m_tabWidget->addTab(new SignalsTab, TEXT_SIGNALS_TAB);
    m_tabWidget->addTab(new SmFlagsTab, TEXT_FLAGS_SM_TAB);
    m_tabWidget->addTab(new LayersStateTab, TEXT_LAYERS_STATE_TAB);
    m_tabWidget->addTab(new SystemStatTab, TEXT_SYSTEM_STAT_TAB);
    m_tabWidget->addTab(new FlxLogicTab, TEXT_FLX_TAB);
    m_tabWidget->addTab(new FatalErrorTab, TEXT_DUMP_FATAL_ERROR_TAB);
    m_tabWidget->addTab(new CriticalErrorTab, TEXT_CRITICAL_ERROR_MESSAGE_TAB);
    m_tabWidget->addTab(new DynamicDataTab, TEXT_STORED_DYNAMIC_DATA_TAB);

    setTabsActive(false);
}

QList<uint16_t> MainForm::paramAddrList() const
{
    QList<uint16_t> list;
    for(int i = 0; i < m_tabWidget->count(); ++i)
        list.append( static_cast<AbstractDeviceTab*>(m_tabWidget->widget(i))->addrList() );

    return list;
}

void MainForm::setTabsActive(bool active)
{
    for (int i = 1; i < m_tabWidget->count(); i++)
        m_tabWidget->setTabVisible(i, active);

    static_cast<CommonDataTab*>(m_tabWidget->widget(0))->deviceOperationFinished(nullptr);
}

void MainForm::operationFinished(AbstractOperation *operation)
{
    for(int i = 0; i < m_tabWidget->count(); ++i) {
        auto widget = dynamic_cast<AbstractDeviceTab*>(m_tabWidget->widget(i));
        if (!widget)
            continue;

        widget->deviceOperationFinished(operation);
        auto icon = widget->hasError() ? QIcon(":/icons/warning.svg") : QIcon();
        m_tabWidget->setTabIcon(i, icon);
    }
}
