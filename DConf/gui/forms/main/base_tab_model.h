#ifndef BASE_TAB_MODEL_H
#define BASE_TAB_MODEL_H

#include <QAbstractTableModel>

#include "device_operations/information_operation.h"

class BaseTabModel : public QAbstractTableModel
{
public:
    using DescriptionFunc = std::function<QString(uint16_t position, uint32_t state)>;

    enum Columns
    {
        Parametr,
        Value,
        Description,

        ColumnsCount
    };

    BaseTabModel(uint16_t addr, uint16_t profile, const QString &columnValue, QObject *parent = nullptr, const DescriptionFunc &descriptionFunc = nullptr);
    ~BaseTabModel() override;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    void updateInfo(const InformationOperation::ParamsContainer &container);
    bool hasError();

private:
    struct Item;

    QList<Item> m_items;

    uint16_t m_addr;
    uint16_t m_profile;
    const QString m_columnValue;
    DescriptionFunc m_descriptionFunc;
    bool m_descExist;
};

#endif // BASE_TAB_MODEL_H
