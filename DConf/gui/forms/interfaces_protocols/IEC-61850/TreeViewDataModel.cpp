#include "TreeViewDataModel.h"

#include <qdebug.h>
#include <qicon.h>
#include <qmenu.h>
#include <qmimedata.h>
#include <qstack.h>
#include <qbuffer.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

namespace {
	const QString FCDA_MIME_TYPE = "application/fcda";

	struct FcdaData
	{
		QString ldInst;
		QString prefix;
		QString lnClass;
		QString lnInst;
		QString doName;
		QString daName;
		QStringList fcList;
	};

	QDataStream& operator<<(QDataStream &stream, const FcdaData &f)
	{
		return stream << f.ldInst << f.prefix << f.lnClass << f.lnInst << f.doName << f.daName << f.fcList;
	}

	QDataStream& operator>>(QDataStream &stream, FcdaData &f)
	{
		return stream >> f.ldInst >> f.prefix >> f.lnClass >> f.lnInst >> f.doName >> f.daName >> f.fcList;
	}

	QString createUniqueName(BaseTreeItem *parent, BaseTreeItem::Type type, const QString &base) {
		QString name;
		int i = 0;
		bool found;
		do {
			name = QString("%1%2").arg(base).arg(++i);
			found = false;
			for (size_t j = 0; j < parent->childCount(); j++) {
				if (parent->child(j)->itemType() != type)
					continue;

				if (parent->child(j)->name() == name) {
					found = true;
					break;
				}
			}
		} while (found);

		return name;
	}

	bool isUniqueName(const QString &name, BaseTreeItem *item, BaseTreeItem::Type type)
	{
		for (size_t i = 0; i < item->parent()->childCount(); i++) {
			BaseTreeItem *it = item->parent()->child(i);
			if (it == item || it->itemType() != type)
				continue;
			if (it->name() == name)
				return false;
		}

		return true;
	}

	LDevicePtr ldFromTreeItem(BaseTreeItem *item)
	{
		do {
			if (item->itemType() == BaseTreeItem::LDType)
				return static_cast<LDeviceTreeItem*>(item)->ld();
			item = item->parent();
		} while (item);

		return LDevicePtr();
	}

	LNPtr lnFromTreeItem(BaseTreeItem *item)
	{
		do {
			if (item->itemType() == BaseTreeItem::LNType)
				return static_cast<LNTreeItem*>(item)->ln();
			item = item->parent();
		} while (item);

		return LNPtr();
	}

	DOPtr doFromTreeItem(BaseTreeItem *item)
	{
		do {
			if (item->itemType() == BaseTreeItem::DOType)
				return static_cast<DOTreeItem*>(item)->doPtr();
			item = item->parent();
		} while (item);

		return DOPtr();
	}

	std::tuple<QString, QStringList> daName(BaseTreeItem *item)
	{		
		QStack<BaseTreeItem*> stack;
		BaseTreeItem *it = item;
		while (it && it->itemType() != BaseTreeItem::DOType) {
			stack.push(it);
			it = it->parent();
		}

		QString res;
		QStringList fcList;
		while (!stack.isEmpty()) {
			BaseTreeItem *item = stack.pop();
			QString name = item->name();
			if (name.isEmpty()) {
				res.append(QString("(%1)").arg(item->row()));
			}
			else {
				if (!res.isEmpty())
					res.append(".");
				res.append(name);
			}
			
			if (item->itemType() == BaseTreeItem::SDOType) {
				SDOPtr p = static_cast<SDOTreeItem*>(item)->sdo();
				fcList = p->fcList();
			}

			if (item->itemType() == BaseTreeItem::DAType) {
				DAPtr p = static_cast<DATreeItem*>(item)->da();
				fcList = QStringList{ p->fc() };
			}
		}

		return std::make_tuple(res, fcList);
	}

} // namespace

TreeViewDataModel::TreeViewDataModel(const IEDPtr &ied, BaseTreeItem::Mode mode, FcSelectorFuncType func, QObject *parent) :
	QAbstractItemModel(parent),
	m_mode(mode), 
	m_fcSelectorFunc(func),
	m_rootItem(nullptr)
{
	m_rootItem = new IEDTreeItem(ied, "Модель данных", mode);
}

TreeViewDataModel::~TreeViewDataModel()
{
	//qDebug() << Q_FUNC_INFO;
	delete m_rootItem;
}

