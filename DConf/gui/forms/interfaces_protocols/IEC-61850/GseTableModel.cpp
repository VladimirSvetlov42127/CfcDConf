#include "GseTableModel.h"

#include <qfont.h>

namespace {
}

GseTableModel::GseTableModel(QObject* parent) :
	QAbstractTableModel(parent)
{
}

int GseTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return RowsCount;
}

int GseTableModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	auto count = m_items.count();
	if (count)
		count += 1;
	return count;
}

QVariant GseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (auto data = m_items.value(section - 1); data)
			return data->cbName();
	}

	return QVariant();
}

Qt::ItemFlags GseTableModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags fl = QAbstractTableModel::flags(index);

	int col = index.column();
	if (!col)
		return fl;

	if (index.data().isValid())
		fl |= Qt::ItemIsEditable;

	return fl;
}

QVariant GseTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	int col = index.column();
	int row = index.row();		

	if (Qt::TextAlignmentRole == role) {
		return col ? Qt::AlignCenter : static_cast<QFlags<Qt::AlignmentFlag>::Int>(Qt::AlignLeft | Qt::AlignVCenter);
	}
	
	if (Qt::EditRole == role || Qt::DisplayRole == role) {
		if (!col)
			return rowName((Rows) row);

		auto data = m_items.at(col - 1);
		switch (row)
		{
		case Rows::MacAddrRow: return data->macAddress();
		case Rows::VlanIdRow: return data->vlanId().toUShort();
		case Rows::VlanPriorityRow: return data->vlanPriority().toShort();
		case Rows::AppIdRow: return QString("0x%1").arg(data->appID().toUpper());
		case Rows::MinTimeRow: return data->minTime().toUInt();
		case Rows::MaxTimeRow: return data->maxTime().toUInt();
		default: break;
		}
	}

	return QVariant();
}

bool GseTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
		return false;

	int row = index.row();
	int col = index.column();
	auto data = m_items[col - 1];	

	if (row < RowsCount) {
		if (row == Rows::MacAddrRow)
			data->setMacAddress(value.toString());
		if (row == Rows::VlanIdRow)
			data->setVlanId(QString::number(value.toInt()));
		if (row == Rows::VlanPriorityRow)
			data->setVlanPriority(QString::number(value.toInt()));
		if (row == Rows::MinTimeRow)
			data->setMinTime(QString::number(value.toInt()));
		if (row == Rows::MaxTimeRow)
			data->setMaxTime(QString::number(value.toInt()));
		if (row == Rows::AppIdRow) {
			bool good = true;
			int val = value.toString().toInt(&good, 16);
			if (!good)
				return false;

			data->setAppID(QString::number(val, 16));
		}

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

void GseTableModel::setGseList(const QList<GSEPtr>& list)
{
	beginResetModel();
	m_items = list;
	endResetModel();
}

QString GseTableModel::rowName(Rows c) const
{
	switch (c)
	{
	case Rows::MacAddrRow: return "MAC-Адрес";
	case Rows::VlanIdRow: return "Номер VLAN";
	case Rows::VlanPriorityRow: return "Приоритет VLAN";
	case Rows::AppIdRow: return "Идентификатор приложения";
	case Rows::MinTimeRow: return "Минимальный интервал (мс)";
	case Rows::MaxTimeRow: return "Максимальный интервал (мс)";
	}

	return QString();
}