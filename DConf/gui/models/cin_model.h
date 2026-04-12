#ifndef CINMODEL_H
#define CINMODEL_H

#include <QAbstractTableModel>

#include "service_manager/signals/cin_signal.h"

class CinModel : public QAbstractTableModel
{
public:
    enum Column {
        Number = 0,
        Name,
        IEC101Select,
        IEC101Group1,
        IEC101Group2,
        IEC101Background,
        SpodesSelect,

        __Count
    };

    CinModel(const std::vector<CinSignal*>& items, QObject* parent = nullptr);
    virtual ~CinModel() = default;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

protected:
    const std::vector<CinSignal*>& items() const { return m_items; }

    template<typename T>
    static QVariant toVariant(const std::optional<T>& optional);

private:
    std::vector<CinSignal*> m_items;
};

template<typename T>
QVariant CinModel::toVariant(const std::optional<T>& optional)
{
    if (!optional)
        return QVariant();

    if constexpr (std::is_same_v<T, bool>)
        return optional.value() ? Qt::Checked : Qt::Unchecked;
    else
        return optional.value();
}

#endif // CINMODEL_H
