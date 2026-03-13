#include "flx_logic_tab.h"

#include <QGridLayout>
#include <QTabWidget>

#include <dpc/gui/widgets/TableView.h>

#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const char *COLUMN_VALUE = "Значение";

} // namespace

FlxLogicTab::FlxLogicTab(QWidget *parent)
    : AbstractDeviceTab(parent)
    , m_tabWidget(new QTabWidget(this))
{
    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(m_tabWidget);
}

QWidget *FlxLogicTab::createTab(const InformationOperation::ParamsContainer &container, uint16_t profile)
{
    auto tab = new QWidget;
    BaseTabModel *flxLogicModel = new BaseTabModel(SP_FLX_RUNSTAT, profile, COLUMN_VALUE, tab);
    flxLogicModel->updateInfo(container);

    auto flxLogicView = new TableView(flxLogicModel, tab);
    flxLogicView->horizontalHeader()->setResizeContentsPrecision(QHeaderView::ResizeToContents);

    QGridLayout *tabLayout = new QGridLayout(tab);
    tabLayout->addWidget(flxLogicView);
    return tab;
}

void FlxLogicTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        const auto &container = info->params();

        const auto &it = container.find(SP_FLX_RUNSTAT);
        if(it == container.end())
            return;

        while (m_tabWidget->count() > 0) {
            QWidget* widget = m_tabWidget->widget(0);
            m_tabWidget->removeTab(0);
            widget->deleteLater();
        }

        auto flxLogicProfileSize = it->second.size();

        for(size_t i = 0; i < flxLogicProfileSize; ++i)
            m_tabWidget->addTab(createTab(container, i), QString("Профиль %1").arg(i + 1));
    }
}

QList<uint16_t> FlxLogicTab::addrList() const
{
    const QList<uint16_t> requiredParams {SP_FLX_RUNSTAT};
    return requiredParams;
}
