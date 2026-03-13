#include "sm_flags_tab.h"

#include <QGridLayout>

#include "device_operations/information_operation.h"
#include "gui/forms/main/sm_flags_model.h"

using namespace Dpc::Gui;

namespace
{

constexpr const char *COLUMN_VALUE = "Значение";
constexpr const std::array g_errorPositions = {7, 8, 9, 11, 14, 15, 16, 17, 18, 19, 20, 24, 25, 27, 28, 29, 30};

QString stateDescription(uint16_t position, uint32_t state)
{
    QStringList descriptions;
    if (state != 0) {
        for (const auto &erPos : g_errorPositions) {
            if (position == erPos) {
                descriptions << "Ошибка";
            }
        }
    }

    if(descriptions.empty())
        return QString();

    return descriptions.join("\n");
}

} // namespace

SmFlagsTab::SmFlagsTab(QWidget *parent)
    : AbstractDeviceTab(parent)
{
    m_smFlagsModel = new SmFlagsModel(SP_SM_ENTITY_STATUS, 0, COLUMN_VALUE, this, ::stateDescription);

    m_smFlagsView = new TableView(m_smFlagsModel, this);
    m_smFlagsView->hideColumn(BaseTabModel::Columns::Description);

    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(m_smFlagsView);
}

void SmFlagsTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        m_smFlagsModel->updateInfo(info->params());
    }

    m_smFlagsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_smFlagsView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

QList<uint16_t> SmFlagsTab::addrList() const
{
    QList<uint16_t> requiredParams = {SP_SM_ENTITY_STATUS};
    return requiredParams;
}

bool SmFlagsTab::hasError() const
{
    return m_smFlagsModel->hasError();
}
