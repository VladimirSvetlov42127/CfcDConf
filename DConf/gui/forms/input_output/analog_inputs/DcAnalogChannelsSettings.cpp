#include "DcAnalogChannelsSettings.h"

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

using namespace Dpc::Gui;

namespace {
	enum Columns {
		NumberColumn = 0,
		NameColumn,		
		ApertureTypeColumn,
		ApertureValColumn,
		ThresholdSensColumn,
		Threshold0Column,
		Threshold1Column,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Название";		
		case Columns::ApertureTypeColumn: return "Тип апертуры";
		case Columns::ApertureValColumn: return "Значение апертуры";
		case Columns::ThresholdSensColumn: return "Порог чувствительности";
		case Columns::Threshold0Column: return "Порог логического 0-ля";
		case Columns::Threshold1Column: return "Порог логической 1-цы";
		}

		return QString();
	}

	ComboBoxDelegate g_ApertureTypeDelegate = { "Абсолютный", "Относительный" };

	struct Item {
		DcSignal *signal;
		QVariantList data;
	};

	std::vector<Item> getItems(DcController *contr)
	{
		std::vector<Item> result;
		for (auto signal: contr->getSignalList(DEF_SIG_TYPE_ANALOG)) {

			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getValue(SP_AIN_APERTURE_TYPE, signal->internalId()).toUInt());
			d.data.append(contr->getValue(SP_AIN_APERTURE_VAL, signal->internalId()).toDouble());
			d.data.append(contr->getValue(SP_AIN_SENSIBILITY, signal->internalId()).toDouble());

			if (DcController::GSM == contr->type()) {
				auto index = signal->internalId() - 4;
				if (index >= 0) {
					auto val = contr->getValue(SP_DIN_TO_LOW, index);
					d.data.append(!val.isNull() ? val.toDouble() : QVariant());

					val = contr->getValue(SP_DIN_TO_HIGH, index);
					d.data.append(!val.isNull() ? val.toDouble() : QVariant());
				}
			}	

			if (DcController::LT == contr->type()) {
				auto idx = signal->internalId() - 3;
				if (idx >= 0 && idx < 3) {
					auto val = contr->getValue(SP_AIN_PHYLVL1, idx * 2 + 1);
					d.data.append(!val.isNull() ? val.toDouble() : QVariant());

					val = contr->getValue(SP_AIN_PHYLVL1, idx * 2);
					d.data.append(!val.isNull() ? val.toDouble() : QVariant());
				}
			}

			result.push_back(d);
		}

		return result;
	}

	class TableModel : public QAbstractTableModel
	{
	public:
		TableModel(DcController *contr, QObject *parent = nullptr) : QAbstractTableModel(parent), m_contr(contr) {
			m_items = getItems(contr);
		}

		virtual ~TableModel() {}

		int rowCount(const QModelIndex &parent = QModelIndex()) const {
			if (parent.isValid())
				return 0;

			return m_items.size();
		}

		int columnCount(const QModelIndex &parent = QModelIndex()) const {
			if (parent.isValid())
				return 0;

			return ColumnsCount;
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return ::columnName((Columns)section);

			return QVariant();
		}

		Qt::ItemFlags flags(const QModelIndex &index) const {
			Qt::ItemFlags fl = QAbstractTableModel::flags(index);
            if (index.column() < 1)
				return fl;

            if (index.column() == Columns::NameColumn) {
                auto subType = m_items[index.row()].signal->subtype();
                if (subType == DEF_SIG_SUBTYPE_VIRTUAL || subType == DEF_SIG_SUBTYPE_REMOTE)
                    return fl | Qt::ItemIsEditable;
            }
            else if (index.data().isValid()) {
                fl |= Qt::ItemIsEditable;
            }

			QVariant val = index.data(Qt::CheckStateRole);
            if (val == Qt::Checked || val == Qt::Unchecked) {
                fl |= Qt::ItemIsUserCheckable;
            }

			return fl;
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
			if (!index.isValid())
				return QVariant();

			Item d = m_items.at(index.row());
			int col = index.column();
			if (col == Columns::NameColumn || col == Columns::NumberColumn || col == Columns::ApertureTypeColumn ||
				col == Columns::ApertureValColumn || col == Columns::ThresholdSensColumn || col == Columns::Threshold0Column ||
				col == Columns::Threshold1Column) {
				if (Qt::DisplayRole == role || Qt::EditRole == role)
					return d.data.value(col);

				if (Qt::TextAlignmentRole == role)
					return Qt::AlignCenter;
			}

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

            Item &d = m_items[index.row()];
			int col = index.column();

            if (Columns::NameColumn == col) {
                QString newName = value.toString().trimmed();
                if (newName.isEmpty())
                    return false;

                d.data[col] = newName;
                d.signal->updateName(newName);
            }

            if (col == Columns::ApertureTypeColumn || col == Columns::ApertureValColumn || col == Columns::ThresholdSensColumn ||
				col == Columns::Threshold0Column ||	col == Columns::Threshold1Column) {
				d.data[col] = value;

                if (col == Columns::ApertureTypeColumn)
                    m_contr->setValue(SP_AIN_APERTURE_TYPE, d.signal->internalId(), value);
                if (col == Columns::ApertureValColumn)
                    m_contr->setValue(SP_AIN_APERTURE_VAL, d.signal->internalId(), value);
                if (col == Columns::ThresholdSensColumn)
                    m_contr->setValue(SP_AIN_SENSIBILITY, d.signal->internalId(), value);

                if (DcController::GSM == m_contr->type()) {
                    auto idx = d.signal->internalId() - 4;
                    if (idx >= 0) {
                        if (col == Columns::Threshold0Column)
                            m_contr->setValue(SP_DIN_TO_LOW, idx, value);
                        if (col == Columns::Threshold1Column)
                            m_contr->setValue(SP_DIN_TO_HIGH, idx, value);
                    }
                }

                if (DcController::LT == m_contr->type()) {
                    auto idx = d.signal->internalId() - 3;
                    if (idx >= 0 && idx < 3) {
                        if (col == Columns::Threshold0Column)
                            m_contr->setValue(SP_AIN_PHYLVL1, idx * 2 + 1, value);
                        if (col == Columns::Threshold1Column)
                            m_contr->setValue(SP_AIN_PHYLVL1, idx * 2, value);
                    }
                }

				emit dataChanged(index, index);
				return true;
			}

			return false;
		}

	private:
		DcController *m_contr;
		std::vector<Item> m_items;
	};
}

