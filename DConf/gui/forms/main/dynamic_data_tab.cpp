#include "dynamic_data_tab.h"

#include <QGridLayout>

#include <dpc/gui/widgets/TableView.h>

#include "device_operations/information_operation.h"
#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const uint16_t ADDR_DYNAMIC_DATA = 0x0830;
constexpr const char *COLUMN_VALUE = "Значение";

} // namespace

DynamicDataTab::DynamicDataTab(QWidget *parent)
    : AbstractDeviceTab(parent)
{
    m_dynamicDataModel = new BaseTabModel(ADDR_DYNAMIC_DATA, 0, COLUMN_VALUE, this);

    auto dynamicDataView = new TableView(m_dynamicDataModel, this);
    dynamicDataView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    dynamicDataView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(dynamicDataView);
}

void DynamicDataTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        m_dynamicDataModel->updateInfo(info->params());
    }
}

QList<uint16_t> DynamicDataTab::addrList() const
{
    const QList<uint16_t> requiredParams = {ADDR_DYNAMIC_DATA};
    return requiredParams;
}
