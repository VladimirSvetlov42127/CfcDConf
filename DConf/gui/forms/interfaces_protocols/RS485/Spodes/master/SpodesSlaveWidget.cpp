#include "SpodesSlaveWidget.h"

#include <qdebug.h>
#include <set>
#include <qpushbutton.h>
#include <qtabwidget.h>

#include <gui/editors/EditorsManager.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/widgets/ComboBox.h>
#include <dpc/gui/widgets/LineEdit.h>
#include <dpc/gui/widgets/SpinBox.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <dpc/gui/delegates/LineEditDelegate.h>
#include <gui/forms/interfaces_protocols/ConnectionDiscretWidget.h>

using namespace Dpc::Gui;

namespace {
	const QString Text_NotUse = "Не используется";

	const QString byteRange = "(?:[0-1]?[0-9]?[0-9]|2[0-4][0-9]|25[0-5])";
	const QRegularExpression byteRangeRegExp("^" + byteRange + "\\." + byteRange + "\\." + byteRange + "\\." + byteRange
		+ "\\." + byteRange + "\\." + byteRange + "$");

	const QRegularExpression byteArrayRegExp("^([0-9A-Fa-f]{2}\\:*)*$");

	const ListEditorContainer addrSizeList = { { "1 байт", 1}, {"2 байта", 2}, {"4 байта", 4} };

	class BaseCommandTableModel : public QAbstractTableModel
	{
	public:
		BaseCommandTableModel(QObject*parent = nullptr) : QAbstractTableModel(parent) {
			m_headers << "№" << "Класс объекта" << "Номер объекта" << "Номер атрибута";
		}

		int rowCount(const QModelIndex &parent = QModelIndex()) const override {
			if (parent.isValid())
				return 0;
			return commandsCount();
		}

		int columnCount(const QModelIndex &parent = QModelIndex()) const override {
			if (parent.isValid()) 
				return 0;
			return m_headers.count();
		}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return m_headers.value(section);
			return QVariant();
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if (role == Qt::TextAlignmentRole)
				return Qt::AlignCenter;

			SpodesSlaveCommand *c = getCommand(index.row());
			if (!c)
				return QVariant();

			int column = index.column();
			if (role == Qt::DisplayRole || role == Qt::EditRole) {
				if (0 == column)
					return index.row() + 1;
				if (1 == column)
					return c->objectCId;
				if (2 == column) {
					QString tmp("%1.%2.%3.%4.%5.%6");
					for (int i = 0; i < 6; i++)
						tmp = tmp.arg(c->objectOBIS[i]);
					return tmp;
				}
				if (3 == column)
					return c->objectAttr;
			}
			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			SpodesSlaveCommand *c = getCommand(index.row());
			if (!c)
				return false;

			int column = index.column();
			if (column == 1) {
				c->objectCId = value.toUInt();
			}
			else if (column == 2) {
				QStringList tmp = value.toString().split('.');
				for (int i = 0; i < 6; i++)
					c->objectOBIS[i] = tmp.value(i, "0").toUInt();
			}
			else if (column == 3) {
				c->objectAttr = value.toUInt();
			}
			else
				return false;

			dataChanged(index, index);
			return true;
		}

		Qt::ItemFlags flags(const QModelIndex &index) const override {
			Qt::ItemFlags fl = QAbstractTableModel::flags(index);
			if (index.column())
				return fl | Qt::ItemIsEditable;

			return fl;
		}
		
		void append() {
			int count = rowCount();
			beginInsertRows(QModelIndex(), count, count);
			onAppend();
			endInsertRows();
		}

		void remove(int row) {
			beginRemoveRows(QModelIndex(), row, row);
			onRemove(row);
			endRemoveRows();
		}

	protected:
		void addHeaders(const QStringList &headers) { m_headers << headers; }

	private:
		virtual void onAppend() = 0;
		virtual void onRemove(int row) = 0;
		virtual int commandsCount() const = 0;
		virtual SpodesSlaveCommand* getCommand(int idx) const = 0;

