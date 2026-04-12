#ifndef DPINMODEL_H
#define DPINMODEL_H

#include <QAbstractTableModel>

#include "service_manager/signals/dpin_signal.h"

class DPinModel : public QAbstractTableModel
{
public:
    enum Column {
        Number = 0,
        RPO,
        RPV,
        IntermediateTime,
        IntermediateTimeRatio,
        NotGenUndef,
        Active,
        Inversion,
        State,

        __Count
    };

    explicit DPinModel(const std::vector<DPinSignal*>& items, QObject* parent = nullptr);
    virtual ~DPinModel() = default;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
    const std::vector<DPinSignal*>& items() const { return m_items; }

    template<typename T>
    static QVariant toVariant(const std::optional<T>& optional);

private:
    std::vector<DPinSignal*> m_items;
};

template<typename T>
QVariant DPinModel::toVariant(const std::optional<T>& optional)
{
    if (!optional)
        return QVariant();

    if constexpr (std::is_same_v<T, bool>)
        return optional.value() ? Qt::Checked : Qt::Unchecked;
    else
        return optional.value();
}

#endif // DPINMODEL_H
