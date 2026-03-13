#include "DcDiscreteOutputClonesForm.h"

#include "data_model/dc_controller.h"

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

using namespace Dpc::Gui;

namespace {
	const uint32_t NotUseValue = 0xFF;
	const QString  NotUseText = "Не дублируется";

	enum Columns {
		NumberColumn = 0,
		NameColumn,
		CloneColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Канал";
		case Columns::CloneColumn: return "Клон";
		}

		return QString();
	}

	struct Item {
		DcSignal *signal;
		QVariantList data;
	};

	std::vector<Item> getItems(DcController *contr)
	{
		std::vector<Item> result;
		for (auto signal: contr->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS, DEF_SIG_DIRECTION_OUTPUT)) {
			if (!signal->isCloningEnabled())
				continue;

			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getValue(SP_DOUT_CLONE, signal->internalId()).toUInt());
			result.push_back(d);
		}

		return result;
	}

	ComboBoxDelegate* g_SignalsDelegate(DcController *device, QObject *parent = nullptr)
	{
		auto delegate = new ComboBoxDelegate(parent);
		delegate->append(NotUseText, NotUseValue);
		for (auto signal : device->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_PHIS, DEF_SIG_DIRECTION_OUTPUT)) {
			if (!signal->isCloningEnabled())
				continue;
			
			delegate->append(signal->name(), signal->internalId());
		}

		return delegate;
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
			if (col == Columns::NameColumn || col == Columns::NumberColumn || col == Columns::CloneColumn) {
				if (Qt::TextAlignmentRole == role)
					return Qt::AlignCenter;

				if (Qt::DisplayRole == role || Qt::EditRole == role) {
					if (col == Columns::CloneColumn) {
						auto signalId = d.signal->internalId();
						auto val = d.data.value(col).toUInt();
						return val == signalId ? NotUseValue : val;
					}
					else
						return d.data.value(col);
				}				
			}

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole)
		{
			if (!index.isValid())
				return false;

			Item &d = m_items[index.row()];
			int col = index.column();
			if (col == Columns::CloneColumn) {
				auto oldVal = d.data[col];
				auto newVal = value.toUInt() == NotUseValue ? d.signal->internalId() : value;
				if (oldVal == newVal)
					return true;

				d.data[col] = newVal;
				if (col == Columns::CloneColumn)
					m_contr->setValue(SP_DOUT_CLONE, d.signal->internalId(), newVal);

				emit dataChanged(index, index);

				if (oldVal != d.signal->internalId())
					setData(indexOf(oldVal.toUInt(), Columns::CloneColumn), NotUseValue);

				if (newVal != d.signal->internalId())
					setData(indexOf(newVal.toUInt(), Columns::CloneColumn), d.signal->internalId());

				return true;
			}

			return false;
		}

		QModelIndex indexOf(int32_t signalInternalId, int column)
		{
			for (size_t i = 0; i < m_items.size(); i++)
				if (m_items[i].signal->internalId() == signalInternalId)
					return this->index(i, column);

			return QModelIndex();
		}

	private:
		DcController *m_contr;
		std::vector<Item> m_items;
	};
}


DcDiscreteOutputClonesForm::DcDiscreteOutputClonesForm(DcController *controller) :
	DcForm(controller,  "Дублирование физических выходов", false)
{
	auto tableView = new TableView(new ::TableModel(controller, this), this);
	tableView->horizontalHeader()->resizeSection(Columns::CloneColumn, 200);
	tableView->setItemDelegateForColumn(Columns::CloneColumn, g_SignalsDelegate(controller, this));

	auto layout = new QHBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DcDiscreteOutputClonesForm::isAvailableFor(DcController * controller)
{
    auto param = controller->paramsRegistry().parameter(SP_DOUT_CLONE);
    return param && param->profilesCount() == 1;
}

void DcDiscreteOutputClonesForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	auto signalsDelegate = std::shared_ptr<ComboBoxDelegate>(g_SignalsDelegate(report->device()));

	DcReportTable table(report->device(), headers, { 5 });
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.data.size(); col++) {
			if (col == Columns::CloneColumn) {
				auto value = it.data[col].toUInt();
				values << signalsDelegate->textFor(value == it.signal->internalId() ? NotUseValue : value).toString();
			}
			else
				values << it.data[col].toString();
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