	private:
		QStringList m_headers;
	};

	class ReadCommandTableModel : public BaseCommandTableModel
	{
	public:
		ReadCommandTableModel(QObject *parent = nullptr) : BaseCommandTableModel(parent) { addHeaders({ "Количество" }); }

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if(role == Qt::DisplayRole || role == Qt::EditRole)
				if (index.column() == 4) {
					SpodesSlaveReadCommand*c = getCommand(index.row());
					if (!c)
						return QVariant();
					return c->count;
				}

			return BaseCommandTableModel::data(index, role);
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			if (index.column() == 4) {
				SpodesSlaveReadCommand *c = getCommand(index.row());
				if (!c)
					return false;

				c->count = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}

			return BaseCommandTableModel::setData(index, value, role);
		}

		void onAppend() override { m_list.append(SpodesSlaveReadCommand()); }

		void onRemove(int row) override { m_list.removeAt(row); }

		int commandsCount() const override { return m_list.count(); }

		SpodesSlaveReadCommand* getCommand(int idx) const override {
			if (idx < 0 || idx >= m_list.count())
				return nullptr;
			return (SpodesSlaveReadCommand*) &m_list[idx];
		}

		QList<SpodesSlaveReadCommand> commandsList() const { return m_list; }

		void setCommandsList(const QList<SpodesSlaveReadCommand> &list) {
			beginResetModel();
			m_list = list;
			endResetModel();
		}

	private:
		QList<SpodesSlaveReadCommand> m_list;
	};

	class AnalogCommandTableModel : public ReadCommandTableModel
	{
	public:
		AnalogCommandTableModel(QObject *parent = nullptr) : ReadCommandTableModel(parent) { addHeaders({ "Коэффициент смещения K0", "Коэффициент умножения K1" }); }

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if (role == Qt::DisplayRole || role == Qt::EditRole) {
				SpodesSlaveAnalogCommand *c = getCommand(index.row());
				if (!c)
					return QVariant();

				if (index.column() == 5)
					return QVariant(c->k0);	//	QLocale::system().toString(c->k0);

				if (index.column() == 6)
					return QVariant(c->k1);	//	QLocale::system().toString(c->k1);
			}

			return ReadCommandTableModel::data(index, role);
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			SpodesSlaveAnalogCommand *c = getCommand(index.row());
			if (!c)
				return false;

			if (index.column() == 5) {
				c->k0 = value.toFloat();	//	QLocale::system().toFloat(value.toString());
				emit dataChanged(index, index);
				return true;
			}
			if (index.column() == 6) {
				c->k1 = value.toFloat();	//	QLocale::system().toFloat(value.toString());
				emit dataChanged(index, index);
				return true;
			}

			return ReadCommandTableModel::setData(index, value, role);
		}

		void onAppend() override { m_list.append(SpodesSlaveAnalogCommand()); }

		void onRemove(int row) override { m_list.removeAt(row); }

		int commandsCount() const override { return m_list.count(); }

		SpodesSlaveAnalogCommand* getCommand(int idx) const override {
			if (idx < 0 || idx >= m_list.count())
				return nullptr;
			return (SpodesSlaveAnalogCommand*)&m_list[idx];
		}

		QList<SpodesSlaveAnalogCommand> commandsList() const { return m_list; }

		void setCommandsList(const QList<SpodesSlaveAnalogCommand> &list) {
			beginResetModel();
			m_list = list;
			endResetModel();
		}

	private:
		QList<SpodesSlaveAnalogCommand> m_list;
	};

	class CounterCommandTableModel : public ReadCommandTableModel
	{
	public:
		CounterCommandTableModel(QObject *parent = nullptr) : ReadCommandTableModel(parent) { addHeaders({ "Множитель" }); }

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if (role == Qt::DisplayRole || role == Qt::EditRole) {
				SpodesSlaveCounterCommand *c = getCommand(index.row());
				if (!c)
					return QVariant();

				if (index.column() == 5)
					return c->multiplicator;
			}

			return ReadCommandTableModel::data(index, role);
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			SpodesSlaveCounterCommand *c = getCommand(index.row());
			if (!c)
				return false;

			if (index.column() == 5) {
				c->multiplicator = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}

			return ReadCommandTableModel::setData(index, value, role);
		}

		void onAppend() override { m_list.append(SpodesSlaveCounterCommand()); }

		void onRemove(int row) override { m_list.removeAt(row); }

		int commandsCount() const override { return m_list.count(); }

		SpodesSlaveCounterCommand* getCommand(int idx) const override {
			if (idx < 0 || idx >= m_list.count())
				return nullptr;
			return (SpodesSlaveCounterCommand*)&m_list[idx];
		}

		QList<SpodesSlaveCounterCommand> commandsList() const { return m_list; }

		void setCommandsList(const QList<SpodesSlaveCounterCommand> &list) {
			beginResetModel();
			m_list = list;
			endResetModel();
		}

	private:
		QList<SpodesSlaveCounterCommand> m_list;
	};

	class MethodCommandTableModel : public BaseCommandTableModel
	{
	public:
		MethodCommandTableModel(QObject *parent = nullptr) : BaseCommandTableModel(parent) { addHeaders({ "Массив параметров" }); }

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if (role == Qt::DisplayRole || role == Qt::EditRole)
				if (index.column() == 4) {
					SpodesSlaveMethodCommand*c = getCommand(index.row());
					if (!c)
						return QVariant();
					return c->params.toHex(':').toUpper();
				}

			return BaseCommandTableModel::data(index, role);
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			if (index.column() == 4) {
				SpodesSlaveMethodCommand *c = getCommand(index.row());
				if (!c)
					return false;

				c->params = QByteArray::fromHex(value.toByteArray());
				emit dataChanged(index, index);
				return true;
			}

			return BaseCommandTableModel::setData(index, value, role);
		}

		void onAppend() override { m_list.append(SpodesSlaveMethodCommand()); }

		void onRemove(int row) override { m_list.removeAt(row); }

		int commandsCount() const override { return m_list.count(); }

		SpodesSlaveMethodCommand* getCommand(int idx) const override {
			if (idx < 0 || idx >= m_list.count())
				return nullptr;
			return (SpodesSlaveMethodCommand*)&m_list[idx];
		}

		QList<SpodesSlaveMethodCommand> commandsList() const { return m_list; }

		void setCommandsList(const QList<SpodesSlaveMethodCommand> &list) {
			beginResetModel();
			m_list = list;
			endResetModel();
		}

	private:
		QList<SpodesSlaveMethodCommand> m_list;
	};

	class TimeSyncCommandTableModel : public BaseCommandTableModel
	{
	public:
		TimeSyncCommandTableModel(QObject *parent = nullptr) : BaseCommandTableModel(parent) { 
			addHeaders({ "Номер метода сдвига", "Минимальное расхождение (ск)", "Максимальное отклонение (ск)", "Таймаут (ск)" }); 
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			if (role == Qt::DisplayRole || role == Qt::EditRole) {
				SpodesSlaveTimeSyncCommand *c = getCommand(index.row());
				if (!c)
					return QVariant();

				if (index.column() == 4) {
					return c->timeShiftMethod;
				}
				if (index.column() == 5) {
					return c->minTimeShift;
				}
				if (index.column() == 6) {
					return c->maxTimeShift;
				}
				if (index.column() == 7) {
					return c->syncTimeout;
				}
			}

			return BaseCommandTableModel::data(index, role);
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			SpodesSlaveTimeSyncCommand *c = getCommand(index.row());
			if (!c)
				return false;

			if (index.column() == 4) {
				c->timeShiftMethod = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}
			if (index.column() == 5) {
				c->minTimeShift = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}
			if (index.column() == 6) {
				c->maxTimeShift = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}
			if (index.column() == 7) {
				c->syncTimeout = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}

			return BaseCommandTableModel::setData(index, value, role);
		}

		void onAppend() override { m_list.append(SpodesSlaveTimeSyncCommand());	}

		void onRemove(int row) override { m_list.removeAt(row); }

		int commandsCount() const override { return m_list.count(); }

		SpodesSlaveTimeSyncCommand* getCommand(int idx) const override {
			if (idx < 0 || idx >= m_list.count())
				return nullptr;
			return (SpodesSlaveTimeSyncCommand*)&m_list[idx];
		}

		QList<SpodesSlaveTimeSyncCommand> commandsList() const { return m_list; }

		void setCommandsList(const QList<SpodesSlaveTimeSyncCommand> &list) {
			beginResetModel();
			m_list = list;
			endResetModel();
		}

	private:
		QList<SpodesSlaveTimeSyncCommand> m_list;
	};

	TableView* setupTableView(TableView *view)
	{
		view->setSelectionMode(QAbstractItemView::ExtendedSelection);
		view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::CurrentChanged);
		view->horizontalHeader()->setStretchLastSection(true);

		view->horizontalHeader()->resizeSection(0, 20);
		view->horizontalHeader()->resizeSection(1, 100);
		view->horizontalHeader()->resizeSection(2, 150);
		view->horizontalHeader()->resizeSection(3, 100);

		view->setItemDelegateForColumn(1, new IntSpinBoxDelegate(0, USHRT_MAX, view));
		auto delegate2 = new LineEditDelegate(view);
		delegate2->setValidator(new QRegularExpressionValidator(byteRangeRegExp, view));
		view->setItemDelegateForColumn(2, delegate2);
		view->setItemDelegateForColumn(3, new IntSpinBoxDelegate(0, UCHAR_MAX, view));

		if (dynamic_cast<ReadCommandTableModel*>(view->model())) {
			view->horizontalHeader()->resizeSection(4, 100);
			view->setItemDelegateForColumn(4, new IntSpinBoxDelegate(0, USHRT_MAX, view));
		}

		if (dynamic_cast<CounterCommandTableModel*>(view->model())) {
			view->horizontalHeader()->resizeSection(5, 100);

			auto delegate5 = new UIntSpinBoxDelegate(0, UINT32_MAX, view);
			delegate5->setSpecialValueText(Text_NotUse);
			view->setItemDelegateForColumn(5, delegate5);
		}

		if (dynamic_cast<AnalogCommandTableModel*>(view->model())) {
			view->horizontalHeader()->resizeSection(5, 170);
			view->horizontalHeader()->resizeSection(6, 170);

            auto delegate5 = new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), view);
			delegate5->setSpecialValueText(Text_NotUse);
            auto delegate6 = new DoubleSpinBoxDelegate(0, std::numeric_limits<float>::max(), view);
			delegate6->setSpecialValueText(Text_NotUse);

			view->setItemDelegateForColumn(5, delegate5);
			view->setItemDelegateForColumn(6, delegate6);
		}

		if (dynamic_cast<MethodCommandTableModel*>(view->model())) {
			view->horizontalHeader()->resizeSection(4, 150);
			auto delegate4 = new LineEditDelegate(view);
			delegate4->setValidator(new QRegularExpressionValidator(byteArrayRegExp, view));
			view->setItemDelegateForColumn(4, delegate4);
		}

		if (dynamic_cast<TimeSyncCommandTableModel*>(view->model())) {
			view->horizontalHeader()->resizeSection(4, 150);
			view->horizontalHeader()->resizeSection(5, 200);
			view->horizontalHeader()->resizeSection(6, 200);
			view->horizontalHeader()->resizeSection(7, 100);
			view->setItemDelegateForColumn(4, new IntSpinBoxDelegate(0, UCHAR_MAX, view));
			view->setItemDelegateForColumn(5, new IntSpinBoxDelegate(0, USHRT_MAX, view));
			view->setItemDelegateForColumn(6, new IntSpinBoxDelegate(0, USHRT_MAX, view));
			view->setItemDelegateForColumn(7, new UIntSpinBoxDelegate(0, UINT32_MAX, view));
		}

		return view;
	}	

} // namespace

