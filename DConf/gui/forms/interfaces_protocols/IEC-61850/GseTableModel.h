#pragma once

#include <qabstractitemmodel.h>

#include <gui/forms/interfaces_protocols/IEC-61850/Communication.h>

class GseTableModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Rows {
		MacAddrRow,
		VlanIdRow,
		VlanPriorityRow,
		AppIdRow,
		MinTimeRow,
		MaxTimeRow,

		RowsCount
	};

	GseTableModel(QObject* parent = nullptr);

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	
	void setGseList(const QList<GSEPtr>& list);
	QString rowName(Rows c) const;


private:
	QList<GSEPtr> m_items;
};