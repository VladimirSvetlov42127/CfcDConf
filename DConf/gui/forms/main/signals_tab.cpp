#include "signals_tab.h"

#include <QGridLayout>
#include <QTabWidget>

#include <dpc/gui/widgets/TableView.h>

#include "device_operations/information_operation.h"
#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const char *COLUMN_VALUE = "Значение";

constexpr const char *DINSNUM = "Дискретные входы";
constexpr const char *DOUTNUM = "Дискретные выходы";
constexpr const char *AINSNUM = "Аналоги";
constexpr const char *CINNUM = "Счётчики";

} // namespace

SignalsTab::SignalsTab(QWidget *parent)
    : AbstractDeviceTab(parent)
    , m_tabWidget(new QTabWidget(this))
{
    QGridLayout *tabLayout = new QGridLayout(this);
    tabLayout->addWidget(m_tabWidget);
}

QWidget *SignalsTab::createSignalsTabs(const InformationOperation::ParamsContainer &container, uint16_t addr)
{
    auto tab = new QWidget;

    BaseTabModel *signalsModel = new BaseTabModel(addr, 0, COLUMN_VALUE, tab);
    signalsModel->updateInfo(container);

    auto signalsView = new TableView(signalsModel, tab);
    signalsView->horizontalHeader()->setResizeContentsPrecision(QHeaderView::ResizeToContents);

    QGridLayout *tabLayout = new QGridLayout(tab);
    tabLayout->addWidget(signalsView);
    return tab;
}

void SignalsTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        QList<QString> signalsTabNames = {DINSNUM, DOUTNUM, AINSNUM, CINNUM};

        const auto &container = info->params();

        QList<uint16_t> existingAddr;
        for (int i = 0; i < addrList().size(); ++i) {
            auto addr = addrList().at(i);
            if (container.find(addr) == container.end())
                continue;

            existingAddr.append(addr);
        }

        if (!existingAddr.empty()) {
            while (m_tabWidget->count() > 0) {
                QWidget* widget = m_tabWidget->widget(0);
                m_tabWidget->removeTab(0);
                delete widget;
            }

            for (int i = 0; i < existingAddr.size(); ++i) {
                m_tabWidget->addTab(createSignalsTabs(container, existingAddr.at(i)), signalsTabNames[i]);
            }
        }
    }
}

QList<uint16_t> SignalsTab::addrList() const
{
    const QList<uint16_t> requiredParams {SP_DINSNUM, SP_DOUTNUM, SP_AINSNUM, SP_CINNUM};
    return requiredParams;
}