SpodesSlaveWidget::SpodesSlaveWidget(DcController *controller, int interfaceIdx, int deviceIdx, QWidget *parent) :
	QWidget(parent),
	m_tabWidget(new QTabWidget(this)),
	m_analogsTableView(setupTableView(new TableView(new AnalogCommandTableModel(this)))),
	m_discretsTableView(setupTableView(new TableView(new ReadCommandTableModel(this)))),
	m_countersTableView(setupTableView(new TableView(new CounterCommandTableModel(this)))),
	m_discretOuputsTableView(setupTableView(new TableView(new ReadCommandTableModel(this)))),
	m_scalersTableView(setupTableView(new TableView(new ReadCommandTableModel(this)))),
	m_methodsTableView(setupTableView(new TableView(new MethodCommandTableModel(this)))),
	m_timeSyncTableView(setupTableView(new TableView(new TimeSyncCommandTableModel(this)))),
	m_activeCheckBox(new QCheckBox),
	m_timeoutEdit(new IntSpinBox),
	m_logAddressEdit(new IntSpinBox),
	m_phisAddressEdit(new IntSpinBox),
	m_addrSizeComboBox(new ListParamEditor(addrSizeList)),
	m_passwordEdit(new LineEdit(this)),
	m_connectionDiscretWidget(new ConnectionDiscretWidget(controller, interfaceIdx, deviceIdx, QString("СПОДЭС Слейв(%1: %2)").arg(interfaceIdx).arg(deviceIdx), this))
{
	m_activeCheckBox->setTristate(false);
	m_timeoutEdit->setRange(0, UCHAR_MAX);
	m_logAddressEdit->setRange(0, USHRT_MAX);
	m_phisAddressEdit->setRange(0, USHRT_MAX);
	m_addrSizeComboBox->setValue(QString());
	m_passwordEdit->setMaxLength(20);	

	m_tabWidget->addTab(m_analogsTableView, "Аналоги");
	m_tabWidget->addTab(m_discretsTableView, "Дискреты");
	m_tabWidget->addTab(m_countersTableView, "Счетчики");
	m_tabWidget->addTab(m_discretOuputsTableView, "Дискретные выходы");
	m_tabWidget->addTab(m_scalersTableView, "Масштабы");
	m_tabWidget->addTab(m_methodsTableView, "Методы");
	m_tabWidget->addTab(m_timeSyncTableView, "Синхронизация времени");

	QPushButton *addButton = new QPushButton("Добавить", this);
	connect(addButton, &QPushButton::clicked, this, &SpodesSlaveWidget::onAddButton);

	QPushButton *removeButton = new QPushButton("Удалить", this);
	connect(removeButton, &QPushButton::clicked, this, &SpodesSlaveWidget::onRemoveButton);

	QGroupBox *commonGroupBox = new QGroupBox(this);
	EditorsManager mg(nullptr, new QGridLayout(commonGroupBox));
	mg.addWidget(m_activeCheckBox, "Задействован");
	mg.addWidget(m_timeoutEdit, "Таймаут ответа, мс");
	mg.addWidget(m_logAddressEdit, "Логический адрес");
	mg.addWidget(m_phisAddressEdit, "Физический адрес");
	mg.addWidget(m_addrSizeComboBox, "Размер адреса");
	mg.addWidget(m_passwordEdit, "Пароль");
	mg.addWidget(m_connectionDiscretWidget, "Дискрет связи");
	mg.addVerticalStretch();

	QGridLayout *mainLayout = new QGridLayout(this);
	mainLayout->addWidget(commonGroupBox, 0, 0, 2, 1);

	mainLayout->addWidget(addButton, 0, 2);
	mainLayout->addWidget(removeButton, 0, 3);

	mainLayout->addWidget(m_tabWidget, 1, 1, 2, 3);
	mainLayout->setColumnStretch(1, 1);
	mainLayout->setRowStretch(1, 1);
}

