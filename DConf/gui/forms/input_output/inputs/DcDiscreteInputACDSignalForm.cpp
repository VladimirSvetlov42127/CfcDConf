 #include "DcDiscreteInputACDSignalForm.h"

#include <db/dc_db_manager.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

using namespace Dpc::Gui;

namespace {
	enum Columns {
		NumberColumn = 0,
		NameColumn,
		OscillColumn,
		JournalColumn,
		ArhciveColumn,
		DrebezgColumn,
		TimerColumn,
		InterferenceColumn,
		InversionColumn,
		ThresholdChainColumn,
		Threshold0Column,
		Threshold1Column,
		ThresholdKZColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Название";
		case Columns::OscillColumn: return "Осциллографирование";
		case Columns::JournalColumn: return "Журнал";
		case Columns::ArhciveColumn: return "Архив";
		case Columns::DrebezgColumn: return "Постоянная обработки дребезга (мс)";
		case Columns::TimerColumn: return "Сторожевой таймер дребезга (мс)";
		case Columns::InterferenceColumn: return "Параметр отстройки от помех (мс)";
		case Columns::InversionColumn: return "Инверсия";
		case Columns::ThresholdChainColumn: return "Порог обравы цепи";
		case Columns::Threshold0Column: return "Порог лог. 0-ля";
		case Columns::Threshold1Column: return "Порог лог. 1-цы";
		case Columns::ThresholdKZColumn: return "Порог КЗ";
		}

