#ifndef ANALOGSMODEL_H
#define ANALOGSMODEL_H

#include <QAbstractTableModel>

#include "data_model/dc_controller.h"

class AnalogsModel : public QAbstractTableModel
{
public:
    enum Columns
    {
        Trand,
        Analog,

        ColumnsCount
    };

    AnalogsModel(DcController *controller, QObject *parent = nullptr);

    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex& index, const QVariant& value, int = Qt::EditRole) override;

private:
    struct Item;

    QList<Item> m_items;
};

#endif // ANALOGSMODEL_H
