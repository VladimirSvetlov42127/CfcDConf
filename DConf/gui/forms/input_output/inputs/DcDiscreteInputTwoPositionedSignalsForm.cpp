#include "DcDiscreteInputTwoPositionedSignalsForm.h"

#include <db/dc_db_manager.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

using namespace Dpc::Gui;

namespace {
	const uint32_t VALUE_NOTUSE = 0xFFFF;

	enum DpAlgBits {
		BitActive = 0,
		BitState1, 
		BitInversion, 
		BitState2,
		BitNotGenerateUntrue
	};

	enum States {
		Unknown,
		State2, 
		State3,
		State4
	};

	enum Columns {
		NameColumn = 0,
		RPOColumn,
		RPVColumn,
		IntermediateTimeColumn,
		IntermediateTimeRatioColumn,
		NotGenerateUntrue,
		ActiveColumn,
		InversionColumn,
		StateColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NameColumn: return "Название сигнала";
		case Columns::RPOColumn: return "Номер канала,\n замкнутогов состоянии\n ОТКЛ (РПО)";
		case Columns::RPVColumn: return "Номер канала,\n замкнутого в состоянии\n ВКЛ (РПВ)";
		case Columns::IntermediateTimeColumn: return "Время в\n промежуточном\nсостоянии, мс";
		case Columns::NotGenerateUntrue: return "Не формировать\n недостоверность при\n превышении времени";
		case Columns::IntermediateTimeRatioColumn: return "Масштабный коэффициент\n времени в\n промежуточном состоянии";
		case Columns::ActiveColumn: return "Активен";
		case Columns::StateColumn: return "Состояния";
		case Columns::InversionColumn: return "Инверсия состояния";
		}

		return QString();
	}

	std::vector<QVariantList> getItems(DcController *contr)
	{
		std::vector<QVariantList> items;
        auto param = contr->paramsRegistry().parameter(SP_DIN_DP_COUPLES);
		if (!param)
			return items;

        for (size_t i = 0; i < param->profilesCount(); i++) {
			uint idxBase = i * PROFILE_SIZE;
			QVariantList data;
			data.append(QString("Двухпозиционный сигнал %1").arg(i + 1));
			data.append(contr->getValue(SP_DIN_DP_COUPLES, idxBase + 0).toUInt());
			data.append(contr->getValue(SP_DIN_DP_COUPLES, idxBase + 1).toUInt());
			data.append(contr->getValue(SP_DIN_DP_INTERMID_TIME, i).toUInt());
			data.append(contr->getValue(SP_DIN_DP_INTERMID_TIME_COEF, i).toUInt());

			data.append(contr->getBitValue(SP_DIN_DP_ALG, BitNotGenerateUntrue, 0, i));
			data.append(contr->getBitValue(SP_DIN_DP_ALG, BitActive, 0, i));
			data.append(contr->getBitValue(SP_DIN_DP_ALG, BitInversion, 0, i));
			auto bit1 = contr->getBitValue(SP_DIN_DP_ALG, BitState1, 0, i);
			auto bit2 = contr->getBitValue(SP_DIN_DP_ALG, BitState2, 0, i);
			States st = Unknown;
			if (bit1.toBool() && !bit2.toBool())
				st = State2;
			else if (!bit1.toBool() && bit2.toBool())
				st = State3;
			else if (!bit1.toBool() && !bit2.toBool())
				st = State4;
			data.append(static_cast<uint>(st));

			items.push_back(data);
		}

		return items;
	}

	ComboBoxDelegate g_StateDelegate = { {"2 Состояния", State2}, {"3 Состояния", State3}, {"4 Состояния", State4} };

	ComboBoxDelegate* g_ChannelsDelegate(DcController *contr, QObject *parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , VALUE_NOTUSE });
		for (auto signal: contr->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}

	class TableModel : public QAbstractTableModel
	{
	public:
		TableModel(DcController *contr, QObject *parent = nullptr) :
			QAbstractTableModel(parent),
			m_contr(contr)
		{
			m_items = getItems(contr);
		}

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
			if (index.column() < 1)	return fl;

			if (index.data().isValid())	fl |= Qt::ItemIsEditable;

			QVariant val = index.data(Qt::CheckStateRole);
			if (val == Qt::Checked || val == Qt::Unchecked)	fl |= Qt::ItemIsUserCheckable;

			return fl;
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const {
			if (!index.isValid())
				return QVariant();

			auto data = m_items.at(index.row());
			int col = index.column();

			if (Qt::EditRole == role) return data.value(col);

			if (Qt::TextAlignmentRole == role)
				return Qt::AlignCenter;

			if (Qt::CheckStateRole == role) {
				if (Columns::ActiveColumn == col || Columns::InversionColumn == col || Columns::NotGenerateUntrue == col)
					return data.value(col);
			}

			if (Qt::DisplayRole == role) {
				if (col == Columns::NameColumn || col == Columns::IntermediateTimeColumn || col == Columns::IntermediateTimeRatioColumn ||
					Columns::RPOColumn == col || Columns::RPVColumn == col || Columns::StateColumn == col)
					return data.value(col);
			}					

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {
			int row = index.row();
			int col = index.column();			
			auto &data = m_items[row];

			if (col == Columns::RPOColumn || col == Columns::RPVColumn || col == Columns::IntermediateTimeColumn ||
				col == Columns::IntermediateTimeRatioColumn || col == Columns::StateColumn) {
				data[col] = value;

				if (col == Columns::RPOColumn)
					m_contr->setValue(SP_DIN_DP_COUPLES, row * PROFILE_SIZE + 0, value);
				if (col == Columns::RPVColumn)
					m_contr->setValue(SP_DIN_DP_COUPLES, row * PROFILE_SIZE + 1, value);
				if (col == Columns::IntermediateTimeColumn)
					m_contr->setValue(SP_DIN_DP_INTERMID_TIME, row, value);
				if (col == Columns::IntermediateTimeRatioColumn)
					m_contr->setValue(SP_DIN_DP_INTERMID_TIME_COEF, row, value);
				if (col == Columns::StateColumn) {
					QVariant bit1Value = false;
					QVariant bit2Value = false;
					switch (static_cast<States>(value.toUInt()))
					{
					case State2: bit1Value = true; break;
					case State3: bit2Value = true; break;
					default: break;
					}

					m_contr->setBitValue(SP_DIN_DP_ALG, BitState1, bit1Value, 0, row);
					m_contr->setBitValue(SP_DIN_DP_ALG, BitState2, bit2Value, 0, row);
				}

				emit dataChanged(index, index);
				return true;
			}

			if (col == Columns::ActiveColumn || col == Columns::InversionColumn || col == Columns::NotGenerateUntrue) {
				data[col] = value.toBool() ? Qt::Checked : Qt::Unchecked;

				int val = value.toBool() ? 1 : 0;
				if (col == Columns::ActiveColumn) m_contr->setBitValue(SP_DIN_DP_ALG, BitActive, val, 0, row);
				if (col == Columns::InversionColumn) m_contr->setBitValue(SP_DIN_DP_ALG, BitInversion, val, 0, row);
				if (col == Columns::NotGenerateUntrue) m_contr->setBitValue(SP_DIN_DP_ALG, BitNotGenerateUntrue, val, 0, row);

				emit dataChanged(index, index);
				return true;
			}

			return false;
		}		

	private:
		DcController *m_contr;
		std::vector<QVariantList> m_items;	
	};

} // namespace

