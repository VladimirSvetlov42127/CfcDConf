#include "system_stat_tab.h"

#include <QGridLayout>
#include <QTabWidget>

#include <dpc/gui/widgets/TableView.h>

#include "device_operations/information_operation.h"
#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const char *COLUMN_VALUE = "Значение";

} // namespace

SystemStatTab::SystemStatTab(QWidget *parent)
    : AbstractDeviceTab(parent)
    , m_tabWidget(new QTabWidget)
{
    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(m_tabWidget);
}

QWidget *SystemStatTab::createTab(const InformationOperation::ParamsContainer &container, uint16_t profile)
{
    auto tab = new QWidget;

    BaseTabModel *systemStatModel = new BaseTabModel(EX_SEARCH_BOOT, profile, COLUMN_VALUE, tab);
    systemStatModel->updateInfo(container);

    auto systemStatView = new TableView(systemStatModel, tab);
    systemStatView->horizontalHeader()->setResizeContentsPrecision(QHeaderView::ResizeToContents);

    QGridLayout *tabLayout = new QGridLayout(tab);
    tabLayout->addWidget(systemStatView);
    return tab;
}

void SystemStatTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        const auto &container = info->params();
        const auto &it = container.find(EX_SEARCH_BOOT);
        if (it == container.end())
            return;

        while (m_tabWidget->count() > 0) {
            QWidget* widget = m_tabWidget->widget(0);
            m_tabWidget->removeTab(0);
            delete widget;
        }

        auto systemStatProfileSize = it->second.size();

        for(size_t i = 0; i < systemStatProfileSize; ++i)
            m_tabWidget->addTab(createTab(container, i), QString("Профиль %1").arg(i + 1));   
    }
}

QList<uint16_t> SystemStatTab::addrList() const
{
    const QList<uint16_t> requiredParams {EX_SEARCH_BOOT};
    return requiredParams;
}
