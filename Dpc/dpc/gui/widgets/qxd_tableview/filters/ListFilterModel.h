#ifndef LISTFILTERMODEL_H
#define LISTFILTERMODEL_H

#include <QSortFilterProxyModel>
#include <unordered_map>

#include <dpc/gui/widgets/qxd_tableview/filters/ListFilter.h>

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
namespace std {
    template<> struct hash<QString> {
        std::size_t operator()(const QString& s) const {
            return qHash(s);
        }
    };
}
#endif

struct ListFilterItem
{
    ListFilterItem() : checked(true), isMatchingMask(false), isAlreadyFiltered(false), isOtherAccepted(false) {}
    bool checked;
    bool isMatchingMask;
    bool isAlreadyFiltered;
    bool isOtherAccepted;
};

typedef std::function<bool(int)>  OtherFiltersAcceptFunc;
typedef std::unordered_map<QString, ListFilterItem> ListFilterItemsHash;

class ListFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ListFilterModel(ListFilter *filter, OtherFiltersAcceptFunc accept, QObject *parent = nullptr);
    ~ListFilterModel();

    virtual bool canFetchMore(const QModelIndex &parent) const override { return false; }
    virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
    virtual QVariant data(const QModelIndex &proxyIndex, int role) const override;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    virtual void setSourceModel(QAbstractItemModel *sourceModel) override;

    Qt::CheckState allCheckState() const;
    void setAllCheckState(bool checked);

signals:
    void allCheckStateChanged(Qt::CheckState);

public slots:
    void saveToFilter();
    void setMask(const QString &mask);
    void setCaseSensitivity(Qt::CaseSensitivity cs);
    void setExactMatch(bool match);

    void setValue1(const QVariant &val);
    void setValue2(const QVariant &val);
    void setOperator1(Qxd::CompareOperator op);
    void setOperator2(Qxd::CompareOperator op);
    void setCombination(Qxd::Combination comb);

protected:
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    inline ListFilterItemsHash::iterator getIt(const QModelIndex &proxyIndex) const;
    inline bool compareIsValid() const;
    inline bool matchCompares(const QModelIndex &index) const;
    inline bool matchMask(const QString &str, const QModelIndex &index) const;
    void update();

private:
    ListFilter *m_filter;
    QString m_mask;
    Qt::CaseSensitivity m_caseSens;
    bool m_exactMatch;
    QVariant m_value1;
    QVariant m_value2;
    Qxd::CompareOperator m_comp1;
    Qxd::CompareOperator m_comp2;
    Qxd::Combination m_comb;
    OtherFiltersAcceptFunc m_otherAccept;
    mutable ListFilterItemsHash m_items;
};

#endif // LISTFILTERMODEL_H
