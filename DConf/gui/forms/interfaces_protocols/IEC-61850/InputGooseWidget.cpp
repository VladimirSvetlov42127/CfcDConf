#include "InputGooseWidget.h"

#include <set>
#include <qpushbutton.h>
#include <qmimedata.h>

#include <gui/forms/interfaces_protocols/ConnectionDiscretWidget.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/widgets/ComboBox.h>
#include <dpc/gui/delegates/ComboBoxDelegate.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>
#include <gui/editors/EditorsManager.h>

using namespace Dpc::Gui;

namespace {
	const QStringList dataSetTableHeaders = { "№", "Данные" };
	const QStringList incomingDataTableHeaders = { "№" , "Тип атрибута", "Номер атрибута", "Тип канала", "Номер Канала" };
	const QStringList lgosModeList ={ "Включен", "Блокирован", "Тест", "Блокирован в тесте", "Отключен" };
	const QList<int> ratioTimeAllowedToLiveList = { 1, 2, 3, 4, 5, 6, 7 };

	const QString Text_NotSet = "Не установлено";

	const QString MIMETYPE_DEFAULT = "application/x-qabstractitemmodeldatalist";

	const uint16_t MaxType = 0xF;
	const uint16_t MaxNum = 0xFFF;
	const uint16_t MaxIntAddrLN = 0xF;
	const uint16_t MaxIntAddr = 0xFFF;	

	enum TagType {
		Tag_Value = 0,
		Tag_Quality,
		Tag_Ts,

		Tag_Empty = MaxType
	};

	enum IntAddrLN {
		ExtInput_Din = 1,
		ExtInput_Ain,
		ExtInput_Cin,
		ExtInput_Dout,
		ExtInput_Aout,

		ExtInput_Din_DPS = 9,
		ExtInput_Ain_INT = 10,
		ExtInput_Cin_64 = 11,
		ExtInput_Dout_DPC = 12,

		ExtInput_Empty = MaxIntAddrLN,
	};	

	ComboBoxDelegate g_TagTypeDelegate = {
		{"Значение", Tag_Value },
		{"Качество", Tag_Quality},
		{"Время", Tag_Ts},
		{Text_NotSet, Tag_Empty}
	};

	ComboBoxDelegate g_IntAddrLNDelegate = {
		{"Din", ExtInput_Din },
		{"Ain",	ExtInput_Ain},
		{"Cin",	ExtInput_Cin},
		{"Dout", ExtInput_Dout},
		{"Aout", ExtInput_Aout},
		{"Din_DPS", ExtInput_Din_DPS },
		{"Ain_INT", ExtInput_Ain_INT},
		{"Cin_64", ExtInput_Cin_64},
		{"Dout_DPC", ExtInput_Dout_DPC},
		{Text_NotSet, ExtInput_Empty},
	};

	struct TagItem
	{
		uint8_t type;
		uint16_t num;
	};
	
	class DataSetTableModel : public QAbstractTableModel
	{
	public:
		DataSetTableModel(const DataSetPtr &ds, QObject *parent = nullptr) : QAbstractTableModel(parent), m_ds(ds) {}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return dataSetTableHeaders.value(section);

			return QVariant();
		}

		int rowCount(const QModelIndex &parent = QModelIndex()) const override {
			if (!m_ds) 
				return 0;

			return m_ds->fcdaList().count();
		}