SpodesSlaveWidget::~SpodesSlaveWidget()
{
}

bool SpodesSlaveWidget::isActive() const
{
	return m_activeCheckBox->isChecked();
}

uint8_t SpodesSlaveWidget::timeout() const
{
	return m_timeoutEdit->value();
}

uint8_t SpodesSlaveWidget::addrSize() const
{
	return m_addrSizeComboBox->value().toUInt();
}

uint16_t SpodesSlaveWidget::logAddress() const
{
	return m_logAddressEdit->value();
}

uint16_t SpodesSlaveWidget::phisAddress() const
{
	return m_phisAddressEdit->value();
}

QString SpodesSlaveWidget::password() const
{
	return m_passwordEdit->text();
}

void SpodesSlaveWidget::setActive(bool active)
{
	m_activeCheckBox->setChecked(active);
}

void SpodesSlaveWidget::setTimeout(uint8_t value)
{
	m_timeoutEdit->setValue(value);
}

void SpodesSlaveWidget::setLogAddress(uint16_t value)
{
	m_logAddressEdit->setValue(value);
}

void SpodesSlaveWidget::setPhisAddress(uint16_t value)
{
	m_phisAddressEdit->setValue(value);
}

void SpodesSlaveWidget::setAddrSize(uint8_t value)
{
	m_addrSizeComboBox->setValue(QString::number(value));
}