DcAnalogChannelsSettings::DcAnalogChannelsSettings(DcController *controller) :
	DcForm(controller,  "Апертуры и пороги аналоговых входов", false)
{
	auto tableView = new TableView(new TableModel(controller, this), this);
	tableView->setItemDelegateForColumn(Columns::ApertureTypeColumn, &g_ApertureTypeDelegate);
    tableView->setItemDelegateForColumn(Columns::ApertureValColumn, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(Columns::ThresholdSensColumn, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(Columns::Threshold0Column, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));
    tableView->setItemDelegateForColumn(Columns::Threshold1Column, new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), this));

	tableView->horizontalHeader()->resizeSection(Columns::ApertureTypeColumn, 120);
	tableView->horizontalHeader()->resizeSection(Columns::ApertureValColumn, 120);
	tableView->horizontalHeader()->resizeSection(Columns::ThresholdSensColumn, 140);
	tableView->horizontalHeader()->resizeSection(Columns::Threshold0Column, 140);
	tableView->horizontalHeader()->resizeSection(Columns::Threshold1Column, 140);

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

DcAnalogChannelsSettings::~DcAnalogChannelsSettings()
{
}

bool DcAnalogChannelsSettings::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_AIN_APERTURE_VAL},
		{SP_AIN_APERTURE_TYPE},
		{SP_AIN_SENSIBILITY},
		{SP_DIN_TO_LOW},
		{SP_DIN_TO_HIGH}
	};

	return hasAny(controller, params);
}

void DcAnalogChannelsSettings::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	DcReportTable table(report->device(), headers, { 5, 30 });
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.data.size(); col++) {
			if (Columns::ApertureTypeColumn == col)
				values << g_ApertureTypeDelegate.textFor(it.data[col].toUInt()).toString();
			else
				values << it.data[col].toString();
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
