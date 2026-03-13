#include "DcDiscreteOutputChannelForm.h"

#include <dpc/gui/widgets/TableView.h>

using namespace Dpc::Gui;

namespace {
	enum Columns {
		NumberColumn = 0,
		NameColumn,
		ImpulseColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumberColumn: return "№";
		case Columns::NameColumn: return "Название";
		case Columns::ImpulseColumn: return "Длительность импульсов управления";
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
		for (auto signal: contr->getSignalList(DEF_SIG_TYPE_DISCRETE, DEF_SIG_SUBTYPE_UNDEF, DEF_SIG_DIRECTION_OUTPUT)) {
			Item d = { signal, QVariantList() };
			d.data.append(signal->internalId());
			d.data.append(signal->name());
			d.data.append(contr->getValue(SP_DOUT_SAMPLTIM, signal->internalId()).toUInt());
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
			if (col == Columns::NameColumn || col == Columns::NumberColumn || col == Columns::ImpulseColumn) {
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
            if (col == Columns::ImpulseColumn ) {
				d.data[col] = value;

				if (col == Columns::ImpulseColumn)
					m_contr->setValue(SP_DOUT_SAMPLTIM, d.signal->internalId(), value);

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

DcDiscreteOutputChannelForm::DcDiscreteOutputChannelForm(DcController *controller) :
	DcForm(controller,  "Поканальные настройки дискретных выходов", false)
{
	auto tableView = new TableView(new TableModel(controller, this), this);
	tableView->horizontalHeader()->resizeSection(Columns::ImpulseColumn, 200);

	auto layout = new QHBoxLayout(centralWidget());
	layout->addWidget(tableView);
}

bool DcDiscreteOutputChannelForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_DOUT_SAMPLTIM}
	};

	return hasAny(controller, params);
}

void DcDiscreteOutputChannelForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();

	QStringList headers;
	for (size_t i = 0; i < ColumnsCount; i++)
		headers << columnName(Columns(i));

	DcReportTable table(report->device(), headers, { 5 });
	for (auto &&it : getItems(report->device())) {
		QStringList values;
		for (size_t col = 0; col < it.data.size(); col++) {
			values << it.data[col].toString();
		}

		table.addRow(values);
	}

	report->insertTable(table);
}