void SpodesSlaveWidget::setPassword(const QString & value)
{
	m_passwordEdit->setText(value);
}

QList<SpodesSlaveAnalogCommand> SpodesSlaveWidget::analogCommands() const
{
	AnalogCommandTableModel *model = dynamic_cast<AnalogCommandTableModel*>(m_analogsTableView->model());
	if (!model)
		return QList<SpodesSlaveAnalogCommand>();

	return model->commandsList();
}

QList<SpodesSlaveReadCommand> SpodesSlaveWidget::discretCommands() const
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_discretsTableView->model());
	if (!model)
		return QList<SpodesSlaveReadCommand>();

	return model->commandsList();
}

QList<SpodesSlaveCounterCommand> SpodesSlaveWidget::counterCommands() const
{
	CounterCommandTableModel *model = dynamic_cast<CounterCommandTableModel*>(m_countersTableView->model());
	if (!model)
		return QList<SpodesSlaveCounterCommand>();

	return model->commandsList();
}

QList<SpodesSlaveReadCommand> SpodesSlaveWidget::discretOutputCommands() const
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_discretOuputsTableView->model());
	if (!model)
		return QList<SpodesSlaveReadCommand>();

	return model->commandsList();
}

QList<SpodesSlaveReadCommand> SpodesSlaveWidget::scalerCommands() const
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_scalersTableView->model());
	if (!model)
		return QList<SpodesSlaveReadCommand>();

	return model->commandsList();
}

