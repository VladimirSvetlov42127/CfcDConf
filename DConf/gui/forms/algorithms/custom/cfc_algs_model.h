#ifndef CFCALGSMODEL_H
#define CFCALGSMODEL_H

#include <QAbstractTableModel>

#include "service_manager/services/alg_cfc/cfc_alg_manager.h"

class CfcAlgsModel : public QAbstractTableModel
{
public:
    enum Column {
        Number = 0,
        Compile,
        Enable,
        Name,
        Desc,

        __Count
    };

    CfcAlgsModel(CfcAlgManager* manager, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    CfcAlgService* create();
    void remove(CfcAlgService *cfcAlg);
    CfcAlgService* cfcAlg(const QModelIndex& index);
    const CfcAlgManager* cfcManager() const;

    static QString columnName(CfcAlgsModel::Column col);

private:
    CfcAlgManager* m_manager;
};

#endif // CFCALGSMODEL_H
