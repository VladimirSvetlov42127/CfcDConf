#pragma once

#include <qabstractitemmodel.h>

#include "TreeItems.h"

class QMenu;
typedef std::shared_ptr<QMenu> MenuPtr;

class TreeViewDataModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	using FcSelectorFuncType = std::function<int(const QString&, const QStringList&)>;

	enum Operation {
		AddOperation,
		RemoveOperation
	};

	TreeViewDataModel(const IEDPtr &ied, BaseTreeItem::Mode mode, FcSelectorFuncType func, QObject *parent = nullptr);
	~TreeViewDataModel();

	QVariant data(const QModelIndex &index, int role) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	Qt::DropActions supportedDropActions() const override;
	QMimeData* mimeData(const QModelIndexList &indexes) const;
	QStringList mimeTypes() const;
	bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);

	MenuPtr contextMenuForIndex(const QModelIndex &index, const QModelIndexList &selectedItems);

private:
	BaseTreeItem *m_rootItem;
	BaseTreeItem::Mode m_mode;

	FcSelectorFuncType m_fcSelectorFunc;
};