		return QString();
	}

	struct Item {
		DcSignal *signal;
		QVariantList data;
	};

	std::vector<Item> getItems(DcController *contr)
	{
		int physicalDiscretCount = 0;
		int virtualDiscretCount = 0;
		std::vector<Item> result;
		for (auto signal: contr->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_LED_AD)) {
			int baseIdx = signal->internalId() * PROFILE_SIZE;
			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getBitValue(SP_DIN_DINOSCMASK, signal->internalId()));
			d.data.append(contr->getBitValue(SP_TREND_INITIALISE, signal->internalId()));
			d.data.append(contr->getBitValue(SP_DIN_CASH_REQMASK, signal->internalId()));
			d.data.append(contr->getValue(SP_DIN_TABLEPARS, baseIdx + 0).toUInt());
			d.data.append(contr->getValue(SP_DIN_TABLEPARS, baseIdx + 1).toUInt());
			d.data.append(contr->getValue(SP_DIN_TABLEPARS, baseIdx + 2).toUInt());
			QVariant inversionState = contr->getValue(SP_DIN_INVERS, signal->internalId());
			if (inversionState.toBool())
				inversionState = Qt::Checked;
			d.data.append(inversionState);
			d.data.append(contr->getValue(SP_DIN_BREAK, baseIdx + 0).toDouble());
			d.data.append(contr->getValue(SP_DIN_BREAK, baseIdx + 1).toDouble());
			d.data.append(contr->getValue(SP_DIN_BREAK, baseIdx + 2).toDouble());
			d.data.append(contr->getValue(SP_DIN_BREAK, baseIdx + 3).toDouble());
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
			if (index.column() < 2)
				return fl;

			if (index.data().isValid())
				fl |= Qt::ItemIsEditable;

			QVariant val = index.data(Qt::CheckStateRole);
			if (val == Qt::Checked || val == Qt::Unchecked)
				fl |= Qt::ItemIsUserCheckable;

			return fl;
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
			if (!index.isValid())
				return QVariant();

			Item d = m_items.at(index.row());
			int col = index.column();
			if (col == Columns::NameColumn || col == Columns::DrebezgColumn || col == Columns::ThresholdChainColumn ||
				col == Columns::NumberColumn || col == Columns::TimerColumn || col == Columns::InterferenceColumn ||
				col == Columns::Threshold0Column || col == Columns::Threshold1Column || col == Columns::ThresholdKZColumn) {
				if (Qt::DisplayRole == role || Qt::EditRole == role)
					return d.data.value(col);

				if (Qt::TextAlignmentRole == role)
					return Qt::AlignCenter;
			}

			if (col == Columns::OscillColumn || col == Columns::JournalColumn || col == Columns::ArhciveColumn ||
				col == Columns::InversionColumn) {
				if (Qt::CheckStateRole == role)
					return d.data.value(col);
			}

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

			Item &d = m_items[index.row()];
			int col = index.column();
			if (col == Columns::DrebezgColumn || col == Columns::TimerColumn || col == Columns::InterferenceColumn ||
				col == Columns::ThresholdChainColumn || col == Columns::Threshold0Column || col == Columns::Threshold1Column || col == Columns::ThresholdKZColumn) {				
				d.data[col] = value;
				int baseIdx = d.signal->internalId() * PROFILE_SIZE;

				if (col == Columns::DrebezgColumn)
					m_contr->setValue(SP_DIN_TABLEPARS, baseIdx + 0, value);
				if (col == Columns::TimerColumn)
					m_contr->setValue(SP_DIN_TABLEPARS, baseIdx + 1, value);
				if (col == Columns::InterferenceColumn)
					m_contr->setValue(SP_DIN_TABLEPARS, baseIdx + 2, value);
				if (col == Columns::ThresholdChainColumn)
					m_contr->setValue(SP_DIN_BREAK, baseIdx + 0, value);
				if (col == Columns::Threshold0Column)
					m_contr->setValue(SP_DIN_BREAK, baseIdx + 1, value);
				if (col == Columns::Threshold1Column)
					m_contr->setValue(SP_DIN_BREAK, baseIdx + 2, value);
				if (col == Columns::ThresholdKZColumn)
					m_contr->setValue(SP_DIN_BREAK, baseIdx + 3, value);


				emit dataChanged(index, index);
				return true;
			}

			if (col == Columns::OscillColumn || col == Columns::JournalColumn || col == Columns::ArhciveColumn ||
				col == Columns::InversionColumn) {
				d.data[col] = value.toBool() ? Qt::Checked : Qt::Unchecked;

				int val = value.toBool() ? 1 : 0;
				if (col == Columns::InversionColumn)
					m_contr->setValue(SP_DIN_INVERS, d.signal->internalId(), val);
				if (col == Columns::OscillColumn)
					m_contr->setBitValue(SP_DIN_DINOSCMASK, d.signal->internalId(), val);
				if (col == Columns::JournalColumn)
					m_contr->setBitValue(SP_TREND_INITIALISE, d.signal->internalId(), val);
				if (col == Columns::ArhciveColumn)
					m_contr->setBitValue(SP_DIN_CASH_REQMASK, d.signal->internalId(), val);

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

DcDiscreteInputACDSignalForm::DcDiscreteInputACDSignalForm(DcController *controller) :
	DcForm(controller,  "Входы АЦП", false)
{
	auto tableView = new TableView(new TableModel(controller, this), this);
	tableView->verticalHeader()->hide();
	tableView->setItemDelegateForColumn(Columns::DrebezgColumn, new IntSpinBoxDelegate(0, UCHAR_MAX, this));
	tableView->setItemDelegateForColumn(Columns::TimerColumn, new IntSpinBoxDelegate(0, UCHAR_MAX, this));
	tableView->setItemDelegateForColumn(Columns::InterferenceColumn, new IntSpinBoxDelegate(0, UCHAR_MAX, this));
    tableView->setItemDelegateForColumn(Columns::ThresholdChainColumn, new DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(Columns::Threshold0Column, new DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(Columns::Threshold1Column, new DoubleSpinBoxDelegate(0, 10000, this));
    tableView->setItemDelegateForColumn(Columns::ThresholdKZColumn, new DoubleSpinBoxDelegate(0, 10000, this));

	tableView->horizontalHeader()->setStretchLastSection(true);
	tableView->horizontalHeader()->resizeSection(Columns::OscillColumn, 140);
	tableView->horizontalHeader()->resizeSection(Columns::JournalColumn, 80);
	tableView->horizontalHeader()->resizeSection(Columns::ArhciveColumn, 80);
	tableView->horizontalHeader()->resizeSection(Columns::DrebezgColumn, 200);	
	tableView->horizontalHeader()->resizeSection(Columns::TimerColumn, 200);
	tableView->horizontalHeader()->resizeSection(Columns::InterferenceColumn, 200);
	tableView->horizontalHeader()->resizeSection(Columns::InversionColumn, 90);
	tableView->horizontalHeader()->resizeSection(Columns::ThresholdChainColumn, 120);
	tableView->horizontalHeader()->resizeSection(Columns::Threshold0Column, 100);
	tableView->horizontalHeader()->resizeSection(Columns::Threshold1Column, 100);
	tableView->horizontalHeader()->resizeSection(Columns::ThresholdKZColumn, 80);

    connect(tableView, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DcDiscreteInputACDSignalForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DIN_TABLEPARS},
		{SP_DIN_BREAK}
	};

	return hasAny(controller, params);
}

void DcDiscreteInputACDSignalForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	DcReportTable table(report->device(), headers, { 5, 25 });
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.data.size(); col++) {
			if (col == Columns::OscillColumn || col == Columns::JournalColumn || col == Columns::ArhciveColumn ||
				col == Columns::InversionColumn)
				values << DcReportTable::checkedSign(it.data[col].toBool());
			else
				values << it.data[col].toString();
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