QVariant TreeViewDataModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();	

	BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());

	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		if (index.column() == 0)
			return !item->name().isEmpty() ? item->name() : QString("(%1)").arg(item->row());
	}

	if (role == Qt::DecorationRole) {
		if (index.column() == 0)
			switch (item->itemType())
			{
            case BaseTreeItem::LDType: return QIcon(":/icons/LD.svg");
            case BaseTreeItem::LNType: return QIcon(":/icons/LN.svg");
            case BaseTreeItem::DOType: return QIcon(":/icons/D_O.svg");
			case BaseTreeItem::SDOType:
			case BaseTreeItem::BDAType:
			case BaseTreeItem::FCDAType:
            case BaseTreeItem::DAType: return QIcon(":/icons/D_A.svg");
            case BaseTreeItem::DSType: return QIcon(":/icons/D_S.svg");
            case BaseTreeItem::RType: return QIcon(":/icons/R.svg");
            case BaseTreeItem::GSEType: return QIcon(":/icons/G_S.svg");
			default: return QVariant();
			}
	}
	return QVariant();
}

bool TreeViewDataModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if(!index.isValid())
		return false;

	BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());
	if (role == Qt::EditRole) {
		if (item->itemType() == BaseTreeItem::DSType) {
			DataSetTreeItem* dsItem = static_cast<DataSetTreeItem*>(item);
			QString newName = value.toString();
			if (!isUniqueName(newName, dsItem, BaseTreeItem::DSType))
				return false;

			dsItem->setName(newName);
			emit dataChanged(index, index);
			return true;
		}
		else if (item->itemType() == BaseTreeItem::RType) {
			ReportControlTreeItem* rcItem = static_cast<ReportControlTreeItem*>(item);
			QString newName = value.toString();
			if (!isUniqueName(newName, rcItem, BaseTreeItem::RType))
				return false;

			rcItem->setName(newName);
			emit dataChanged(index, index);
			return true;
		}
	}

	return false;
}

Qt::ItemFlags TreeViewDataModel::flags(const QModelIndex & index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	Qt::ItemFlags flags;
	flags |= Qt::ItemIsEnabled;

	BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());
	if (item->itemType() == BaseTreeItem::DAType || item->itemType() == BaseTreeItem::BDAType) {
		flags |= Qt::ItemIsSelectable;
		flags |= Qt::ItemIsDragEnabled;
	}
	else if (item->itemType() == BaseTreeItem::SDOType) {
		SDOPtr d = static_cast<SDOTreeItem*>(item)->sdo();
		if (d && d->fcList().size()) {
			flags |= Qt::ItemIsSelectable;
			flags |= Qt::ItemIsDragEnabled;
		}
	}
	else if (item->itemType() == BaseTreeItem::DOType) {
		DOPtr d = static_cast<DOTreeItem*>(item)->doPtr();
		if (d && d->fcList().size()) {
			flags |= Qt::ItemIsSelectable;
			flags |= Qt::ItemIsDragEnabled;
		}
	}
	else if (item->itemType() == BaseTreeItem::DSType) {
		flags |= Qt::ItemIsEditable;
		flags |= Qt::ItemIsSelectable;
		flags |= Qt::ItemIsDropEnabled;
	}
	else if (item->itemType() == BaseTreeItem::FCDAType) {
		flags |= Qt::ItemIsSelectable;
	}
	else if (item->itemType() == BaseTreeItem::RType) {
		flags |= Qt::ItemIsEditable;
		flags |= Qt::ItemIsSelectable;
	}

	return flags;
}

QVariant TreeViewDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

QModelIndex TreeViewDataModel::index(int row, int column, const QModelIndex & parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	BaseTreeItem *parentItem;
	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<BaseTreeItem*>(parent.internalPointer());

	BaseTreeItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);

	return QModelIndex();
}

