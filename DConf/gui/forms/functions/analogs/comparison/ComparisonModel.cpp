#include "ComparisonModel.h"

#include "data_model/dc_controller.h"

namespace {
	const uint16_t CUSTOM_CONVERSION_TYPE_VALUE = 3;
}

ComparisonModel::ComparisonModel(DcController* contr, QObject* parent) :
	QAbstractTableModel(parent),
	m_contr(contr)
{
    auto param = contr->paramsRegistry().parameter(SP_AINCMP_WORDAIN_PARAM);
	if (!param)
		return;

    for (int i = 0; i < param->elementsCount(); i++) {
		QVariantList data;
		data.append(i + 1);
		data.append(contr->getValue(SP_AINCMP_WORDAIN_PARAM, i).toUInt());
		data.append(contr->getValue(SP_AINCMP_OUTWORD_PARAM, i).toUInt());
		data.append(contr->getValue(SP_AINCMP_BYTE_PARAM, i * PROFILE_SIZE).toUInt());
		data.append(contr->getValue(SP_AINCMP_FLOAT_PARAM, i * PROFILE_SIZE + 0).toFloat());
		data.append(contr->getValue(SP_AINCMP_FLOAT_PARAM, i * PROFILE_SIZE + 1).toFloat());
		data.append(contr->getValue(SP_AINCMP_FLOAT_PARAM, i * PROFILE_SIZE + 2).toFloat());

		m_items.push_back(data);
	}
}

int ComparisonModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return m_items.size();
}

int ComparisonModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;

	return ColumnsCount;
}

QVariant ComparisonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return columnName((Columns)section);

	return QVariant();
}

Qt::ItemFlags ComparisonModel::flags(const QModelIndex& index) const
{
	Qt::ItemFlags fl = QAbstractTableModel::flags(index);

	int col = index.column();
	if (!col)
		return fl;

	if (index.data().isValid())
		fl |= Qt::ItemIsEditable;

	return fl;
}

QVariant ComparisonModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	auto data = m_items.at(index.row());
	int col = index.column();

	if (Qt::TextAlignmentRole == role)
		return Qt::AlignCenter;

	if (Qt::EditRole == role || Qt::DisplayRole == role) {		
		return data.value(col);
	}

	return QVariant();
}

bool ComparisonModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	int row = index.row();
	int col = index.column();
	auto& data = m_items[row];

	if (!index.isValid())
		return false;

	if (col < ColumnsCount) {
		data[col] = value;

		if (col == Columns::SourceColumn)
			m_contr->setValue(SP_AINCMP_WORDAIN_PARAM, row, value);
		if (col == Columns::DestinationColumn)
			m_contr->setValue(SP_AINCMP_OUTWORD_PARAM, row, value);
		if (col == Columns::CompareTypeColumn)
			m_contr->setValue(SP_AINCMP_BYTE_PARAM, row * PROFILE_SIZE, value);
		if (col == Columns::ThresholdColumn)
			m_contr->setValue(SP_AINCMP_FLOAT_PARAM, row * PROFILE_SIZE + 0, value);
		if (col == Columns::HysteresisColumn)
			m_contr->setValue(SP_AINCMP_FLOAT_PARAM, row * PROFILE_SIZE + 1, value);
		if (col == Columns::ResponseTimeColumn)
			m_contr->setValue(SP_AINCMP_FLOAT_PARAM, row * PROFILE_SIZE + 2, value);

		emit dataChanged(index, index);
		return true;
	}

	return false;
}

QString ComparisonModel::columnName(Columns col) const
{
	switch (col)
	{
	case Columns::NumColumn: return "№";
	case Columns::SourceColumn: return "Исходный аналог";
	case Columns::DestinationColumn: return "Дискрет результата";
	case Columns::CompareTypeColumn: return "Тип сравнения";
	case Columns::ThresholdColumn: return "Порог";
	case Columns::HysteresisColumn: return "Гистерезис";
	case Columns::ResponseTimeColumn: return "Время срабатывания (с)";
	}

	return QString();
}

std::vector<QVariantList> ComparisonModel::getItems() const
{
	return m_items;
}
