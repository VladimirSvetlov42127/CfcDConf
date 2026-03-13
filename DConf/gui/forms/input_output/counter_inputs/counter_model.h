#ifndef COUNTER_MODEL_H
#define COUNTER_MODEL_H

#include <QAbstractTableModel>

#include "data_model/dc_controller.h"



class CounterModel : public QAbstractTableModel
{
public:
    enum Columns
    {
        NumberColumn = 0,
        NameColumn,

        ColumnsCount
    };

    CounterModel(DcController *controller, QObject *parent = nullptr);
    virtual ~CounterModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

private:
    DcController *m_controller;
    QList<DcSignal*> m_items;
};

#endif // COUNTER_MODEL_H
