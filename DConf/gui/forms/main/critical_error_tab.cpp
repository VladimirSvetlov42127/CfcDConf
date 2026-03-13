#include "critical_error_tab.h"

#include <QGridLayout>

#include <dpc/gui/widgets/TableView.h>

#include "device_operations/information_operation.h"
#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const uint16_t ADDR_CRITICAL_ERROR = 0x0A30;
constexpr const char *COLUMN_VALUE = "Ошибка";

} // namespace

CriticalErrorTab::CriticalErrorTab(QWidget *parent)
    : AbstractDeviceTab(parent)
{
    m_criticalErrorModel = new BaseTabModel(ADDR_CRITICAL_ERROR, 0, COLUMN_VALUE, this);

    auto criticalErrorView = new TableView(m_criticalErrorModel, this);
    criticalErrorView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    criticalErrorView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(criticalErrorView);
}

void CriticalErrorTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        m_criticalErrorModel->updateInfo(info->params());
    }
}

QList<uint16_t> CriticalErrorTab::addrList() const
{
    QList<uint16_t> requiredParams = { ADDR_CRITICAL_ERROR };
    return requiredParams;
}
