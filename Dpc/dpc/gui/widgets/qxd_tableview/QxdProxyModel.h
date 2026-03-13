#ifndef QXDPROXYMODEL_H
#define QXDPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QSet>
#include <QIcon>

class QxdIFilter;
class QCollator;

class QxdProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT
public:
    QxdProxyModel(QObject *parent = nullptr);
    ~QxdProxyModel();

	virtual void setSourceModel(QAbstractItemModel *sourceModel) override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;    
    virtual bool canFetchMore(const QModelIndex &parent) const override { return false; }

    void addFilter(int column, QxdIFilter *filter);
    QxdIFilter* getFilter(int column) const;

    void setSortCaseSensitivityX(Qt::CaseSensitivity cs);

    // If column is -1, reset filters on all columns. Should Invalidate after resetFilter
    void resetFilter(int column = -1);

    void setUseSourceSort(const QSet<int> &columns);
    void setFilterIcon(const QIcon &icon);
    QIcon filterIcon() const;

    bool acceptedByFilters(int source_row, const QModelIndex &source_parent, int ignoreColumn = -1) const;

signals:
    void filteredCountChanged(int count);
    void filtersAboutToBeInvalidate();
    void filtersInvalidated();
    void sorted(int column);

public slots:
	void invalidateFilter();    

private slots:
	void clearInternalData();

protected:
	virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;
	virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    typedef QMap<int, QxdIFilter*> ColumnFilter; // key - column, value - filter
	QMap<int, ColumnFilter> m_filters;		  // key - filter type, value - map(column, filter)
    QCollator *m_collator;
    QSet<int> m_useSourceSort;
    QIcon m_filterIcon;
};

#endif // QXDPROXYMODEL_H
