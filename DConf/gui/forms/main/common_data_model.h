#ifndef COMMON_DATA_MODEL_H
#define COMMON_DATA_MODEL_H

#include <QAbstractTableModel>

#include "device_operations/information_operation.h"

class DcController;

class CommonDataModel : public QAbstractTableModel
{
public:
    enum Columns
    {
        Parameters,
        Template,
        Device,

        ColumnsCount
    };

    struct ElementInfo
    {
        uint16_t addr;
        uint16_t index;
    };

    using ElementsInfoList = QList<ElementInfo>;

    CommonDataModel(DcController *device, const ElementsInfoList &elementList, QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    const ElementsInfoList &elementList() const;
    void updateInfo(const InformationOperation::ParamsContainer &container);

private:
    struct Item;

    DcController *m_device;
    ElementsInfoList m_elementList;
    QList<Item> m_items;
};

#endif // COMMON_DATA_MODEL_H
