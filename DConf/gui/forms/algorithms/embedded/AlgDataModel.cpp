#include "AlgDataModel.h"


#include <gui/forms/algorithms/embedded/DcAlgDefines.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

namespace {
	const QColor CurrentProfie = QColor("#7AB1C9");
}

AlgDataModel::AlgDataModel(QObject *parent) :
	QAbstractTableModel(parent),
	m_groupNameTemplate("Группа %1"),
	m_currentGroup(-1),
	m_highlightCurrentGroup(false)
{
}

void AlgDataModel::setSettingHeaders(const QStringList & headers)
{
	m_settingHeaders = headers;
}

void AlgDataModel::setGroupNameTemplate(const QString & temp)
{
	m_groupNameTemplate = temp;
}

void AlgDataModel::setHighlightCurrentGroup(bool enable)
{
	m_highlightCurrentGroup = enable;
}

void AlgDataModel::setGroupEnabled(int group, bool enabled)
{
	if (enabled)
		m_disabledGroups.remove(group);
	else
		m_disabledGroups.insert(group);

	emit dataChanged(index(0, group), index(rowCount(), group));
}

void AlgDataModel::setDelegate(int row, ComboBoxDelegate * delegate)
{
	m_delegates.insert(row, delegate);
}

void AlgDataModel::addItems(const AlgDataItemList & items)
{
	m_items << items;
}

int AlgDataModel::rowCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;

	return m_settingHeaders.count();
}

int AlgDataModel::columnCount(const QModelIndex & parent) const
{
	if (parent.isValid())
		return 0;

	return m_items.count();
}

QVariant AlgDataModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return m_groupNameTemplate.arg(section + 1);

	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
		return m_settingHeaders.value(section);

	/*if (role == Qt::BackgroundColorRole)
		return CurrentProfie;*/

	return QVariant();
}

Qt::ItemFlags AlgDataModel::flags(const QModelIndex & index) const
{
	Qt::ItemFlags fl = QAbstractTableModel::flags(index);
	if (m_disabledGroups.contains(index.column())) {
		fl.setFlag(Qt::ItemIsEnabled, false);
		return fl;
	}

	if (index.data().isValid() )
		fl |= Qt::ItemIsEditable;

	QVariant val = index.data(Qt::CheckStateRole);
	if (val == Qt::Checked || val == Qt::Unchecked) {
		fl |= Qt::ItemIsUserCheckable;
		fl.setFlag(Qt::ItemIsSelectable, false);
	}

	
	return fl;
}

QVariant AlgDataModel::data(const QModelIndex & index, int role) const
{
	if (!index.isValid())
		return QVariant();
	
	if (Qt::TextAlignmentRole == role)
		return Qt::AlignCenter;

	if (Qt::BackgroundRole == role) {
		if (m_currentGroup == index.column())
			return Algs::Colors::CurrentProfie;
	}

	auto item = m_items.at(index.column()).at(index.row());
	if (!item.param)
		return QVariant();

	if (Qt::DisplayRole == role  || Qt::EditRole == role) {
		if (item.type == AlgDataItem::Line)
			return item.param->value();

		if (item.type == AlgDataItem::List)
			return item.param->value().toUInt();
	}

	if (Qt::CheckStateRole == role) {
		if (item.type == AlgDataItem::Check)
			return item.param->value().toUInt() ? Qt::Checked : Qt::Unchecked;
	}	

	return QVariant();
}

bool AlgDataModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	auto item = m_items.at(index.column()).at(index.row());
	if (Qt::EditRole == role && AlgDataItem::Check != item.type) {
		item.param->updateValue(value.toString());

		emit dataChanged(index, index);
		return true;
	}

	if (Qt::CheckStateRole == role && AlgDataItem::Check == item.type) {
		item.param->updateValue(value.toBool() ? "1" : "0");

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

void AlgDataModel::setCurrentGroup(int group)
{
	if (group == m_currentGroup)
		return;

	auto oldCurrentGroup = m_currentGroup;
	m_currentGroup = group;

	int column = oldCurrentGroup;
	emit dataChanged(index(0, column), index(rowCount(), column));

	column = m_currentGroup;
	emit dataChanged(index(0, column), index(rowCount(), column));
}