#include "common_data_tab.h"

#include <QGridLayout>

#include "data_model/dc_controller.h"
#include "device_operations/information_operation.h"
#include "gui/forms/main/common_data_model.h"

using namespace Dpc::Gui;

CommonDataTab::CommonDataTab(DcController *device, QWidget *parent)
    : AbstractDeviceTab(parent)
{
    CommonDataModel::ElementsInfoList elementList;
    elementList.append({SP_DEVICE_NAME, 0});
    elementList.append({SP_HDWPARTNUMBER, 0});
    elementList.append({SP_SOFTWARE_ID, 0});
    elementList.append({SP_SOFTWARE_VERTION, 1});
    elementList.append({SP_SOFTWARE_VERTION, 0});
    elementList.append({SP_CFGVER, 0});
    elementList.append({SP_SERNUM, 0});
    elementList.append({SP_IP4_ADDR, 0});
    elementList.append({SP_NEWPROFILE, 0});
    elementList.append({SP_INTTEMP, 0});
    elementList.append({SP_SM_ENTITY_STATUS, 7});
    elementList.append({SP_SM_ENTITY_STATUS, 8});
    m_commonDataModel = new CommonDataModel(device, elementList, this);

    m_commonDataView = new TableView(m_commonDataModel, this);
    m_commonDataView->hideColumn(CommonDataModel::Columns::Device);
    m_commonDataView->setColumnWidth(CommonDataModel::Columns::Parameters, 350);
    m_commonDataView->setColumnWidth(CommonDataModel::Columns::Template, 350);
    m_commonDataView->setColumnWidth(CommonDataModel::Columns::Device, 350);

    QGridLayout *commonDataLayout = new QGridLayout(this);
    commonDataLayout->addWidget(m_commonDataView);
}

void CommonDataTab::deviceOperationFinished(AbstractOperation *op)
{
    if  (op == nullptr)
        m_commonDataView->hideColumn(CommonDataModel::Columns::Device);

    if (auto info = dynamic_cast<InformationOperation*>(op); info) {
        if (AbstractOperation::ErrorState == info->state())
            return;

        updateData(info->params());
    }
}

QList<uint16_t> CommonDataTab::addrList() const
{
    QList<uint16_t>requiredParams ;
    for(const auto &element : m_commonDataModel->elementList())
        requiredParams << element.addr;

    return requiredParams;
}

void CommonDataTab::updateData(const InformationOperation::ParamsContainer &container)
{
    if(m_commonDataView->isColumnHidden(CommonDataModel::Columns::Device))
        m_commonDataView->showColumn(CommonDataModel::Columns::Device);

    m_commonDataModel->updateInfo(container);
}