		int columnCount(const QModelIndex &parent = QModelIndex()) const override {
			return dataSetTableHeaders.count();
		}
		
		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid() || !m_ds)
				return QVariant();

			int col = index.column();
			FCDAPtr f = m_ds->fcdaList().at(index.row());
			if (Qt::DisplayRole == role) {
				if (col == 0)
					return index.row();
				if (col == 1)
					return f->name();
			}

			return QVariant();
		}

		Qt::ItemFlags flags(const QModelIndex &index) const override {
			if (!index.isValid())
				return Qt::NoItemFlags;

			return QAbstractTableModel::flags(index) | Qt::ItemIsDragEnabled;
		}

		QMimeData* mimeData(const QModelIndexList &indexes) const {
			QMimeData *mime = new QMimeData;
			QByteArray encodedData;
			QDataStream stream(&encodedData, QIODevice::WriteOnly);

			std::set<int> rows;
			for (const QModelIndex &index : indexes) {
				if (!index.isValid())
					continue;

				rows.insert(index.row());
			}

			for (auto row : rows)
				for (auto it : tagItems(row))
					stream << it.type << it.num;

			mime->setData(MIMETYPE_DEFAULT, encodedData);
			return mime;
		}

	private:
		QList<TagItem> tagItems(int row) const {
			uint16_t startTagNum = 0;
			for (int i = 0; i < row; i++) {
				auto fcda = m_ds->fcdaList().at(i);
				if (fcda->daName().isEmpty())
					startTagNum += 4;
				else
					startTagNum++;
			}

			QList<TagItem> result;
			QString daName = m_ds->fcdaList().at(row)->daName();
			if (daName.isEmpty()) {
				startTagNum++;
				result << TagItem{ Tag_Value, startTagNum++ };
				result << TagItem{ Tag_Quality, startTagNum++ };
				result << TagItem{ Tag_Ts, startTagNum++ };
			}
			else if ((daName.count() == 1 && daName == "q") || (daName.count() > 1 && daName.endsWith(".q")))
				result << TagItem{ Tag_Quality, startTagNum };
			else if ((daName.count() == 1 && daName == "t") || (daName.count() > 1 && daName.endsWith(".t")))
				result << TagItem{ Tag_Ts, startTagNum };
			else
				result << TagItem{ Tag_Value, startTagNum };

			return result;
		}

	private:
		DataSetPtr m_ds;
	};

	class IncomingDataTableModel : public QAbstractTableModel
	{
	public:
		IncomingDataTableModel(const QList<GooseSubscriberEntryCfg> &dataList, QObject *parent = nullptr) : QAbstractTableModel(parent), m_dataList(dataList) {}

		QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
			if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
				return incomingDataTableHeaders.value(section);			

			return QVariant();
		}

		Qt::ItemFlags flags(const QModelIndex &index) const override {
			if (!index.isValid())
				return Qt::NoItemFlags;

			Qt::ItemFlags f = QAbstractTableModel::flags(index) | Qt::ItemIsDropEnabled;
			if (index.column() > 0)
				f |= Qt::ItemIsEditable;

			return f;
		}

		int rowCount(const QModelIndex &parent = QModelIndex()) const override {
			if (parent.isValid())
				return 0;

			return m_dataList.count();
		}

		int columnCount(const QModelIndex &parent = QModelIndex()) const override {
			if (parent.isValid())
				return 0;

			return incomingDataTableHeaders.count();
		}

		QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
			if (!index.isValid())
				return QVariant();

			int col = index.column();
			auto item = m_dataList.at(index.row());
			if (Qt::DisplayRole == role || Qt::EditRole == role) {
				if (col == 0)
					return index.row();
				if (col == 1)
					return item.tagType;
				if (col == 2)
					return static_cast<int>(item.tagNum == MaxNum ? -1 : item.tagNum);
				if (col == 3)
					return item.intAddrLn;
				if (col == 4)
					return static_cast<int>(item.intAddr == MaxIntAddr ? -1 : item.intAddr);
			}			

			if (Qt::TextAlignmentRole == role)
				return Qt::AlignCenter;

			if (Qt::ToolTipRole == role && col == 4)
				return "Нумерация каналов начинается с 0";

			return QVariant();
		}

		bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override {
			if (!index.isValid())
				return false;

			auto &item = m_dataList[index.row()];
			int col = index.column();
			if (col == 1) {
				item.tagType = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}
			if (col == 2) {
				auto val = value.toInt();
				item.tagNum = val == -1 ? MaxNum : val;
				emit dataChanged(index, index);
				return true;
			}
			if (col == 3) {
				item.intAddrLn = value.toUInt();
				emit dataChanged(index, index);
				return true;
			}
			if (col == 4) {
				auto val = value.toInt();
				item.intAddr = val == -1 ? MaxIntAddr : val;
				emit dataChanged(index, index);
				return true;
			}

			return false;
		}		

		bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) {
			if (action == Qt::IgnoreAction)
				return true;

			if (!parent.isValid())
				return false;

			QByteArray encodedData = data->data(MIMETYPE_DEFAULT);
			QDataStream stream(&encodedData, QIODevice::ReadOnly);

			int currentRow = parent.row();
			while (!stream.atEnd()) {
				if (currentRow >= m_dataList.count())
					break;
				
				TagItem item;
				stream >> item.type >> item.num;
				auto &entry = m_dataList[currentRow];
				entry.tagType = item.type;
				entry.tagNum = item.num;
				emit dataChanged(index(currentRow, 1), index(currentRow, 2));
				currentRow++;
			}

			return true;
		}

		void setDataList(const QList<GooseSubscriberEntryCfg> &dataList) {
			beginResetModel();
			m_dataList = dataList;
			endResetModel();
		}

		QList<GooseSubscriberEntryCfg> dataList() const { return m_dataList; }

		void clear() {
			beginResetModel();
			for (auto &entry : m_dataList)
				entry = GooseSubscriberEntryCfg();
			endResetModel();
		}

	private:
		QList<GooseSubscriberEntryCfg> m_dataList;
	};
	
} // namespace

