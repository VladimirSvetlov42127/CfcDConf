#ifndef DINMODEL_H
#define DINMODEL_H

#include <QAbstractTableModel>

#include "service_manager/signals/din_signal.h"

class DinModel : public QAbstractTableModel
{
public:
    enum Column {
        Number = 0,
        Name,
        Oscill,
        Journal,
        Archive,
        Drebezg,
        DrebezgRatio,
        DrebezgTimer,
        DrebezgNoise,
        Save,
        Fix1,
        Inversion,
        ThresholdChain,
        Threshold0,
        Threshold1,
        ThresholdKZ,
        DP,
        IEC101Select,
        IEC101Group1,
        IEC101Group2,
        IEC101Background,
        IEC103Select,
        SpodesSelect,

        __Count
    };

    DinModel(const std::vector<DinSignal*>& items, QObject* parent = nullptr);
    virtual ~DinModel() = default;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
    const std::vector<DinSignal*>& items() const { return m_items; }

    template<typename T>
    static QVariant toVariant(const std::optional<T>& optional);

private:
    std::vector<DinSignal*> m_items;
};

template<typename T>
QVariant DinModel::toVariant(const std::optional<T>& optional)
{
    if (!optional)
        return QVariant();

    if constexpr (std::is_same_v<T, bool>)
        return optional.value() ? Qt::Checked : Qt::Unchecked;
    else
        return optional.value();
}

#endif // DINMODEL_H