DcDiscreteInputTwoPositionedSignalsForm::DcDiscreteInputTwoPositionedSignalsForm(DcController *controller) :
	DcForm(controller,  "Двухпозиционные сигналы", false)
{
	auto view = new TableView(new TableModel(controller, this), this);
	view->horizontalHeader()->setMinimumHeight(60);
	view->horizontalHeader()->setStretchLastSection(true);
	view->setColumnWidth(NameColumn, 200);
	view->setColumnWidth(RPOColumn, 200);
	view->setColumnWidth(RPVColumn, 200);
	view->setColumnWidth(IntermediateTimeColumn, 160);
	view->setColumnWidth(NotGenerateUntrue, 220);
	view->setColumnWidth(IntermediateTimeRatioColumn, 220);
	view->setColumnWidth(ActiveColumn, 100);
	view->setColumnWidth(StateColumn, 100);
	view->setColumnWidth(InversionColumn, 150);

	auto rpoDelegate = g_ChannelsDelegate(controller, this);
	auto rpvDelegate = g_ChannelsDelegate(controller, this);
	view->setItemDelegateForColumn(RPOColumn, rpoDelegate);
	view->setItemDelegateForColumn(RPVColumn, rpvDelegate);
	view->setItemDelegateForColumn(IntermediateTimeColumn, new IntSpinBoxDelegate(0, UCHAR_MAX, this));
	view->setItemDelegateForColumn(IntermediateTimeRatioColumn, new IntSpinBoxDelegate(1, UCHAR_MAX, this));
	view->setItemDelegateForColumn(StateColumn, &g_StateDelegate);

    connect(view, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(view, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *layout = new QVBoxLayout(centralWidget());
	layout->addWidget(view);
}

bool DcDiscreteInputTwoPositionedSignalsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DIN_DP_COUPLES}
	};

	return hasAny(controller, params);
}

void DcDiscreteInputTwoPositionedSignalsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	auto channelsDelegate = std::shared_ptr<ComboBoxDelegate>(g_ChannelsDelegate(report->device()));

	DcReportTable table(report->device(), headers, {30});
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.size(); col++) {
			if (Columns::ActiveColumn == col || Columns::InversionColumn == col || Columns::NotGenerateUntrue == col)
				values << DcReportTable::checkedSign(it[col].toBool());
			else {
				if (col == Columns::NameColumn || col == Columns::IntermediateTimeColumn || col == Columns::IntermediateTimeRatioColumn)
					values << it[col].toString();

				if (Columns::RPOColumn == col || Columns::RPVColumn == col)
					values << channelsDelegate->textFor(it[col].toUInt()).toString();

				if (Columns::StateColumn == col)
					values << g_StateDelegate.textFor(it[col].toUInt()).toString();
			}
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