InputGooseWidget::InputGooseWidget(DcController *controller, int interfaceIdx, int deviceIdx, QWidget *parent) :
	QWidget(parent),
	m_publisherEdit(new QLineEdit(this)),
	m_macAddressEdit(new QLineEdit(this)),
	m_vlanEdit(new QLineEdit(this)),
	m_priorityVlanEdit(new QLineEdit(this)),
	m_appIdEdit(new QLineEdit(this)),
	m_confRevEdit(new QLineEdit(this)),
	m_gooseIdEdit(new QLineEdit(this)),
	m_goCbRefEdit(new QLineEdit(this)),
	m_goDatSetEdit(new QLineEdit(this)),
	m_dataSetView(new TableView(this)), 
	m_incomingDataView(new TableView(this)),
	m_lgosComboBox(new ComboBox(this)),
	m_activateCheckBox(new QCheckBox(this)),
	m_controlAfterRestartCheckBox(new QCheckBox(this)),
	m_controlAfterReconnectionCheckBox(new QCheckBox(this)),
	m_ratioTimeAllowedToLiveComboBox(new ComboBox(this)),
	m_connectionDiscretWidget(new ConnectionDiscretWidget(controller, interfaceIdx, deviceIdx, QString("GOOSE %1").arg(deviceIdx), this))
{	
	m_publisherEdit->setReadOnly(true);
	m_macAddressEdit->setReadOnly(true);
	m_vlanEdit->setReadOnly(true);
	m_priorityVlanEdit->setReadOnly(true);
	m_appIdEdit->setReadOnly(true);
	m_confRevEdit->setReadOnly(true);
	m_gooseIdEdit->setReadOnly(true);
	m_goCbRefEdit->setReadOnly(true);
	m_goDatSetEdit->setReadOnly(true);
	
	m_dataSetView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_dataSetView->setMinimumWidth(200);
	m_dataSetView->setDragEnabled(true);
	m_dataSetView->horizontalHeader()->setStretchLastSection(true);

	auto tagNumDelegate = new IntSpinBoxDelegate(-1, MaxNum - 1, this);
	tagNumDelegate->setSpecialValueText(Text_NotSet);
	auto intAddrDelegate = new IntSpinBoxDelegate(-1, MaxIntAddr - 1, this);
	intAddrDelegate->setSpecialValueText(Text_NotSet);

	m_incomingDataView->setModel(new IncomingDataTableModel(QList<GooseSubscriberEntryCfg>(), this));
	m_incomingDataView->setSelectionMode(QAbstractItemView::NoSelection);
	m_incomingDataView->setAcceptDrops(true);
	m_incomingDataView->setDropIndicatorShown(true);
	m_incomingDataView->horizontalHeader()->setStretchLastSection(true);

	m_incomingDataView->setItemDelegateForColumn(1, &g_TagTypeDelegate);
	m_incomingDataView->setItemDelegateForColumn(3,	&g_IntAddrLNDelegate);
	m_incomingDataView->setItemDelegateForColumn(2, tagNumDelegate);
	m_incomingDataView->setItemDelegateForColumn(4, intAddrDelegate);	

	m_lgosComboBox->addItems(lgosModeList);
	for(auto it: ratioTimeAllowedToLiveList)
		m_ratioTimeAllowedToLiveComboBox->addItem(QString::number(it), it);

	QPushButton *publisherClearButton = new QPushButton("X", this);
	publisherClearButton->setMaximumWidth(20);
	connect(publisherClearButton, &QPushButton::clicked, this, &InputGooseWidget::onClearButton);

	QPushButton *publisherBrowseButton = new QPushButton("...", this);
	publisherBrowseButton->setMaximumWidth(20);
	connect(publisherBrowseButton, &QPushButton::clicked, this, [=]() { emit this->selectPublisher(); });

	QHBoxLayout *publisherEditLayout = new QHBoxLayout;
	publisherEditLayout->addWidget(m_publisherEdit);
	publisherEditLayout->addWidget(publisherClearButton);
	publisherEditLayout->addWidget(publisherBrowseButton);	

	QGridLayout *layout = new QGridLayout(this);
	EditorsManager mg(nullptr, layout);
	
	mg.addWidget(m_activateCheckBox, "Активен");
	mg.addWidget(m_lgosComboBox, "Режим LGOS");
	mg.addWidget(m_connectionDiscretWidget, "Дискрет связи");
	mg.addLayout(publisherEditLayout, "Издатель");
	mg.addWidget(m_macAddressEdit, "MAC-адрес");
	mg.addWidget(m_vlanEdit, "VLAN");
	mg.addWidget(m_priorityVlanEdit, "Приоритет VLAN");
	mg.addWidget(m_appIdEdit, "AppID");
	mg.addWidget(m_confRevEdit, "ConfRev");
	mg.addWidget(m_gooseIdEdit, "GooseID");
	mg.addWidget(m_goCbRefEdit, "GoCbRef");
	mg.addWidget(m_goDatSetEdit, "GoDatSet");
	mg.addVerticalStretch(1);

	auto editColumn = mg.editorColumn();
	layout->setColumnMinimumWidth(editColumn + 1, 20);

	int dataSetColumn = editColumn + 2;
	layout->addWidget(new QLabel("Датасет"), 0, dataSetColumn);
	layout->addWidget(m_dataSetView, 1, dataSetColumn, layout->rowCount(), 1);
	layout->setColumnMinimumWidth(dataSetColumn + 1, 20);

	int inputDataColumn = dataSetColumn + 2;
	layout->addWidget(new QLabel("Команды управления и входящие данные"), 0, inputDataColumn);
	layout->addWidget(m_incomingDataView, 1, inputDataColumn, layout->rowCount(), 1);
	layout->setColumnStretch(inputDataColumn, 2);
	layout->setColumnMinimumWidth(inputDataColumn + 1, 20);
	
	int inputDataSettingsLabelColumn = inputDataColumn + 3;
	int inputDataSettingsEditorColumn = inputDataSettingsLabelColumn + 1;
	int row = 0;
	layout->addWidget(new QLabel("Выполнение управления статическими\nвыходами после рестарта"), row, inputDataSettingsLabelColumn);
	layout->addWidget(m_controlAfterRestartCheckBox, row, inputDataSettingsEditorColumn);
	row++;

	layout->addWidget(new QLabel("Выполнение управления статическими\nвыходами после восстановления соединения"), row, inputDataSettingsLabelColumn);
	layout->addWidget(m_controlAfterReconnectionCheckBox, row, inputDataSettingsEditorColumn);
	row++;

	layout->addWidget(new QLabel("Коэффициент умножения принятого\ntimeAllowedtoLive для определения рассоединения"), row, inputDataSettingsLabelColumn);
	layout->addWidget(m_ratioTimeAllowedToLiveComboBox, row, inputDataSettingsEditorColumn);
	row++;
}

