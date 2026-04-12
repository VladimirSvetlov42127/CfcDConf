#ifndef DOUTMODEL_H
#define DOUTMODEL_H

#include <QAbstractTableModel>

#include "service_manager/signals/dout_signal.h"

class DoutModel : public QAbstractTableModel
{
public:
    enum Column {
        Number = 0,
        Name,
        Impulse,

        __Count
    };

    DoutModel(const std::vector<DoutSignal*>& items, QObject* parent = nullptr);
    virtual ~DoutModel() = default;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
    const std::vector<DoutSignal*>& items() const { return m_items; }

    template<typename T>
    static QVariant toVariant(const std::optional<T>& optional);

private:
    std::vector<DoutSignal*> m_items;
};

template<typename T>
QVariant DoutModel::toVariant(const std::optional<T>& optional)
{
    if (!optional)
        return QVariant();

    if constexpr (std::is_same_v<T, bool>)
        return optional.value() ? Qt::Checked : Qt::Unchecked;
    else
        return optional.value();
}

#endif // DOUTMODEL_H