QList<SpodesSlaveMethodCommand> SpodesSlaveWidget::methodCommands() const
{
	MethodCommandTableModel *model = dynamic_cast<MethodCommandTableModel*>(m_methodsTableView->model());
	if (!model)
		return QList<SpodesSlaveMethodCommand>();

	return model->commandsList();
}

QList<SpodesSlaveTimeSyncCommand> SpodesSlaveWidget::timeSyncCommands() const
{
	TimeSyncCommandTableModel *model = dynamic_cast<TimeSyncCommandTableModel*>(m_timeSyncTableView->model());
	if (!model)
		return QList<SpodesSlaveTimeSyncCommand>();

	return model->commandsList();
}

void SpodesSlaveWidget::setAnalogCommands(const QList<SpodesSlaveAnalogCommand>& commands)
{
	AnalogCommandTableModel *model = dynamic_cast<AnalogCommandTableModel*>(m_analogsTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setDiscretCommands(const QList<SpodesSlaveReadCommand>& commands)
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_discretsTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setCounterCommands(const QList<SpodesSlaveCounterCommand>& commands)
{
	CounterCommandTableModel *model = dynamic_cast<CounterCommandTableModel*>(m_countersTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setDiscretOutputCommands(const QList<SpodesSlaveReadCommand>& commands)
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_discretOuputsTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setScalerCommands(const QList<SpodesSlaveReadCommand>& commands)
{
	ReadCommandTableModel *model = dynamic_cast<ReadCommandTableModel*>(m_scalersTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setMethodCommands(const QList<SpodesSlaveMethodCommand>& commands)
{
	MethodCommandTableModel *model = dynamic_cast<MethodCommandTableModel*>(m_methodsTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::setTimeSyncCommands(const QList<SpodesSlaveTimeSyncCommand>& commands)
{
	TimeSyncCommandTableModel *model = dynamic_cast<TimeSyncCommandTableModel*>(m_timeSyncTableView->model());
	if (!model)
		return;

	model->setCommandsList(commands);
}

void SpodesSlaveWidget::showEvent(QShowEvent * event)
{
	m_connectionDiscretWidget->updateList();
	QWidget::showEvent(event);
}

void SpodesSlaveWidget::onAddButton()
{
	QTableView *view = dynamic_cast<QTableView*>(m_tabWidget->currentWidget());
	if (!view)
		return;

	BaseCommandTableModel *model = dynamic_cast<BaseCommandTableModel*>(view->model());
	if (!model)
		return;

	model->append();
}

void SpodesSlaveWidget::onRemoveButton()
{
	QTableView *view = dynamic_cast<QTableView*>(m_tabWidget->currentWidget());
	if (!view)
		return;

	BaseCommandTableModel *model = dynamic_cast<BaseCommandTableModel*>(view->model());
	if (!model)
		return;
		
	std::set<int> rowsSet;
	for (auto &it : view->selectionModel()->selectedRows())
		rowsSet.insert(it.row());

	for (auto it = rowsSet.rbegin(); it != rowsSet.rend(); it++)
		model->remove(*it);
}
