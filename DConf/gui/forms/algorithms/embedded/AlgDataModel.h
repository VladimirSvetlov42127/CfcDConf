#pragma once

#include <qabstractitemmodel.h>
#include <qset.h>

#include <data_model/parameters/parameter_element.h>

class ComboBoxDelegate;

struct AlgDataItem {
	enum Type {
		Line, 
		Check,
		List
	};

    ParameterElement* param = nullptr;
	Type type;
};

using AlgDataItemList = QList<AlgDataItem>;

class AlgDataModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	AlgDataModel(QObject *parent = nullptr);

	void setSettingHeaders(const QStringList &headers);
	void setGroupNameTemplate(const QString &temp);
	void setHighlightCurrentGroup(bool enable);
	void setGroupEnabled(int group, bool enabled);
	void setDelegate(int row, ComboBoxDelegate *delegate);
	void addItems(const AlgDataItemList &items);

	virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex &parent = QModelIndex()) const override; 
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex &index) const override;
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public slots:
	void setCurrentGroup(int group);

private:
	QStringList m_settingHeaders;
	QString m_groupNameTemplate;
	int m_currentGroup;
	bool m_highlightCurrentGroup;	
	QSet<int> m_disabledGroups;
	QList<AlgDataItemList> m_items;
	QHash<int, ComboBoxDelegate*> m_delegates;
};
