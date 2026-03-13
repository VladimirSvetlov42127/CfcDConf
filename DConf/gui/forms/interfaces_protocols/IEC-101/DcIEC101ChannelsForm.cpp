#include "DcIEC101ChannelsForm.h"

#include <db/dc_db_manager.h>

#include <dpc/gui/widgets/TableView.h>

using namespace Dpc::Gui;

namespace {
	enum Columns {
		NumberColumn = 0,
		NameColumn,
		PollingColumn,
		Group1Column,
		Group2Column,
		BackgroundColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Название";
		case Columns::PollingColumn: return "Опрос";
		case Columns::Group1Column: return "Группа 1";
		case Columns::Group2Column: return "Группа 2";
		case Columns::BackgroundColumn: return "Циклическая передача";
		}

		return QString();
	}

	struct Item {
		DcSignal *signal;
		QVariantList data;
	};

	std::vector<Item> getItems(DcController *contr, DefSignalType signalType)
	{
		int profile = signalType - 1;
		std::vector<Item> result;
		for (auto signal: contr->getSignalList(signalType)) {
			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getBitValue(SP_SELECT_MASK, signal->internalId(), profile));
			d.data.append(contr->getBitValue(SP_GROUP1LIST, signal->internalId(), profile));
			d.data.append(contr->getBitValue(SP_GROUP2LIST, signal->internalId(), profile));
			d.data.append(contr->getBitValue(SP_BACKGROUNDLIST, signal->internalId(), profile));
			result.push_back(d);
		}
		return result;
	}

	class TableModel : public QAbstractTableModel
	{
	public:
		TableModel(DcController *contr, DefSignalType signalType, QObject *parent = nullptr) :
			QAbstractTableModel(parent),
			m_contr(contr),
			m_profile(signalType - 1)
		{
			m_items = getItems(contr, signalType);
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

			if (col == Columns::PollingColumn || col == Columns::Group1Column || col == Columns::Group2Column || col == Columns::BackgroundColumn) {
				if (Qt::CheckStateRole == role)
					return d.data.value(col);
			}

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) {

			Item &d = m_items[index.row()];
			int col = index.column();
			if (col == Columns::PollingColumn || col == Columns::Group1Column || col == Columns::Group2Column || col == Columns::BackgroundColumn) {
				d.data[col] = value.toBool() ? Qt::Checked : Qt::Unchecked;
				
				int32_t addr = 0;
				switch (col) {
				case Columns::PollingColumn: addr = SP_SELECT_MASK; break;
				case Columns::Group1Column: addr = SP_GROUP1LIST; break;
				case Columns::Group2Column: addr = SP_GROUP2LIST; break;
				case Columns::BackgroundColumn: addr = SP_BACKGROUNDLIST; break;
				default: break;
				}

				m_contr->setBitValue(addr, d.signal->internalId(), value, m_profile);
				emit dataChanged(index, index);
				return true;
			}

			return false;
		}

	private:
		DcController *m_contr;
		std::vector<Item> m_items;
		int m_profile;
	};
}

namespace Text {
	const QMap< DefSignalType, QString> Titles = {
		{DEF_SIG_TYPE_DISCRETE, "Дискреты"},
		{DEF_SIG_TYPE_ANALOG, "Аналоги"},
		{DEF_SIG_TYPE_COUNTER, "Счётчики"}
	};
}

DcIEC101ChannelsForm::DcIEC101ChannelsForm(DcController *controller) :
	DcForm(controller,  "Поканальные настройки МЭК 60870-5-101/104", false)
{
	auto vbox = new QVBoxLayout(centralWidget());
	QTabWidget *tabWidget = new QTabWidget();
	vbox->addWidget(tabWidget);
	
	tabWidget->addTab(createChannelsTable(DEF_SIG_TYPE_DISCRETE), Text::Titles.value(DEF_SIG_TYPE_DISCRETE));
	tabWidget->addTab(createChannelsTable(DEF_SIG_TYPE_ANALOG), Text::Titles.value(DEF_SIG_TYPE_ANALOG));
	tabWidget->addTab(createChannelsTable(DEF_SIG_TYPE_COUNTER), Text::Titles.value(DEF_SIG_TYPE_COUNTER));
}

DcIEC101ChannelsForm::~DcIEC101ChannelsForm()
{
}

bool DcIEC101ChannelsForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_SELECT_MASK},
		{SP_GROUP1LIST},
		{SP_GROUP2LIST},
		{SP_BACKGROUNDLIST}
	};

	return hasAny(controller, params);
}

void DcIEC101ChannelsForm::fillReport(DcIConfigReport * report)
{
	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	for (auto it = Text::Titles.begin(); it != Text::Titles.end(); it++) {
		report->insertSection(it.value());

		DcReportTable table(report->device(), headers, { 5, 20 });
		for (auto &&it : getItems(report->device(), it.key())) {
			QStringList values;
			for (size_t col = 0; col < it.data.size(); col++) {
				if (col == Columns::PollingColumn || col == Columns::Group1Column || col == Columns::Group2Column || col == Columns::BackgroundColumn)
					values << DcReportTable::checkedSign(it.data[col].toBool());
				else
					values << it.data[col].toString();
			}

			table.addRow(values);
		}

		report->insertTable(table);
	}
}

QWidget * DcIEC101ChannelsForm::createChannelsTable(DefSignalType signalType)
{
	auto tableView = new TableView;
	tableView->setModel(new TableModel(controller(), signalType, tableView));
	tableView->horizontalHeader()->resizeSection(Columns::PollingColumn, 80);
	tableView->horizontalHeader()->resizeSection(Columns::Group1Column, 90);
	tableView->horizontalHeader()->resizeSection(Columns::Group2Column, 90);
	tableView->horizontalHeader()->resizeSection(Columns::BackgroundColumn, 160);
	
    connect(tableView, &TableView::itemsCheckStateAboutToChange, controller(), &DcController::beginTransaction);
    connect(tableView, &TableView::itemsCheckStateChanged, controller(), &DcController::endTransaction);

	QWidget *widget = new QWidget;
	QVBoxLayout *widgetLayout = new QVBoxLayout(widget);
	widgetLayout->addWidget(tableView);
	return widget;
}
