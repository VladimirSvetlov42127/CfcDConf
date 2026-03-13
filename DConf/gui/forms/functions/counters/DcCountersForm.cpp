#include "DcCountersForm.h"

#include <qtabwidget.h>

#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>

#include <gui/editors/EditorsManager.h>

using namespace Dpc::Gui;

namespace {
	ComboBoxDelegate g_AlgorithmsDelegate = { "Не считать", "Из 0 в 1", "Из 1 в 0", "Все" };

	ComboBoxDelegate* g_DiscretsDelegate(DcController* contr, QObject* parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
		for (auto signal : contr->getSignalList(DEF_SIG_TYPE_DISCRETE)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}

	ComboBoxDelegate* g_CountersDelegate(DcController* contr, QObject* parent = nullptr)
	{
		ComboBoxDelegate* result = new ComboBoxDelegate(parent);
		result->append({ "Не используется" , std::numeric_limits<uint16_t>::max() });
		for (auto signal : contr->getSignalList(DEF_SIG_TYPE_COUNTER, DEF_SIG_SUBTYPE_VIRTUAL)) {
			result->append({ signal->name(), static_cast<uint>(signal->internalId()) });
		}

		return result;
	}

	enum Columns {
		NumColumn = 0,
		DiscretColumn,
		CounterColumn,
		AlgorithmColumn,

		ColumnsCount
	};

	QString columnName(Columns col) {
		switch (col)
		{
		case Columns::NumColumn: return "№";
		case Columns::DiscretColumn: return "Дискрет";
		case Columns::CounterColumn: return "Счётчик";
		case Columns::AlgorithmColumn: return "Алгоритм подсчёта";
		}

		return QString();
	}

	std::vector<QVariantList> getItems(DcController* contr)
	{
		std::vector<QVariantList> items;
        auto param = contr->paramsRegistry().parameter(SP_VCOUNT_WORDDIN_PARSM);
		if (!param)
			return items;

        for (int i = 0; i < param->elementsCount(); i++) {
			QVariantList data;
			data.append(i + 1);
			data.append(contr->getValue(SP_VCOUNT_WORDDIN_PARSM, i).toUInt());
			data.append(contr->getValue(SP_VCIN_CNT_OUTWORD_PARAM, i).toUInt());
			data.append(contr->getValue(SP_VIRTUALCIN_BYTE_PARAM, i).toUInt());

			items.push_back(data);
		}

		return items;
	}

	class TableModel : public QAbstractTableModel
	{
	public:
		TableModel(DcController* contr, QObject* parent = nullptr) :
			QAbstractTableModel(parent),
			m_contr(contr)
		{
			m_items = getItems(contr);
		}

		int rowCount(const QModelIndex& parent = QModelIndex()) const {
			if (parent.isValid())
				return 0;

			return m_items.size();
		}

		int columnCount(const QModelIndex& parent = QModelIndex()) const {
			if (parent.isValid())
				return 0;

			return ColumnsCount;
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const {
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return ::columnName((Columns)section);

			return QVariant();
		}

		Qt::ItemFlags flags(const QModelIndex& index) const {
			Qt::ItemFlags fl = QAbstractTableModel::flags(index);
			if (index.column() < Columns::DiscretColumn)
				return fl;

			if (index.data().isValid())
				fl |= Qt::ItemIsEditable;

			QVariant val = index.data(Qt::CheckStateRole);
			if (val == Qt::Checked || val == Qt::Unchecked)
				fl |= Qt::ItemIsUserCheckable;

			return fl;
		}

		QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const {
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

		bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) {
			int row = index.row();
			int col = index.column();
			auto& data = m_items[row];

			if (!index.isValid())
				return false;

			if (col < Columns::ColumnsCount) {
				data[col] = value;
				
				if (col == Columns::DiscretColumn)
					m_contr->setValue(SP_VCOUNT_WORDDIN_PARSM, row, value);
				if (col == Columns::CounterColumn)
					m_contr->setValue(SP_VCIN_CNT_OUTWORD_PARAM, row, value);
				if (col == Columns::AlgorithmColumn)
					m_contr->setValue(SP_VIRTUALCIN_BYTE_PARAM, row, value);

				emit dataChanged(index, index);
				return true;
			}

			return false;
		}

	private:
		DcController* m_contr;
		std::vector<QVariantList> m_items;
	};
}

namespace Text
{
	namespace DiscretsChanges {
		const QString Title = "Подсчёт состояний дискретов";
	}
}

DcCountersForm::DcCountersForm(DcController *controller) :
	DcForm(controller,  "Счётчики", false)
{
	QVBoxLayout *formLayout = new QVBoxLayout(centralWidget());

	QTabWidget *tabWidget = new QTabWidget;
	formLayout->addWidget(tabWidget);

	tabWidget->addTab(createDiscretsChangesTab(), Text::DiscretsChanges::Title);
}

bool DcCountersForm::isAvailableFor(DcController * controller)
{
	static QList<Param> params = {
		{SP_VCOUNT_WORDDIN_PARSM}
	};

	return hasAny(controller, params);
}

void DcCountersForm::fillReport(DcIConfigReport * report)
{
	auto device = report->device();
    auto param = device->paramsRegistry().parameter(SP_VCOUNT_WORDDIN_PARSM);
	if (param) {
		auto discretsDelegate = std::shared_ptr<ComboBoxDelegate>(g_DiscretsDelegate(device));
		auto countersDelegate = std::shared_ptr<ComboBoxDelegate>(g_CountersDelegate(device));
		
		QStringList headers;
		for (size_t i = 0; i < ColumnsCount; i++)
			headers << columnName(Columns(i));

		report->insertSection(Text::DiscretsChanges::Title);
		DcReportTable table(report->device(), headers, { 5 });

		for (auto&& it : getItems(device)) {
			QStringList values;
            for (int col = 0; col < it.size(); col++) {
				auto value = it[col];
				switch (col)
				{
				case NumColumn:
					values << value.toString();
					break;
				case DiscretColumn:
					values << discretsDelegate->textFor(value.toUInt()).toString();
					break;
				case CounterColumn:
					values << countersDelegate->textFor(value.toUInt()).toString();
					break;
				case AlgorithmColumn:
					values << g_AlgorithmsDelegate.textFor(value.toUInt()).toString();
					break;
				default:
					break;
				}
			}

			table.addRow(values);
		}
		report->insertTable(table);
	}
}

QWidget * DcCountersForm::createDiscretsChangesTab()
{
    auto param = controller()->paramsRegistry().parameter(SP_VCOUNT_WORDDIN_PARSM);
	if (!param)
		return nullptr;

    //  Модель данных и делегаты
    TableModel* model = new TableModel(controller(), this);
    TableView* view = new TableView(model, this);
    Dpc::Gui::ComboBoxDelegate* discrete_delegate = g_DiscretsDelegate(controller(), view);
    Dpc::Gui::ComboBoxDelegate* counters_delegate = g_CountersDelegate(controller(), view);

    //	Функция фильтрации дискретов
    auto discreteUpdate = [=]() {
        QList<uint32_t> list;
        for (int i = 0; i < model->rowCount(); i++) {
            uint32_t value = model->data(model->index(i, DiscretColumn)).toInt();
            if (value != 0xFFFF) list.append(value); }
        discrete_delegate->setExcluding(list); };

    //	Функция фильтрации счетчиков
    auto countersUpdate = [=]() {
        QList<uint32_t> list;
        for (int i = 0; i < model->rowCount(); i++) {
            uint32_t value = model->data(model->index(i, CounterColumn)).toInt();
            if (value != 0xFFFF) list.append(value); }
        counters_delegate->setExcluding(list); };

    //  Проверка существующих данных и привязка сигналов
    discreteUpdate();
    countersUpdate();
    connect(model, &QAbstractItemModel::dataChanged, this, discreteUpdate);
    connect(model, &QAbstractItemModel::dataChanged, this, countersUpdate);

    view->setColumnWidth(NumColumn, 20);
	view->setColumnWidth(DiscretColumn, 300);
	view->setColumnWidth(CounterColumn, 300);
	view->setColumnWidth(AlgorithmColumn, 200);

    view->setItemDelegateForColumn(DiscretColumn, discrete_delegate);
    view->setItemDelegateForColumn(CounterColumn, counters_delegate);
    view->setItemDelegateForColumn(AlgorithmColumn, &g_AlgorithmsDelegate);

	QWidget *tab = new QWidget;
	QGridLayout *tabLayout = new QGridLayout(tab);
	EditorsManager manager(controller(), tabLayout);	
	manager.addWidget(view);
	return tab;
}