InputGooseWidget::~InputGooseWidget()
{
}

void InputGooseWidget::setPublisher(const GoosePublisher & pub)
{
	m_publisher = pub;

	if (pub.gseControl) {
		m_publisherEdit->setText(QString("%1/%2/%3/%4").arg(pub.ied->name(), pub.device->inst(), pub.ln->name(), pub.gseControl->name()));
		if (pub.gseControl->gseAP()) {
			m_macAddressEdit->setText(pub.gseControl->gseAP()->macAddress());
			m_vlanEdit->setText(pub.gseControl->gseAP()->vlanId());
			m_priorityVlanEdit->setText(pub.gseControl->gseAP()->vlanPriority());
			m_appIdEdit->setText(QString("0x%1").arg(pub.gseControl->gseAP()->appID()));
			
		}		
		m_confRevEdit->setText(QString::number(pub.gseControl->confRev()));
		m_gooseIdEdit->setText(pub.gseControl->appID());
		m_goCbRefEdit->setText(QString("%1%2/%3$GO$%4").arg(pub.ied->name(), pub.device->inst(), pub.ln->name(), pub.gseControl->name()));
		m_goDatSetEdit->setText(QString("%1%2/%3$%4").arg(pub.ied->name(), pub.device->inst(), pub.ln->name(), pub.gseControl->datSet()->name()));

		m_dataSetView->setModel(new DataSetTableModel(pub.gseControl->datSet(), this));	
	} 
	else {
		m_publisherEdit->setText(QString());
		m_macAddressEdit->setText(QString());
		m_vlanEdit->setText(QString());
		m_priorityVlanEdit->setText(QString());
		m_appIdEdit->setText(QString());
		m_confRevEdit->setText(QString());
		m_gooseIdEdit->setText(QString());
		m_goCbRefEdit->setText(QString());
		m_goDatSetEdit->setText(QString());
		m_dataSetView->setModel(new DataSetTableModel(DataSetPtr(), this));
	}
}

