#pragma once

#include <qabstractitemmodel.h>

#include <gui/forms/interfaces_protocols/IEC-61850/IED.h>

class SvTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Rows {
		WorkModeRow, 
		MacAddrRow,
		VlanIdRow,
		VlanPriorityRow,
		AppIdRow,
		ConfRevRow,
		SmvIdRow,
		SmpRateRow,
		NofAsduRow,

		RowsCount
	};

	SvTableModel(QObject* parent = nullptr);

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	
	void setItemList(const QList<SVControlPtr>& list);
	QString rowName(Rows c) const;


private:
	QList<SVControlPtr> m_items;
};