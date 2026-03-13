#include "fatal_error_tab.h"

#include <QGridLayout>
#include <QTabWidget>

#include <dpc/gui/widgets/TableView.h>

#include "gui/forms/main/base_tab_model.h"

using namespace Dpc::Gui;

namespace {

constexpr const char *COLUMN_VALUE = "Значение";

const std::vector<std::pair<uint32_t, QString>> er_Description {
    {1, "Грубая программно-аппаратная ошибка (например обращение по несуществующему адресу)"},
    {2, "Ошибка блока управления памятью"},
    {3, "Ошибка шины"},
    {4, "Ошибка применения ресурса"},
    {5, "Сброс по сторожевому таймеру окна"},
    {6, "Ошибка в канале DMA"},
    {7, "Ошибка обмена по SPI или другому интерфейсу"},
    {8, "Ошибка сервиса RTOS"},
    {9, "Не классифицируемая ошибка"},
    {10, "Ошибка в стеке TCP/I протоколов"},
    {11, "Ошибка записи/стирания Flash памяти"},
    {12, "Переполнение стека задачи в RTOS"},
    {13, "Ошибка выделения места в куче RTOS"},
    {14, "Ошибка инициализации АЦП"},
    {15, "Ошибка инициализации кодека SAI"},
    {16, "Немаскируемое NMI исключение (прерывание)"},
    {17, "SVC исключение(прерывание)"},
    {18, "Pend SV исключение(прерывание) в смене режима работы микроконтроллера"},
    {19, "Debug Monitor исключение(прерывание)"},
    {20, "Необрабатываемое прерывание"},
    {21, "Ошибка выделения динамической памяти в системе"},
};

QString stateDescription(uint16_t position, uint32_t state)
{
    QStringList descriptions;
    for (const auto &desc: er_Description) {
        if (state == desc.first) {
            descriptions << desc.second;
        }
    }

    if (descriptions.empty())
        return QString();

    return descriptions.join("\n");
}

} // namespace

FatalErrorTab::FatalErrorTab(QWidget *parent)
    : AbstractDeviceTab(parent)
    , m_tabWidget(new QTabWidget(this))
{
    QGridLayout *tabLayout = new QGridLayout(this);

    tabLayout->addWidget(m_tabWidget);
}

QWidget *FatalErrorTab::createTab(const InformationOperation::ParamsContainer &container, uint16_t profile)
{
    auto tab = new QWidget;
    QGridLayout *tabLayout = new QGridLayout(tab);

    BaseTabModel *fatalErrorModel = new BaseTabModel(SP_FATALERROR_DUMP, profile, COLUMN_VALUE, tab, ::stateDescription);
    fatalErrorModel->updateInfo(container);

    if (fatalErrorModel->hasError()) {
        m_hasError.push_back(true);
    }
    else
        m_hasError.push_back(false);

    auto fatalErrorView = new TableView(fatalErrorModel, tab);

    fatalErrorView->verticalHeader()->setMinimumSectionSize(30);
    fatalErrorView->resizeColumnsToContents();
    fatalErrorView->resizeRowsToContents();
    tabLayout->addWidget(fatalErrorView);
    return tab;
}

void FatalErrorTab::deviceOperationFinished(AbstractOperation *op)
{
    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        const auto &container = info->params();

        const auto &it = container.find(SP_FATALERROR_DUMP);
        if (it == container.end())
            return;
        m_hasError.clear();

        while (m_tabWidget->count() > 0) {
            QWidget* widget = m_tabWidget->widget(0);
            m_tabWidget->removeTab(0);
            delete widget;
        }

        auto fatalErrorProfileSize = it->second.size();

        for (size_t i = 0; i < fatalErrorProfileSize; ++i) {
            m_tabWidget->addTab(createTab(container, i), QString("Профиль %1").arg(i + 1));

            if (m_hasError[i]){
                QIcon icon (":/icons/warning.svg");
                m_tabWidget->setTabIcon(i, icon);
            }
        }
    }
}

QList<uint16_t> FatalErrorTab::addrList() const
{
    const QList<uint16_t> requiredParams {SP_FATALERROR_DUMP};
    return requiredParams;
}

bool FatalErrorTab::hasError() const
{
    for (auto error: m_hasError)
        if (error)
            return true;

    return false;
}