QList<GooseSubscriberEntryCfg> InputGooseWidget::incomingDataList() const
{
	IncomingDataTableModel *model = dynamic_cast<IncomingDataTableModel*>(m_incomingDataView->model());
	if (!model)
		return QList<GooseSubscriberEntryCfg>();

	return model->dataList();
}

void InputGooseWidget::setIncomingDataList(const QList<GooseSubscriberEntryCfg>& list)
{
	IncomingDataTableModel *model = dynamic_cast<IncomingDataTableModel*>(m_incomingDataView->model());
	if (!model)
		return;

	model->setDataList(list);
	m_incomingDataView->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
	m_incomingDataView->horizontalHeader()->resizeSection(1, 120);
	m_incomingDataView->horizontalHeader()->resizeSection(2, 150);
	m_incomingDataView->horizontalHeader()->resizeSection(3, 120);
}

int InputGooseWidget::lgosMode() const
{
	return m_lgosComboBox->currentIndex();
}

bool InputGooseWidget::isActive() const
{
	return m_activateCheckBox->isChecked();
}

bool InputGooseWidget::isControlAfterRestart() const
{
	return m_controlAfterRestartCheckBox->isChecked();
}

bool InputGooseWidget::isControlAfterReconnection() const
{
	return m_controlAfterReconnectionCheckBox->isChecked();
}

int InputGooseWidget::ratioTimeAllowedToLive() const
{
	return m_ratioTimeAllowedToLiveComboBox->currentData().toUInt();
}

void InputGooseWidget::setLgosMode(int mode)
{
	m_lgosComboBox->setCurrentIndex(mode < m_lgosComboBox->count() ? mode : -1);
}

void InputGooseWidget::setActive(bool active)
{
	m_activateCheckBox->setChecked(active);
}

void InputGooseWidget::setControlAfterRestart(bool control)
{
	m_controlAfterRestartCheckBox->setChecked(control);
}

void InputGooseWidget::setControlAfterReconnection(bool control)
{
	m_controlAfterReconnectionCheckBox->setChecked(control);
}

void InputGooseWidget::setRatioTimeAllowedToLive(int ratio)
{
	int currentIndex = -1;
	for (size_t i = 0; i < m_ratioTimeAllowedToLiveComboBox->count(); i++)
		if (m_ratioTimeAllowedToLiveComboBox->itemData(i).toUInt() == ratio)
			currentIndex = i;

	m_ratioTimeAllowedToLiveComboBox->setCurrentIndex(currentIndex);
}

void InputGooseWidget::onClearButton()
{
	this->setPublisher(GoosePublisher());
	IncomingDataTableModel *model = dynamic_cast<IncomingDataTableModel*>(m_incomingDataView->model());
	if (!model)
		return;

	model->clear();
}

void InputGooseWidget::showEvent(QShowEvent * event)
{
	m_connectionDiscretWidget->updateList();
	QWidget::showEvent(event);
}
