#include "DcIEC103ChannelsForm.h"

#include <db/dc_db_manager.h>

#include <dpc/gui/widgets/TableView.h>

using namespace Dpc::Gui;

namespace {

	enum Columns {
		NumberColumn = 0,
		NameColumn,
		PollingColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Название";
		case Columns::PollingColumn: return "Опрос";
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
		for (auto signal : contr->getSignalList(DEF_SIG_TYPE_DISCRETE)) {
			if (signal->subtype() == DEF_SIG_SUBTYPE_LED_AD)
				continue;

			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getBitValue(SP_IEC_103_MASK_DINS, signal->internalId()));
			result.push_back(d);
		}

		return result;
	}

	class TableModel : public QAbstractTableModel
	{
	public:
		TableModel(DcController *contr, QObject *parent = nullptr) : QAbstractTableModel(parent), m_contr(contr)
		{
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
			if (col == Columns::NameColumn || col == Columns::NumberColumn) {
				if (Qt::DisplayRole == role || Qt::EditRole == role)
					return d.data.value(col);

				if (Qt::TextAlignmentRole == role)
					return Qt::AlignCenter;
			}

			if (col == Columns::PollingColumn) {
				if (Qt::CheckStateRole == role)
					return d.data.value(col);
			}

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

			Item &d = m_items[index.row()];
			int col = index.column();
			if (col == Columns::PollingColumn) {
				d.data[col] = value.toBool() ? Qt::Checked : Qt::Unchecked;

				int val = value.toBool() ? 1 : 0;
				m_contr->setBitValue(SP_IEC_103_MASK_DINS, d.signal->internalId(), val);

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

DcIEC103ChannelsForm::DcIEC103ChannelsForm(DcController *controller) :
	DcForm(controller,  "Поканальные настройки МЭК 60870-5-103", false)
{
	auto tableView = new TableView(new TableModel(controller, this), this);
	tableView->horizontalHeader()->resizeSection(Columns::PollingColumn, 80);

    connect(tableView, &TableView::itemsCheckStateAboutToChange, controller, &DcController::beginTransaction);
    connect(tableView, &TableView::itemsCheckStateChanged, controller, &DcController::endTransaction);

	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());
	QTabWidget *tabWidget = new QTabWidget;
	mainLayout->addWidget(tabWidget);

	QWidget *discretTab = new QWidget;
	QVBoxLayout *discretTabLayout = new QVBoxLayout(discretTab);
	discretTabLayout->addWidget(tableView);

	tabWidget->addTab(discretTab, "Дискретные сигналы");
}

bool DcIEC103ChannelsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_IEC_103_MASK_DINS}
	};

	return hasAny(controller, params);
}

void DcIEC103ChannelsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	DcReportTable table(report->device(), headers, { 5 });
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.data.size(); col++) {
			if (col == Columns::PollingColumn)
				values << DcReportTable::checkedSign(it.data[col].toBool());
			else
				values << it.data[col].toString();
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
