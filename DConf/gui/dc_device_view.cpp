#include "dc_device_view.h"

#include <QDebug>
#include <QStandardItemModel>
//#include <QItemSelectionModel>
#include <QHeaderView>

#include "gui/forms/DcMenu.h"
#include "data_model/dc_controller.h"

namespace {

enum {
    FormIdxRole = Qt::UserRole + 1
};

} // namespace

DcDeviceView::DcDeviceView(DcController *config, QWidget *parent)
    : QTreeView{parent}
    , m_model{new QStandardItemModel(this)}
    , m_config{config}
{
    setModel(m_model);
    setIconSize(QSize(24, 24));
    setContextMenuPolicy(Qt::CustomContextMenu);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setEditTriggers(QAbstractItemView::EditKeyPressed);
//    setItemDelegateForColumn(0, new ProgressDelegate(this));
    header()->hide();
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &DcDeviceView::onSelectionChanged);

    auto deviceViewItem = new QStandardItem("Устройство");
    deviceViewItem->setEditable(false);
    deviceViewItem->setIcon(config->icon());
    m_model->appendRow(deviceViewItem);

    if (!DcMenu::root().isAvailableFor(config))
        return;

    auto configViewItem = new QStandardItem("Конфигурация");
    configViewItem->setEditable(false);
    configViewItem->setIcon(DcMenu::root().icon());
    configViewItem->setData(DcMenu::root().formIdx(), FormIdxRole);
    m_model->appendRow(configViewItem);

    for (auto &it : DcMenu::root().childs())
        addSubmenu(it.second, configViewItem);

    expand(m_model->indexFromItem(configViewItem));
}

DcDeviceView::~DcDeviceView()
{
}

DcController *DcDeviceView::config() const
{
    return m_config;
}

void DcDeviceView::onSelectionChanged(const QItemSelection &selected, [[maybe_unused]] const QItemSelection &deselected)
{
    QString itemIdx;
    bool isConfigItem = false;
    auto item = m_model->itemFromIndex(selected.indexes().value(0));
    if (item) {
        itemIdx = item->data(FormIdxRole).toString();
        isConfigItem = item->row() > 0 || item->parent();
    }

    emit itemSelected(itemIdx, isConfigItem);
}

void DcDeviceView::addSubmenu(DcMenu *menu, QStandardItem *parent_view_item)
{
    if (!menu->isAvailableFor(config()))
        return;

    QStandardItem* menu_view_item = new QStandardItem(menu->icon(), menu->title());
    menu_view_item->setEditable(false);
    menu_view_item->setData(menu->formIdx(), FormIdxRole);
    parent_view_item->appendRow(menu_view_item);

    for (auto &it : menu->childs())
        addSubmenu(it.second, menu_view_item);
}
