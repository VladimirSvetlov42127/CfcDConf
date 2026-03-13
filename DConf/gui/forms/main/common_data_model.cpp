#include "common_data_model.h"

#include <QAbstractItemModel>
#include <QBrush>
#include <QColor>

#include "device_templates/dc_templates_manager.h"
#include "data_model/dc_controller.h"

namespace {

const std::array g_WorkMode = {"Рабочий", "Подготовка"};

}

struct CommonDataModel::Item
{
    QString paramName;
    QString templateInfo;
    QString deviceInfo;
    uint16_t addr;
};

CommonDataModel::CommonDataModel(DcController *device, const ElementsInfoList &elementList, QObject *parent)
    : QAbstractTableModel(parent)
    , m_device(device)
    , m_elementList(elementList)
{
    for(const auto &element : elementList) {
        Item item;

        auto param = device->paramsRegistry().element(element.addr, element.index);
        if (param == nullptr)
            continue;

        item.paramName = param->name();
        item.templateInfo = param->value();
        item.addr = element.addr;

        m_items.append(item);
    }
}

int CommonDataModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return m_items.count();
}

int CommonDataModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;

    return CommonDataModel::Columns::ColumnsCount;
}

QVariant CommonDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (Qt::Horizontal == orientation && Qt::DisplayRole == role)
    {
        switch(section)
        {
        case Parameters:
            return "";
            break;
        case Template:
            return "Конфигурация";
            break;
        case Device:
            return "Устройство";
            break;
        }
    }

    return QVariant();
}

Qt::ItemFlags CommonDataModel::flags(const QModelIndex &index) const
{
    auto fl = QAbstractTableModel::flags(index);

    return fl;
}

QVariant CommonDataModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int col = index.column();
    int row = index.row();

    if (Columns::Parameters == col && Qt::TextAlignmentRole == role)
        return Qt::AlignVCenter;

    if (Qt::TextAlignmentRole == role)
        return Qt::AlignCenter;

    const auto &item = m_items[row];

    if (Qt::DisplayRole == role)
    {
        switch(col)
        {
        case Parameters:
            return item.paramName;
            break;
        case Template:
            return item.templateInfo;
            break;
        case Device:
            if (item.addr == SP_NEWPROFILE) {
                auto value = item.deviceInfo.toUInt();
                return value < g_WorkMode.size() ? g_WorkMode[value] : QString();
            }
            else
                return item.deviceInfo;
            break;
        }
    }

    if (Qt::BackgroundRole == role && index.column() == Device) {

        if (item.addr == SP_SOFTWARE_ID) {
            if (!gTemplatesManager.isCompatibleDevices(item.templateInfo.toInt(), item.deviceInfo.toInt()))
                return QBrush(QColor(Qt::red));
        }
        else if (item.addr == SP_HDWPARTNUMBER && item.templateInfo != item.deviceInfo) {
            return QBrush(QColor(Qt::red));
        }
        else if (item.addr == SP_SM_ENTITY_STATUS) {
            if (item.deviceInfo.toUInt() != 0)
                return QBrush(QColor(Qt::red));
        }
        else if (item.addr == SP_SOFTWARE_VERTION || item.addr == SP_CFGVER || item.addr == SP_IP4_ADDR) {
            if (item.templateInfo != item.deviceInfo)
                return QBrush(QColor(0xab9f35));
        }
    }

    return QVariant();
}

const CommonDataModel::ElementsInfoList &CommonDataModel::elementList() const
{
    return m_elementList;
}

void CommonDataModel::updateInfo(const InformationOperation::ParamsContainer &container)
{
    beginResetModel();

    m_items.clear();

    for(const auto &element : elementList()) {
        auto templateParam = m_device->paramsRegistry().element(element.addr, element.index);
        const InformationOperation::Element* deviceParam = nullptr;
        if (auto findIt = container.find(element.addr); findIt != container.end() && findIt->second.size()) {
            auto &paramElements = findIt->second.at(0);
            if (auto pelFindIt = paramElements.find(element.index); pelFindIt != paramElements.end() )
                deviceParam = &pelFindIt->second;
        }

        if (!templateParam && !deviceParam)
            continue;

        Item item;
        item.paramName = deviceParam ? deviceParam->name : templateParam->name();
        item.deviceInfo = deviceParam ? deviceParam->value : QString();
        item.templateInfo = templateParam ? templateParam->value() : QString();
        item.addr = element.addr;
        m_items.append(item);
    }

    endResetModel();
}