QModelIndex TreeViewDataModel::parent(const QModelIndex & index) const
{
	if (!index.isValid())
		return QModelIndex();

	BaseTreeItem *childItem = static_cast<BaseTreeItem*>(index.internalPointer());
	BaseTreeItem *parentItem = childItem->parent();

	if (parentItem == m_rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeViewDataModel::rowCount(const QModelIndex & parent) const
{
	BaseTreeItem *parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = m_rootItem;
	else
		parentItem = static_cast<BaseTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int TreeViewDataModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return static_cast<BaseTreeItem*>(parent.internalPointer())->columnCount();

	return m_rootItem->columnCount();
}

Qt::DropActions TreeViewDataModel::supportedDropActions() const
{
	return Qt::CopyAction;
}

QMimeData * TreeViewDataModel::mimeData(const QModelIndexList & indexes) const
{
	QMimeData *mimeData = new QMimeData;

	QBuffer buffer;
	buffer.open(QIODevice::WriteOnly);
	QDataStream stream(&buffer);

	for (const QModelIndex &index : indexes) {
		if (!index.isValid())
			continue;

		BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());
		LDevicePtr ld = ::ldFromTreeItem(item);
		if (!ld)
			continue;

		LNPtr ln = ::lnFromTreeItem(item);
		if (!ln)
			continue;

		DOPtr doPtr = ::doFromTreeItem(item);
		if (!doPtr)
			continue;

		QString da;
		QStringList fcList;
		std::tie(da, fcList) = daName(item);
		if (fcList.isEmpty())
			fcList = doPtr->fcList();

		FcdaData data{ ld->inst(), ln->prefix(), ln->lnClass(), ln->inst(), doPtr->name(), da, fcList };
		stream << data;
	}

	mimeData->setData(FCDA_MIME_TYPE, buffer.buffer());
	return mimeData;
}

QStringList TreeViewDataModel::mimeTypes() const
{
	QStringList types;
	types << FCDA_MIME_TYPE;
	return types;
}

bool TreeViewDataModel::dropMimeData(const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent)
{
	if (action == Qt::IgnoreAction)
		return true;

	if (!parent.isValid())
		return false;

	BaseTreeItem *item = static_cast<BaseTreeItem*>(parent.internalPointer());
	if (item->itemType() != BaseTreeItem::DSType)
		return false;

	DataSetTreeItem *dsTreeItem = static_cast<DataSetTreeItem*>(item);
	QByteArray encodedData = data->data(FCDA_MIME_TYPE);
	QBuffer buffer(&encodedData);
	buffer.open(QIODevice::ReadOnly);
	QDataStream stream(&buffer);
	
	while (!stream.atEnd()) {
		FcdaData d;
		stream >> d;

		QString fc = d.fcList.first();
		if (d.fcList.size() > 1) {
			QString fcdaName = QString("%1/%2%3%4.%5").arg(d.ldInst, d.prefix, d.lnClass, d.lnInst, d.doName);
			int idx = m_fcSelectorFunc(fcdaName, d.fcList);
			if (idx < 0)
				continue;
			fc = d.fcList.at(idx);
		}

		beginInsertRows(parent, dsTreeItem->childCount(), dsTreeItem->childCount());
		dsTreeItem->appendFCDA(d.ldInst, d.prefix, d.lnClass, d.lnInst, d.doName, d.daName, fc);
		endInsertRows();
	}

	return true;
}

MenuPtr TreeViewDataModel::contextMenuForIndex(const QModelIndex & index, const QModelIndexList &selectedItems)
{
	MenuPtr menu;
	if (!index.isValid())
		return menu;

	auto removeIndex = [=](const QModelIndex &index) {
		BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());
		beginRemoveRows(index.parent(), item->row(), item->row());
		item->parent()->removeChild(item->row());
		endRemoveRows();
	};
	
	BaseTreeItem *item = static_cast<BaseTreeItem*>(index.internalPointer());
	if (item->itemType() == BaseTreeItem::LNType) {
		menu = std::make_shared<QMenu>();
		QAction *act = menu->addAction("Добавить датасет", [=]() {
			LNTreeItem *item = static_cast<LNTreeItem*>(index.internalPointer());

			beginInsertRows(index, item->childCount(), item->childCount());
			item->appendDataSet(createUniqueName(item, BaseTreeItem::DSType, "Dataset"));
			endInsertRows();
		});
		act->setData(AddOperation);

		act = menu->addAction("Добавить отчёт", [=]() {
			LNTreeItem *item = static_cast<LNTreeItem*>(index.internalPointer());

			beginInsertRows(index, item->childCount(), item->childCount());
			item->appendReport(createUniqueName(item, BaseTreeItem::RType, "Report"));
			endInsertRows();
		});
		act->setData(AddOperation);
	}
	else if (item->itemType() == BaseTreeItem::DSType || item->itemType() == BaseTreeItem::FCDAType || item->itemType() == BaseTreeItem::RType) {
		menu = std::make_shared<QMenu>();
		QAction *act = menu->addAction("Удалить", [=]() {
			if (!MsgBox::question("Вы уверены что хотите удалить выбраные элементы?"))
				return;

			QModelIndexList all = selectedItems;
			if (!all.contains(index))
				all.append(index);

			// сначала удаляем FCDA
			auto it = all.begin();
			while (it != all.end()) {
				if (static_cast<BaseTreeItem*>(it->internalPointer())->itemType() == BaseTreeItem::FCDAType) {
					removeIndex(*it);
					it = all.erase(it);
				}
				else
					it++;
			}

			// потом удаляем остальное
			for (auto &it : all)
				removeIndex(it);
		});
		act->setData(RemoveOperation);
	}
	
	return menu;
}
