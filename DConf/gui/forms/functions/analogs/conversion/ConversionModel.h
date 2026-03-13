#pragma once

#include <qabstractitemmodel.h>

class DcController;

class ConversionModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	enum Columns {
		NumColumn = 0,
		SourceColumn,
		DestinationColumn,
		ConversionTypeColumn,
		SettingColumn,

		ColumnsCount
	};

	ConversionModel(DcController* contr, QObject* parent = nullptr);

	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

	QString columnName(Columns col) const;
	std::vector<QVariantList> getItems() const;

private:
	DcController* m_contr;
	std::vector<QVariantList> m_items;
};