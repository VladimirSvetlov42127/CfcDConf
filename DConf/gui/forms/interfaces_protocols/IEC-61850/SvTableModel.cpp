#include "SvTableModel.h"

#include <qfont.h>

namespace {
}

SvTableModel::SvTableModel(QObject* parent) :
	QAbstractTableModel(parent)
{
}

int SvTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return RowsCount;
}

int SvTableModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	auto count = m_items.count();
	if (count)
		count += 1;
	return count;
}

QVariant SvTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (auto data = m_items.value(section - 1); data)
			return QString("Поток %1").arg(section);
	}

	return QVariant();
}

Qt::ItemFlags SvTableModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags fl = QAbstractTableModel::flags(index);

	int col = index.column();
	if (!col)
		return fl;

	if (index.data().isValid())
		fl |= Qt::ItemIsEditable;

	return fl;
}

QVariant SvTableModel::data(const QModelIndex& index, int role) const
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
		case Rows::WorkModeRow: return data->workMode();
		case Rows::MacAddrRow: return data->smvAP()->macAddress();
		case Rows::VlanIdRow: return data->smvAP()->vlanId().toUShort();
		case Rows::VlanPriorityRow: return data->smvAP()->vlanPriority().toShort();
		case Rows::AppIdRow: return QString("0x%1").arg(data->smvAP()->appID().toUpper());
		case Rows::ConfRevRow: return data->confRev();
		case Rows::SmvIdRow: return data->smvID();
		case Rows::SmpRateRow: return data->smpRate();
		case Rows::NofAsduRow: return data->nofASDU();
		default: break;
		}
	}

	return QVariant();
}

bool SvTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
		return false;

	int row = index.row();
	int col = index.column();
	auto data = m_items[col - 1];	

	if (row < RowsCount) {
		if (row == Rows::WorkModeRow)
			data->setWorkMode(value.toInt());
		if (row == Rows::MacAddrRow)
			data->smvAP()->setMacAddress(value.toString());
		if (row == Rows::VlanIdRow)
			data->smvAP()->setVlanId(QString::number(value.toInt()));
		if (row == Rows::VlanPriorityRow)
			data->smvAP()->setVlanPriority(QString::number(value.toInt()));
		if (row == Rows::AppIdRow) {
			bool good = true;
			int val = value.toString().toInt(&good, 16);
			if (!good)
				return false;

			data->smvAP()->setAppID(QString::number(val, 16));
		}
		if (row == Rows::ConfRevRow)
			data->setConfRev(value.toInt());
		if (row == Rows::SmvIdRow)
			data->setSmvID(value.toString());
		if (row == Rows::SmpRateRow) {
			auto smpRate = value.toInt();
			data->setSmpRate(smpRate);
			if (smpRate == 256 && data->nofASDU() < 4) {
				setData(this->index(NofAsduRow, col), 4);
			}
		}
		if (row == Rows::NofAsduRow)
			data->setNofASDU(value.toInt());

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

void SvTableModel::setItemList(const QList<SVControlPtr>& list)
{
	beginResetModel();
	m_items = list;
	endResetModel();
}

QString SvTableModel::rowName(Rows c) const
{
	switch (c)
	{
	case Rows::WorkModeRow: return "Режим работы";
	case Rows::MacAddrRow: return "MAC-Адрес";
	case Rows::VlanIdRow: return "Номер VLAN";
	case Rows::VlanPriorityRow: return "Приоритет VLAN";
	case Rows::AppIdRow: return "Идентификатор приложения (App ID)";
	case Rows::ConfRevRow: return "Номер ревизии (ConfRev)";
	case Rows::SmvIdRow: return "Идентификатор (Smv ID)";
	case Rows::SmpRateRow: return "Частота дискретизации (Smp Rate)";
	case Rows::NofAsduRow: return "Число срезов ASDU";
	}

	return QString();
}