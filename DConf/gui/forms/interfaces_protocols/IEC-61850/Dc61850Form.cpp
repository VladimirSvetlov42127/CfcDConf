#include "Dc61850Form.h"

#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qprocess.h>
#include <qapplication.h>
#include <qtabwidget.h>
#include <qtreeview.h>
#include <qmenu.h>
#include <qcombobox.h>
#include <qscrollarea.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <qjsonvalue.h>
#include <qsettings.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>
#include <dpc/gui/widgets/LineEdit.h>
#include <dpc/gui/widgets/SpinBox.h>
#include <dpc/gui/widgets/ComboBox.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/delegates/LineEditDelegate.h>
#include <dpc/gui/delegates/SpinBoxDelegate.h>

#include <gui/forms/interfaces_protocols/IEC-61850/61850_defines.h>
#include <gui/forms/interfaces_protocols/IEC-61850/IEC61850Configuration.h>
#include <gui/forms/interfaces_protocols/IEC-61850/TreeViewDataModel.h>
#include <gui/forms/interfaces_protocols/IEC-61850/InputGooseWidget.h>
#include <gui/forms/interfaces_protocols/IEC-61850/GoosePublishersDialog.h>
#include <gui/forms/interfaces_protocols/IEC-61850/GseTableModel.h>
#include <gui/forms/interfaces_protocols/IEC-61850/SvTableModel.h>
#include <gui/editors/EditorsManager.h>

using namespace Dpc::Gui;

namespace {

#ifdef QT_DEBUG
    const QString IEC61850_GENCONFIG_FILE = "./DConf/3rdparty/genconfig.jar";
#else
	const QString IEC61850_GENCONFIG_FILE = "genconfig.jar";
#endif

	const QString gSettingsGroup = "61850";
	const QString gDefaultFilePath = "/home";

	const QString inputGooseSettingsFile = "input_goose_settings.json";

	const QString JSON_GOOSELIST = "gooseList";
	const QString JSON_INTERFACE_NUM = "interfaceNum";
	const QString JSON_FILENAME = "fileName";
	const QString JSON_IED = "ied";
	const QString JSON_DEVICE = "device";
	const QString JSON_LN = "ln";
	const QString JSON_GSE = "gse";

	const int VLAN_ID_MIN = 0;
	const int VLAN_ID_MAX = 4095;	
	const int VLAN_PRIORITY_MIN = 0;
	const int VLAN_PRIORITY_MAX = 15;
	const int MINTIME_MIN = 1;
	const int MINTIME_MAX = 100;
	const int MAXTIME_MIN = 100;
	const int MAXTIME_MAX = 60000;

	const int32_t INDEX_MAC_ADDRESS_1 = 4;
	const int32_t INDEX_VLAN_ID_1 = 0;
	const int32_t INDEX_VLAN_PRIORITY_1 = 0;
	const int32_t INDEX_APPID_1 = 0;	
	const int32_t INDEX_MINTIME_1 = 6;
	const int32_t INDEX_MAXTIME_1 = 4;

	const int32_t INDEX_MAC_ADDRESS_2 = 10;
	const int32_t INDEX_VLAN_ID_2 = 8;
	const int32_t INDEX_VLAN_PRIORITY_2 = 8;
	const int32_t INDEX_APPID_2 = 1;	
	const int32_t INDEX_MINTIME_2 = 8;
	const int32_t INDEX_MAXTIME_2 = 7;

	const int32_t INDEX_GO_ENA_1 = 2;
	const int32_t INDEX_GO_ENA_2 = 4;
	const int32_t INDEX_CONFREV_1 = 0;
	const int32_t INDEX_GOOOSE_ID_1 = 0;
	const int32_t INDEX_COCBREF_1 = 0;
	const int32_t INDEX_GODATSET_1 = 0;
	const int32_t INDEX_CONFREV_2 = 1;
	const int32_t INDEX_GOOOSE_ID_2 = 1;
	const int32_t INDEX_COCBREF_2 = 1;
	const int32_t INDEX_GODATSET_2 = 1;

	const int32_t INDEX_INPUT_GOOSE_BASE = 2;
	const int32_t INDEX_INPUT_GOOSE_MAC_ADDRESS_BASE = 5;
	const int32_t INDEX_INPUT_GOOSE_VLAN_BASE = 1;
	const int32_t INDEX_INPUT_GOOSE_VLAN_PRIORITY_BASE = 1;
	const int32_t INDEX_INPUT_GOOSE_ACTIVE = 5;
	const int32_t INDEX_INPUT_GOOSE_LGOSMODE = 13;
	const int32_t INDEX_INPUT_GOOSE_SETTINGS_BASE = 9;

	const int32_t APPID_RANGE1_MIN = 0;
	const int32_t APPID_RANGE1_MAX = 0x3FFF;
	const int32_t APPID_RANGE2_MIN = 0x8000;
	const int32_t APPID_RANGE2_MAX = 0xBFFF;

	const uint32_t GOOSE_OUTPUT_INVALID_VALUE = 0xFFFFFFFF;

	struct GooseParamIndexs {
		int32_t goEna;
		int32_t confRev;
		int32_t id;
		int32_t coCbRef;
		int32_t goDatSet;
	};

	const QList<GooseParamIndexs> GooseParamIndexsList {
		{INDEX_GO_ENA_1, INDEX_CONFREV_1, INDEX_GOOOSE_ID_1, INDEX_COCBREF_1, INDEX_GODATSET_1},
		{INDEX_GO_ENA_2, INDEX_CONFREV_2, INDEX_GOOOSE_ID_2, INDEX_COCBREF_2, INDEX_GODATSET_2},
	};

	enum GooseEntrySrc_Type {
		DO_Din = 0,
		DO_Ain,
		DO_Cin,
		DO_Dout,
		DO_Aout,
		DO_DynData,
		DO_EtcLocDat,

		MaxAplDOtype,
		DO_Empty = 0xFF
	};

	enum GooseEntrySrc_Sel {
		DA_AllStructure = 0,		//структура: значение,качество,метка времени
		DA_value,			//только значение
		DA_quality,			//только качество
		DA_timestamp,		//только метка времени
	};

	enum GooseEntryCDC {
		CDC_SPS = 0,
		CDC_DPS,
		CDC_INS,
		CDC_MV_f,
		CDC_MV_i,
		CDC_SPC,
		CDC_DPC,
		CDC_BCR_64,

		CDC_UNKNOWN
	};

#pragma pack(push, 1)
	typedef struct {
		unsigned char DO_type : 4;
		unsigned char DA_sel : 4;
	} DO_DA_Type;

	typedef struct
	{
		DO_DA_Type Src;	/*Logical Node (parameter type ...*/
		uint8_t Rep;	/*representation  FCDA and DA pointer*/
		uint16_t Number;  /*parameter number */
	} GoosePublisherEntry_Type;

	struct FirstGoMsgUpdRules_Type { /*32 flags: 0-15 - configured rules; 16-31 work flags*/
		unsigned GoMsg_StaticDoutsCtrl_AfterRst : 1; /*Выполнение управления статическими выходами после рестарта*/
		unsigned GoMsg_StaticDoutsCtrl__AfterDisc : 1; /*Выполнение управления статическими выходами после восстановления соединения */
		unsigned GoMsg_ImpulseDoutsCtrl__AfterRst : 1; /*резерв*/
		unsigned GoMsg_ImpulseDoutsCtrl__AfterDisc : 1; /*резерв*/
		unsigned GoMsg_Use_TAL_Mult : 3; /*Коэффициент умножения принятого timeAllowedtoLive для определения рассоединения*/
		unsigned Reserve : 23; /*не задействовано*/
		unsigned GoLinkLoss : 1; /*не задействовано*/
		unsigned GoRestart : 1; /*не задействовано*/

		FirstGoMsgUpdRules_Type(uint32_t init = 0) { memcpy(this, &init, sizeof(FirstGoMsgUpdRules_Type)); }
		uint32_t toUint() const { uint32_t val; memcpy(&val, this, sizeof(FirstGoMsgUpdRules_Type)); return val; }
	};
#pragma pack(pop)

	GooseEntrySrc_Type gooseTypeFromString(const QString &s) {
		if (s == "Dins") return DO_Din;
		if (s == "Ains") return DO_Ain;
		if (s == "Cins") return DO_Cin;

		return DO_Empty;
	}

	GooseEntrySrc_Sel gooseSelFromString(const QString &s) {
		if (s.isEmpty()) return DA_AllStructure;
		if ((s.count() == 1 && s == "q") || (s.count() > 1 && s.endsWith(".q"))) return DA_quality;
		if ((s.count() == 1 && s == "t") || (s.count() > 1 && s.endsWith(".t"))) return DA_timestamp;	

		return DA_value;
	}

	GooseEntryCDC gooseCDCFromString(const QString &s) {
		if (s == "SPS")	return CDC_SPS;
		if (s == "DPS" || s == "DPC") return CDC_DPS;
		if (s == "INS")	return CDC_INS;
		if (s == "MV")	return CDC_MV_f;
		if (s == "BCR")	return CDC_BCR_64;

		return CDC_UNKNOWN;
	}

	class FcSelectorDialog : public QDialog
	{
		QComboBox *m_box;
	public:
		FcSelectorDialog(const QString &fcdaName, const QStringList &fcList, QWidget *parent = nullptr) :
			QDialog(parent),
			m_box(new QComboBox)
		{
			setWindowTitle("Выбор FC");

			m_box->addItems(fcList);

			QPushButton *okButton = new QPushButton("Ok", this);
			connect(okButton, &QPushButton::clicked, this, &QDialog::accept);			

			QGridLayout *layout = new QGridLayout(this);
			layout->addWidget(new QLabel(QString("Выберите FC для элемента %1").arg(fcdaName)), 0, 0, Qt::AlignCenter);
			layout->addWidget(m_box, 1, 0, Qt::AlignCenter);
			layout->addWidget(okButton, 2, 0, Qt::AlignCenter);
		}

		int selectedFc() const { return m_box->currentIndex(); }
	};
} // namespace

Dc61850Form::Dc61850Form(DcController *controller) :
	DcForm(controller,  "Настройки МЭК 61850", false),
	m_configuration(new IEC61850Configuration(this)),
	m_saveConfigButton(nullptr),
	m_exportCidButton(nullptr),
	m_mainTabWidget(nullptr),
	m_communicationTab(nullptr),
	m_iedTab(nullptr),
	m_svTab(nullptr),
	m_inputGooseLayout(nullptr),
	m_iedNameEdit(nullptr),
	m_dataModelView(nullptr),
	m_settingsView(nullptr),
	m_selectedItemWidget(nullptr),
	m_currentGooseWidget(new QWidget(this)),
	m_fileManager(controller),
	m_gseTableView(new TableView),
	m_svTableView(new TableView),
	m_svComboBox(new ComboBox)
{
	QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget());

	// кнопки управления
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	mainLayout->addLayout(buttonsLayout);

	QPushButton *loadIcdFileButton = new QPushButton("Загрузить icd файл");
	connect(loadIcdFileButton, &QPushButton::clicked, this, &Dc61850Form::onLoadIcdFileButton);
	buttonsLayout->addWidget(loadIcdFileButton);

	m_saveConfigButton = new QPushButton("Сохранить в конфигурацию");
	connect(m_saveConfigButton, &QPushButton::clicked, this, &Dc61850Form::onSaveConfigButton);
	buttonsLayout->addWidget(m_saveConfigButton);

	auto deleteConfigButton = new QPushButton("Удалить из конфигурации");
	buttonsLayout->addWidget(deleteConfigButton);
	connect(deleteConfigButton, &QPushButton::clicked, this, [=]() {
        if (auto param = controller->paramsRegistry().element(SP_MMSCFGFILE, 0); param)
			param->updateValue(QString());
	});

	m_exportCidButton = new QPushButton("Экспортировать конфигурацию в cid файл");
	connect(m_exportCidButton, &QPushButton::clicked, this, &Dc61850Form::onExportCidButton);
	buttonsLayout->addWidget(m_exportCidButton);	

	buttonsLayout->addStretch();

	// Tab коммуникация
	m_gseTableView->setModel(new GseTableModel(m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::MacAddrRow, new LineEditDelegate("\\01-\\0C-C\\D-\\01-HH-HH;_", m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::VlanIdRow, new IntSpinBoxDelegate(VLAN_ID_MIN, VLAN_ID_MAX, m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::VlanPriorityRow, new IntSpinBoxDelegate(VLAN_PRIORITY_MIN, VLAN_PRIORITY_MAX, m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::AppIdRow, new LineEditDelegate("\\0\\xHHHH;_", m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::MinTimeRow, new IntSpinBoxDelegate(MINTIME_MIN, MINTIME_MAX, m_gseTableView));
	m_gseTableView->setItemDelegateForRow(GseTableModel::MaxTimeRow, new IntSpinBoxDelegate(MAXTIME_MIN, MAXTIME_MAX, m_gseTableView));

	m_communicationTab = new QWidget;
	QVBoxLayout *communicationLayout = new QVBoxLayout(m_communicationTab);
	communicationLayout->addWidget(m_gseTableView);

	// Tab устройство
	m_iedNameEdit = new QLineEdit;
	connect(m_iedNameEdit, &QLineEdit::textEdited, this, [this](const QString &text) {
		auto ied = m_configuration->iedList().value(0);
		if (ied)
			ied->setName(text);
	});

	m_dataModelView = new QTreeView;
	m_dataModelView->setHeaderHidden(true);
	m_dataModelView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_dataModelView->setDragEnabled(true);
    m_dataModelView->setIconSize(QSize(24, 24));

	m_settingsView = new QTreeView;
	m_settingsView->setHeaderHidden(true);
	m_settingsView->setSelectionMode(QAbstractItemView::ExtendedSelection);
	m_settingsView->setAcceptDrops(true);
	m_settingsView->setDropIndicatorShown(true);
	m_settingsView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_settingsView->setIconSize(QSize(24, 24));
	connect(m_settingsView, &QTreeView::customContextMenuRequested, this, &Dc61850Form::onSettingsViewCustomContextMenuRequested);

	m_selectedItemWidget = new QWidget;

	m_iedTab = new QWidget;
	QGridLayout *iedLayout = new QGridLayout(m_iedTab);

	iedLayout->addWidget(new QLabel("Имя"), 0, 0);
	iedLayout->addWidget(m_iedNameEdit, 0, 1);
	iedLayout->addWidget(new QLabel("Модель данных"), 1, 0);
	iedLayout->addWidget(m_dataModelView, 2, 0, 2, 2);
	iedLayout->addWidget(new QLabel("Настройка"), 1, 2);
	iedLayout->addWidget(m_settingsView, 2, 2, 2, 3);
	iedLayout->addWidget(m_selectedItemWidget, 2, 5, 2, 2);

	iedLayout->setColumnStretch(4, 1);
	iedLayout->setColumnStretch(5, 2);
	iedLayout->setRowStretch(3, 1);

	// Tab входящие goose
	QComboBox *gooseComboBox = new QComboBox;
    auto inputGooseParam = controller->paramsRegistry().parameter(SP_INPUTITEM_DFN);
    auto portsParam = controller->paramsRegistry().parameter(SP_USARTPRTPAR_BYTE);
	if (inputGooseParam && portsParam)
        for (size_t i = 0; i < inputGooseParam->profilesCount(); i++) {
            InputGooseWidget *w = new InputGooseWidget(controller, portsParam->profilesCount(), i, this);
			connect(w, &InputGooseWidget::selectPublisher, this, &Dc61850Form::onGooseSelectPublisher);
			w->hide();
			m_inputGooseWidgets << w;
			gooseComboBox->insertItem(i, QString::number(i));
		}

	QWidget *inputGooseTab = new QWidget;
	m_inputGooseLayout = new QGridLayout(inputGooseTab);
	m_inputGooseLayout->addWidget(new QLabel("Номер издателя"), 0, 0);
	m_inputGooseLayout->addWidget(gooseComboBox, 0, 1);
	m_inputGooseLayout->addWidget(m_currentGooseWidget, 1, 0, 2, 3);
	m_inputGooseLayout->setRowStretch(2, 1);
	m_inputGooseLayout->setColumnStretch(2, 1);

	connect(gooseComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Dc61850Form::onGooseComboBoxIndexChanged);
	onGooseComboBoxIndexChanged(0);

	// Tab SV
    auto svParam = controller->paramsRegistry().element(SP_61850_SUPLIST, 2);
	if (svParam && svParam->value().toInt() > 0) {
		auto tableModel = new SvTableModel(this);
		connect(tableModel, &SvTableModel::dataChanged, this, &Dc61850Form::onSvModelDataChanged);

		m_svTableView->setModel(tableModel);

		auto workModeDelegate = new ComboBoxDelegate({ "Включен", "Блокирован", "Тест", "Блокирован в тесте", "Отключен" }, this);
		m_svTableView->setItemDelegateForRow(SvTableModel::WorkModeRow, workModeDelegate);
		m_svTableView->setItemDelegateForRow(SvTableModel::MacAddrRow, new LineEditDelegate("\\01-\\0C-C\\D-\\04-HH-HH;_", this));
		m_svTableView->setItemDelegateForRow(SvTableModel::VlanIdRow, new IntSpinBoxDelegate(VLAN_ID_MIN, VLAN_ID_MAX, this));
		m_svTableView->setItemDelegateForRow(SvTableModel::VlanPriorityRow, new IntSpinBoxDelegate(VLAN_PRIORITY_MIN, VLAN_PRIORITY_MAX, this));
		m_svTableView->setItemDelegateForRow(SvTableModel::AppIdRow, new LineEditDelegate("\\0\\xHHHH;_", this));
		m_svTableView->setItemDelegateForRow(SvTableModel::ConfRevRow, new IntSpinBoxDelegate(1, std::numeric_limits<int>::max(), this));

		m_nofAsduDelegate = new ComboBoxDelegate({ {"1", 1}, {"2", 2}, {"4", 4}, {"8", 8} }, this);
		m_nofAsduDelegate->setOrientation(Qt::Horizontal);
		m_svTableView->setItemDelegateForRow(SvTableModel::NofAsduRow, m_nofAsduDelegate);

		m_smpRateDelegate = new ComboBoxDelegate({ {"80", 80}, {"256", 256} }, this);
		m_smpRateDelegate->setOrientation(Qt::Horizontal);
		m_svTableView->setItemDelegateForRow(SvTableModel::SmpRateRow, m_smpRateDelegate);

		m_svTab = new QWidget;
		EditorsManager mg(nullptr, new QGridLayout(m_svTab));
		mg.addWidget(m_svComboBox, "Профиль");
		mg.addWidget(m_svTableView, 0, 1, 3);

		mg.layout()->setColumnStretch(2, 1);
	}

	// Настройка Tab'ов
	m_mainTabWidget = new QTabWidget;
	m_mainTabWidget->addTab(m_iedTab, "Устройство");
	m_mainTabWidget->addTab(m_communicationTab, "Коммуникация");
	m_mainTabWidget->addTab(inputGooseTab, "Входящие GOOSE");
	m_mainTabWidget->addTab(m_svTab, "SV");
	mainLayout->addWidget(m_mainTabWidget);	

	// Загрузка данных из файла
	setEnableEditing(false);
    auto param = controller->paramsRegistry().element(SP_MMSCFGFILE, 0);
	if (param && !param->value().isEmpty()) {
		QString fileName = QString("%1/%2.cid").arg(m_fileManager.localPath()).arg(QFileInfo(param->value()).completeBaseName());
		if (QFileInfo::exists(fileName))
			loadConfigFromFile(fileName);
	}

	loadInputGooseSettings();

	if (!checkJavaInstalled())
		MsgBox::warning("Для генерации файла конфигурации МЭК 61850, уставноите java");
}

Dc61850Form::~Dc61850Form()
{
}

bool Dc61850Form::isAvailableFor(DcController * controller)
{
    if (!controller->hasEthernet())
        return false;

    if (auto p = controller->paramsRegistry().parameter(SP_61850_SUPLIST); p) {
        for (size_t i = 0; i < p->elementsCount(); i++)
            if (controller->paramsRegistry().element(SP_61850_SUPLIST, i)->value().toInt())
                return true;

        return false;
    }

	static QList<Param> params = {
		{SP_EXCHANGESETTINGS},
		{SP_MMSCFGFILE}
	};

	return hasAll(controller, params);
}

void Dc61850Form::fillReport(DcIConfigReport * report)
{
}

void Dc61850Form::onLoadIcdFileButton()
{
	QSettings settings;
	auto filePathKey = QString("%1/IcdFilePath").arg(gSettingsGroup);
	auto dir = settings.value(filePathKey, gDefaultFilePath).toString();

	QString fileName = QFileDialog::getOpenFileName(this, "Выбрать файл", dir, "Файл конфигурации (*.icd *.cid)");
	if (fileName.isEmpty())
		return;
	
	settings.setValue(filePathKey, QFileInfo(fileName).absolutePath());

	loadConfigFromFile(fileName);		
}

void Dc61850Form::onSaveConfigButton()
{
	bool result = true;
	result &= saveConfig();
	result &= saveInputGooseSettings();
	if (!result)
		return;

	MsgBox::info("Изменения сохраненны в конфигурацию");
}

void Dc61850Form::onExportCidButton()
{
	QSettings settings;
	auto filePathKey = QString("%1/ExportFilePath").arg(gSettingsGroup);
	auto dir = settings.value(filePathKey, gDefaultFilePath).toString();

	QString fileName = QFileDialog::getSaveFileName(this, "Сохранить в файл", dir, "*.cid");
	if (fileName.isEmpty())
		return;

	settings.setValue(filePathKey, QFileInfo(fileName).absolutePath());

	if (!m_configuration->save(fileName)) {
		MsgBox::error(QString("Не удалось сохранить конфигурацию 61850 в файл %1: %2")
			.arg(fileName)
			.arg(m_configuration->errorString()));
		return;
	}
}

void Dc61850Form::onSettingsViewCustomContextMenuRequested(const QPoint &pos)
{
	TreeViewDataModel *model = dynamic_cast<TreeViewDataModel*>(m_settingsView->model());
	if (!model)
		return;

	QModelIndex index = m_settingsView->indexAt(pos);
	MenuPtr menu = model->contextMenuForIndex(index, m_settingsView->selectionModel()->selectedIndexes());
	if (menu) {
		QAction *act = menu->exec(m_settingsView->viewport()->mapToGlobal(pos));
		if (!act)
			return;

		if (act->data().toInt() == TreeViewDataModel::AddOperation)
			m_settingsView->expand(index);
		if (act->data().toInt() == TreeViewDataModel::RemoveOperation)
			clearSelectedItemWidget();
	}
}

void Dc61850Form::onSettingsViewSelected(const QModelIndex &current, const QModelIndex &previous)
{
	clearSelectedItemWidget();
	if (!current.isValid())
		return;	

	// получаем список датасетов родителя(ln)
	BaseTreeItem *item = static_cast<BaseTreeItem*>(current.internalPointer());
	QList<DataSetPtr> dataSetList;
	if (item->parent() && item->parent()->itemType() == BaseTreeItem::LNType)
		dataSetList = static_cast<LNTreeItem*>(item->parent())->ln()->dataSets();
	
	// строим виджет в зависимости от выбраного элемента
	if (item->itemType() == BaseTreeItem::GSEType) {
		GSEControlPtr p = static_cast<GSEControlTreeItem*>(item)->gseControl();

		auto appIdEdit = new QLineEdit;
		connect(appIdEdit, &QLineEdit::textEdited, this, [=](const QString &text) { p->setAppID(text); });
		appIdEdit->setText(p->appID());

		auto confRevEdit = new IntSpinBox;
		confRevEdit->setRange(1, INT32_MAX);
		confRevEdit->setValue(p->confRev());
		connect(confRevEdit, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) { p->setConfRev(value); });

		auto datSetComboBox = new ComboBox;
		for (auto &ds: dataSetList)
			datSetComboBox->addItem(ds->name());
		datSetComboBox->setCurrentIndex(-1);
		for (size_t i = 0; i < dataSetList.count(); i++)
			if (dataSetList.at(i) == p->datSet())
				datSetComboBox->setCurrentIndex(i);		
		connect(datSetComboBox, QOverload<int>::of(&ComboBox::currentIndexChanged), this, [=](int idx) {
			p->setDatSet(dataSetList.value(idx));
		});				

		auto goEnaCheckBox = new QCheckBox;
		connect(goEnaCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setGoEna(state == Qt::Checked); });
		goEnaCheckBox->setChecked(p->goEna());

		EditorsManager mg(nullptr, new QGridLayout(m_selectedItemWidget));
		mg.addWidget(appIdEdit, "Идентификатор приложения");
		mg.addWidget(confRevEdit, "Номер ревизии");
		mg.addWidget(datSetComboBox, "Датасет");
		mg.addWidget(goEnaCheckBox, "GoEna");

		mg.addStretch();	
	}
	if (item->itemType() == BaseTreeItem::RType) {
		ReportControlPtr p = static_cast<ReportControlTreeItem*>(item)->report();

		QLineEdit *rptIdEdit = new QLineEdit;
		connect(rptIdEdit, &QLineEdit::textEdited, this, [=](const QString &text) { p->setRptID(text); });
		rptIdEdit->setText(p->rptID());

		auto confRevEdit = new IntSpinBox;
		confRevEdit->setRange(1, INT32_MAX);
		connect(confRevEdit, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) { p->setConfRev(value); });
		confRevEdit->setValue(p->confRev());		

		auto datSetComboBox = new ComboBox;
		for (auto &ds : dataSetList)
			datSetComboBox->addItem(ds->name());
		datSetComboBox->setCurrentIndex(-1);
		for (size_t i = 0; i < dataSetList.count(); i++)
			if (dataSetList.at(i) == p->datSet())
				datSetComboBox->setCurrentIndex(i);
		connect(datSetComboBox, QOverload<int>::of(&ComboBox::currentIndexChanged), this, [=](int idx) {
			p->setDatSet(dataSetList.value(idx));
		});

		auto bufferedCheckBox = new QCheckBox;
		connect(bufferedCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setBuffered(state == Qt::Checked); });
		bufferedCheckBox->setChecked(p->buffered());

		auto intgPdEdit = new IntSpinBox;
		intgPdEdit->setRange(0, INT32_MAX);
		connect(intgPdEdit, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) { p->setIntgPd(value); });
		intgPdEdit->setValue(p->intgPd());

		auto bufTimeEdit = new IntSpinBox;
		bufTimeEdit->setRange(0, INT32_MAX);
		connect(bufTimeEdit, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) { p->setBufTime(value); });
		bufTimeEdit->setValue(p->bufTime());

		auto dchgCheckBox = new QCheckBox;
		connect(dchgCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setDchg(state == Qt::Checked); });
		dchgCheckBox->setChecked(p->dchg());

		auto qchgCheckBox = new QCheckBox;
		connect(qchgCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setQchg(state == Qt::Checked); });
		qchgCheckBox->setChecked(p->qchg());

		auto dupdCheckBox = new QCheckBox;
		connect(dupdCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setDupd(state == Qt::Checked); });
		dupdCheckBox->setChecked(p->dupd());

		auto periodCheckBox = new QCheckBox;
		connect(periodCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setPeriod(state == Qt::Checked); });
		periodCheckBox->setChecked(p->period());

		auto seqNumCheckBox = new QCheckBox;
		connect(seqNumCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setSeqNum(state == Qt::Checked); });
		seqNumCheckBox->setChecked(p->seqNum());

		auto timeStampCheckBox = new QCheckBox;
		connect(timeStampCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setTimeStamp(state == Qt::Checked); });
		timeStampCheckBox->setChecked(p->timeStamp());

		auto dataSetCheckBox = new QCheckBox;
		connect(dataSetCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setDataSet(state == Qt::Checked); });
		dataSetCheckBox->setChecked(p->dataSet());

		auto reasonCodeCheckBox = new QCheckBox;
		connect(reasonCodeCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setReasonCode(state == Qt::Checked); });
		reasonCodeCheckBox->setChecked(p->reasonCode());

		auto dataRefCheckBox = new QCheckBox;
		connect(dataRefCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setDataRef(state == Qt::Checked); });
		dataRefCheckBox->setChecked(p->dataRef());

		auto entryIDCheckBox = new QCheckBox;
		connect(entryIDCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setEntryID(state == Qt::Checked); });
		entryIDCheckBox->setChecked(p->entryID());

		auto configRefCheckBox = new QCheckBox;
		connect(configRefCheckBox, &QCheckBox::stateChanged, this, [=](int state) { p->setConfigRef(state == Qt::Checked); });
		configRefCheckBox->setChecked(p->configRef());

		auto maxEdit = new IntSpinBox;
		maxEdit->setRange(0, INT32_MAX);
		connect(maxEdit, qOverload<int>(&IntSpinBox::valueChanged), this, [=](int value) { p->setMaxRpt(value); });
		maxEdit->setValue(p->maxRpt());

		EditorsManager mg(nullptr, new QGridLayout(m_selectedItemWidget));
		mg.addWidget(rptIdEdit, "Идентификатор");
		mg.addWidget(confRevEdit, "Номер ревизии");
		mg.addWidget(datSetComboBox, "Датасет");
		mg.addWidget(bufferedCheckBox, "Буферизированный");
		mg.addWidget(bufTimeEdit, "Время буферизации (мс)");
		mg.addWidget(intgPdEdit, "Период выдачи (мс)");
		mg.addWidget(dchgCheckBox, "dchg");
		mg.addWidget(qchgCheckBox, "qchg");
		mg.addWidget(dupdCheckBox, "dupd");
		mg.addWidget(periodCheckBox, "period");
		mg.addWidget(seqNumCheckBox, "seq num");
		mg.addWidget(timeStampCheckBox, "time stamp");
		mg.addWidget(dataSetCheckBox, "dataset");
		mg.addWidget(reasonCodeCheckBox, "reason code");
		mg.addWidget(dataRefCheckBox, "data ref");
		mg.addWidget(entryIDCheckBox, "entry ID");
		mg.addWidget(configRefCheckBox, "config ref");
		mg.addWidget(maxEdit, "Максимальное число блоков");

		mg.addStretch();
	}
}

void Dc61850Form::onGooseComboBoxIndexChanged(int idx)
{
	InputGooseWidget* next = m_inputGooseWidgets.value(idx);
	if (!next)
		return;

	m_inputGooseLayout->replaceWidget(m_currentGooseWidget, next);
	m_currentGooseWidget->hide();
	m_currentGooseWidget = next;
	m_currentGooseWidget->show();
}

void Dc61850Form::onGooseSelectPublisher()
{
	InputGooseWidget *w = dynamic_cast<InputGooseWidget*>(sender());
	if (!w)
		return;

	QList<GoosePublisher> publishersList;
	for (auto &w : m_inputGooseWidgets)
		if (w->publisher().config)
			publishersList << w->publisher();

	GoosePublishersDialog dlg(publishersList);
	if (dlg.exec() == QDialog::Rejected)
		return;

	w->setPublisher(dlg.selectedPublisher());
}

void Dc61850Form::onSvComboBoxChanged()
{
	auto ied = m_configuration->iedList().value(0);
	if (!ied)
		return;

	auto profile = m_svComboBox->currentText();
	auto ln0 = ied->device("SV1")->ln0();
	if (ln0)
		while (!ln0->svControls().isEmpty()) ln0->removeSV(0);

	ln0 = ied->device("SV2")->ln0();
	if (ln0)
		while (!ln0->svControls().isEmpty()) ln0->removeSV(0);

	auto connAP = m_configuration->communication()->subNetworkList().value(0)->connectedAPList().value(0);
	if (connAP)
		while (!connAP->smvList().isEmpty())
			connAP->removeSMV(0);

	auto words = profile.split('_');
	if (words.size() < 3)
		return;

	words.takeFirst();
	words.takeFirst();

	QStringList datSets;
	datSets << words.takeFirst().trimmed();
	if (!words.isEmpty())
		datSets << words.takeFirst().trimmed();

	QList<SVControlPtr> svList;
	for (size_t i = 0; i < datSets.size(); i++) {
		QString cbName = QString("%1_%2").arg(profile).arg(i + 1);
		QString device = QString("SV%1").arg(i + 1);
		auto ln0 = ied->device(device)->ln0();
		if (!ln0)
			return;

		auto ds = ln0->dataSet(datSets.at(i));
		if (!ds)
			return;

		auto smv = connAP->addSMV(cbName, device);
		auto svControl = ln0->addSV(cbName, ds);
		svControl->setSmvAP(smv);
		svList << svControl;
	}

	updateSVControlsEditor(svList);	
}

void Dc61850Form::onSvModelDataChanged(const QModelIndex& index)
{
	if (index.row() != SvTableModel::SmpRateRow)
		return;

	auto value = index.data().toInt();
	if (value > 80)
		m_nofAsduDelegate->setItems({ {"4", 4}, {"8", 8} }, index.column());
	else
		m_nofAsduDelegate->removeSection(index.column());

	auto tableModel = static_cast<SvTableModel*>(m_svTableView->model());
	for (int i = 1; i < tableModel->columnCount(); i++) {
		if (i == index.column())
			continue;
		if (value > 80)
			m_smpRateDelegate->setItems({ {"80", 80} }, i);
		else
			m_smpRateDelegate->removeSection(i);
	}
}

bool Dc61850Form::updateParamValue(int32_t addr, int32_t index, const QString & value)
{
    auto p = controller()->paramsRegistry().element(addr, index);
	if (!p) {
		MsgBox::error(QString("Не удалось обновить значение параметра(%1:%2): параметр не найден в шаблоне устройства")
			.arg(QString("0x%1").arg(QString("%1").arg(addr, 4, 16, QChar('0')).toUpper()))
			.arg(index));
		return false;
	}

	p->updateValue(value);
	return true;
}

void Dc61850Form::loadConfigFromFile(const QString &fileName)
{	
	if (!m_configuration->load(fileName)) {
		MsgBox::error(QString("Не удалось загрузить конфигурацию из файла %1: %2")
			.arg(fileName)
			.arg(m_configuration->errorString()));
		return;
	}

	setEnableEditing(true);
	m_mainTabWidget->setCurrentWidget(m_iedTab);
	clearSelectedItemWidget();	

	QList<GSEPtr> gseList = m_configuration->communication() ? m_configuration->communication()->allGSEList() : QList<GSEPtr>();
	static_cast<GseTableModel*>(m_gseTableView->model())->setGseList(gseList);
	m_gseTableView->resizeColumnsToContents();

	auto ied = m_configuration->iedList().value(0);
	m_iedNameEdit->setText(ied ? ied->name() : QString());

	auto fcSelector = [=](const QString &fcdaName, const QStringList &fcList) {
		FcSelectorDialog dlg(fcdaName, fcList, this);
		if (QDialog::Rejected == dlg.exec())
			return -1;

		return dlg.selectedFc();
	};

	delete m_dataModelView->model();
	m_dataModelView->setModel(new TreeViewDataModel(ied, BaseTreeItem::DataModelMode, fcSelector, this));

	delete m_settingsView->model();
	m_settingsView->setModel(new TreeViewDataModel(ied, BaseTreeItem::SetupMode, fcSelector, this));
	connect(m_settingsView->selectionModel(), &QItemSelectionModel::currentChanged, this, &Dc61850Form::onSettingsViewSelected);

	if (!m_svTab)
		return;

	disconnect(m_svComboBox, qOverload<int>(&ComboBox::currentIndexChanged), this, &Dc61850Form::onSvComboBoxChanged);
	m_svComboBox->clear();
	if (auto subNetwork = m_configuration->communication()->subNetworkList().value(0); subNetwork)
		if (auto connectedAP = subNetwork->connectedAPList().value(0); connectedAP)
			m_svComboBox->addItems(connectedAP->supportedSMV());

	QList<SVControlPtr> svList;
	QStringList datSets;
	auto getSvAndDatSet = [&](const QString& device) {
		if (auto sv = ied->svControlList(device).value(0); sv && sv->smvAP()) {
			svList << sv;
			if (!sv->datSet().isEmpty())
				datSets << sv->datSet();
		}
	};

	getSvAndDatSet("SV1");
	getSvAndDatSet("SV2");
	auto profile = QString("MSVCB_%1").arg(datSets.size());
	for (auto& ds : datSets)
		profile.append(QString("_%1").arg(ds));
	m_svComboBox->setCurrentIndex(-1);
	m_svComboBox->setCurrentText(profile);
	connect(m_svComboBox, qOverload<int>(&ComboBox::currentIndexChanged), this, &Dc61850Form::onSvComboBoxChanged);;

	updateSVControlsEditor(svList);
}

void Dc61850Form::setEnableEditing(bool enable)
{
	//m_saveConfigButton->setEnabled(enable);
	m_exportCidButton->setEnabled(enable);
	for (size_t i = 0; i < m_mainTabWidget->count(); i++) {
		QWidget *w = m_mainTabWidget->widget(i);
		if (w == m_communicationTab || w == m_iedTab || w == m_svTab)
			m_mainTabWidget->setTabEnabled(i, enable);
	}
}

bool Dc61850Form::saveConfig()
{
	// Если конфигурация не загружена, нечего сохранять.
	if (!m_configuration->isLoaded())
		return true;

	// Проверка на коректность 
	if (!isConfigValid())
		return false;

	// сохраянем необходимые значения параметров
	saveConfigParams();

	// создаем папку для 61850
	QString iec61850Path = m_fileManager.localPath();
	if (!QDir(iec61850Path).mkpath(".")) {
		MsgBox::error(QString("Не удалось создать папку %1").arg(iec61850Path));
		return false;
	}

	// Вычисляем путь к выходному cid файлу, если существует файл с таким же именем, удаляем его.
	QString cidFileName = QString("%1/%2.cid").arg(iec61850Path).arg(IEC61850::FILE_BASE_NAME);
	if (QFile::exists(cidFileName))
		QFile::remove(cidFileName);

	// сохраняем конфигурацию в cid файл
	if (!m_configuration->save(cidFileName)) {
		MsgBox::error(QString("Не удалось сохранить конфигурацию 61850 в файл %1: %2")
			.arg(cidFileName)
			.arg(m_configuration->errorString()));
		return false;
	}

	// запускаем программу генерации cfg файла и подаем на вход cid файл.
	QProcess process;
	QByteArray errorBuffer;
	connect(&process, &QProcess::readyReadStandardError, this, [&process, &errorBuffer]() { errorBuffer.append(process.readAllStandardError()); });
	QByteArray outputBuffer;
	connect(&process, &QProcess::readyReadStandardOutput, this, [&process, &outputBuffer]() { outputBuffer.append(process.readAllStandardOutput()); });

	// запуск программы 	
	QStringList arguments{ "-jar", IEC61850_GENCONFIG_FILE, cidFileName };
	QApplication::setOverrideCursor(Qt::WaitCursor);
	process.start("java", arguments);
	process.waitForFinished();
	QApplication::restoreOverrideCursor();

	// обработка результата работы внешней программы
	if (process.exitStatus() != QProcess::NormalExit) {
		MsgBox::error("Внешняя программа завершилась крахом");
		return false;
	}
	if (process.error() != QProcess::UnknownError) {
		MsgBox::error(QString("При выполнении внешней программы возникла ошибка(%1): %2").arg(process.error()).arg(process.errorString()));
		return false;
	}
	if (process.exitCode() || !errorBuffer.isEmpty()) {
		MsgBox::error(QString("Внешняя программа завершилась с кодом(%1): %2").arg(process.exitCode()).arg(QString(errorBuffer)));
		return false;
	}

	// обрезаем ненужную чать
	int start = outputBuffer.indexOf("MODEL(");
	if (start > 0)
		outputBuffer = outputBuffer.mid(start);

	// записываем резултат в cfg файл
	QString cfgFileName = QString("%1/%2.cfg").arg(iec61850Path).arg(QFileInfo(cidFileName).completeBaseName());
	QFile cfgFile(cfgFileName);
	if (!cfgFile.open(QIODevice::WriteOnly) || cfgFile.write(outputBuffer) < 0) {
		MsgBox::error(QString("Не удалось записать файл(%1): %2").arg(cfgFileName).arg(cfgFile.errorString()));
		return false;
	}

	// записать путь к файлу в параметр
    auto param = controller()->paramsRegistry().element(SP_MMSCFGFILE, 0);
	if (!param) {
		MsgBox::error(QString("Не найден параметр 0x%1").arg(QString::number(SP_MMSCFGFILE, 16).toUpper()));
		return false;
	}

	QString pathInDevice = m_fileManager.devicePath(QFileInfo(cfgFileName).fileName());
	param->updateValue(pathInDevice);
	return true;
}

bool Dc61850Form::saveConfigParams()
{
	bool result = true;
	auto subNetwork = m_configuration->communication()->subNetworkList().value(0);
	if (subNetwork) {
		auto connectedAP = subNetwork->connectedAPList().value(0);
		if (connectedAP) {
            auto ipParam = controller()->paramsRegistry().element(SP_IP4_ADDR, 0);
			if (ipParam)
				connectedAP->setIpAddress(ipParam->value());

            auto subNet = controller()->paramsRegistry().element(SP_IP4_NETMASK, 0);
			if (subNet)
				connectedAP->setSubnetAddress(subNet->value());

            auto gateway = controller()->paramsRegistry().element(SP_IP4_GW_ADDR, 0);
			if (gateway)
				connectedAP->setGatewayAddress(gateway->value());
		}
	}

	auto gseList = m_configuration->communication()->allGSEList();
	if (gseList.count() > 0) {
		GSEPtr g = gseList.at(0);
		result &= updateParamValue(SP_MAC_ADDR, INDEX_MAC_ADDRESS_1, g->macAddress().replace("-", ":"));
		result &= updateParamValue(SP_VLAN_TAG, INDEX_VLAN_ID_1, g->vlanId());
		result &= updateParamValue(SP_VLAN_PRIO, INDEX_VLAN_PRIORITY_1, g->vlanPriority());
		result &= updateParamValue(SP_PROTPARS_WORD, INDEX_APPID_1, QString::number(g->appID().toInt(nullptr, 16)) );
		result &= updateParamValue(SP_EXCHANGESETTINGS, INDEX_MINTIME_1, g->minTime());
		result &= updateParamValue(SP_EXCHANGESETTINGS, INDEX_MAXTIME_1, g->maxTime());
	}

	if (gseList.count() > 1) {
		GSEPtr g = gseList.at(1);
		result &= updateParamValue(SP_MAC_ADDR, INDEX_MAC_ADDRESS_2, g->macAddress().replace("-", ":"));
		result &= updateParamValue(SP_VLAN_TAG, INDEX_VLAN_ID_2, g->vlanId());
		result &= updateParamValue(SP_VLAN_PRIO, INDEX_VLAN_PRIORITY_2, g->vlanPriority());
		result &= updateParamValue(SP_PROTPARS_WORD, INDEX_APPID_2, QString::number(g->appID().toInt(nullptr, 16)) );
		result &= updateParamValue(SP_EXCHANGESETTINGS, INDEX_MINTIME_2, g->minTime());
		result &= updateParamValue(SP_EXCHANGESETTINGS, INDEX_MAXTIME_2, g->maxTime());
	}

	// очищаем исходящие goose
	for(auto &it: GooseParamIndexsList)
		result &= updateParamValue(SP_TESTSEND, it.goEna, "0");
    auto outputGooseParam = controller()->paramsRegistry().parameter(SP_OUTPUTITEM_DFN);
	if (outputGooseParam) {
        for (size_t i = 0; i < outputGooseParam->profilesCount(); i++)
            for (size_t j = 0; j < outputGooseParam->elementsCount(); j++)
				result &= updateParamValue(SP_OUTPUTITEM_DFN, i * PROFILE_SIZE + j, QString::number(GOOSE_OUTPUT_INVALID_VALUE));
	}

	// если нет конфигурации устройства, выходим.
	auto ied = m_configuration->iedList().value(0);
	if (!ied)
		return result;

	// заполняем исходящие goose
	auto fillGooseOutputParam = [=](const DataSetPtr &ds, int gooseProfile) -> bool {
		if (!ds || !outputGooseParam)
			return true;

        int maxCount = outputGooseParam->elementsCount();
		bool result = true;
		int32_t count = 0;			
		int32_t indexBase = gooseProfile * PROFILE_SIZE;
		for (auto &fcda : ds->fcdaList()) {
			auto type = gooseTypeFromString(fcda->prefix());
			if (type != DO_Din && type != DO_Ain && type != DO_Cin)
				continue;

			auto device = ied->device(fcda->ldInst());
			if (!device)
				continue;

			auto ln = device->ln(fcda->prefix(), fcda->lnClass(), fcda->lnInst());
			if (!ln)
				continue;

			auto doPtr = ln->type()->getDO(fcda->doName());
			if (!doPtr)
				continue;

			auto cdc = gooseCDCFromString(doPtr->type()->cdc());
			if (cdc == CDC_UNKNOWN)
				continue;

			GoosePublisherEntry_Type st;
			st.Src.DO_type = type;
			st.Src.DA_sel = gooseSelFromString(fcda->daName());
			st.Rep = cdc;
			st.Number = ln->type()->doList().indexOf(doPtr) - 4;
			uint32_t val;
			memcpy(&val, &st, sizeof(st));

			result &= updateParamValue(SP_OUTPUTITEM_DFN, indexBase + count, QString::number(val));
			count++;
			if (count >= maxCount)
				break;
		}

		return result;
	};

	QString iedName = ied->name();
	QString goCbRefTemplate = QString("%1%2/LLN0$GO$%3").arg(iedName);
	QString goDatSetTemplate = QString("%1%2/LLN0$%3").arg(iedName);
	int gooseNum = 0;
	for(auto &dev: ied->lDevices())
		for(auto &ln: dev->lnList())
			for (auto &gseControl : ln->gseControls()) {
				if (gooseNum >= GooseParamIndexsList.count())
					continue;

				auto gpi = GooseParamIndexsList.at(gooseNum);
				result &= updateParamValue(SP_TESTSEND, gpi.goEna, gseControl->goEna() ? "1" : "0");
				result &= updateParamValue(SP_PROTPARS_DWORD, gpi.confRev, QString::number(gseControl->confRev()));
				result &= updateParamValue(SP_PROTPARS_STRING, gpi.id, gseControl->appID());
				result &= updateParamValue(SP_GOCBREF_NAM, gpi.coCbRef, goCbRefTemplate.arg(dev->inst(), gseControl->name()));
				result &= updateParamValue(SP_GODATASET_NAM, gpi.goDatSet, goDatSetTemplate.arg(dev->inst(), gseControl->datSet()->name()));
				result &= fillGooseOutputParam(gseControl->datSet(), gooseNum);
				gooseNum++;
			}

	QList<SVControlPtr> svList;
	auto svcb = ied->svControlList("SV1").value(0);
	if (svcb) {
		svList << svcb;
		svcb = ied->svControlList("SV2").value(0);
		if (svcb)
			svList << svcb;
	}

	for(size_t i = 0; i < svList.size(); i++) {
		auto sv = svList.at(i);
		result &= updateParamValue(SP_CONFREV_SV, i, QString::number(sv->confRev()));
		result &= updateParamValue(SP_AIN_LONG_MODE_NAME, i, sv->smvID());
		result &= updateParamValue(SP_STREAM_SYNC, i, sv->smpRate() == 80 ? "0" : "1");
		result &= updateParamValue(SP_EXCHANGE_INTERFACE, i, QString::number(sv->nofASDU()));
		result &= updateParamValue(SP_MODE_SV, i, QString::number(sv->workMode()));

		result &= updateParamValue(SP_MAC_ADDR, 10 + i, sv->smvAP()->macAddress().replace("-", ":"));
		result &= updateParamValue(SP_VLAN_TAG, 8 + i, sv->smvAP()->vlanId());
		result &= updateParamValue(SP_VLAN_PRIO, 8 + i, sv->smvAP()->vlanPriority());
		result &= updateParamValue(SP_ASDU, i, QString::number(sv->smvAP()->appID().toInt(nullptr, 16)));		
	}

	return result;
}

bool Dc61850Form::isConfigValid() const
{
	auto gseList = m_configuration->communication()->allGSEList();
	for (size_t i = 0; i < gseList.count(); i++) {
		auto gse = gseList.at(i);
		bool good = true;
		int value = gse->appID().toInt(&good, 16);
		if (!good)
			continue;

		bool inFirstRange = APPID_RANGE1_MIN <= value && value <= APPID_RANGE1_MAX;
		bool inSecondRange = APPID_RANGE2_MIN <= value && value <= APPID_RANGE2_MAX;
		if (!inFirstRange && !inSecondRange) {
			MsgBox::error(QString("Идентификатор приложения %1, не входит в диапазон допустимых значений: %2-%3, %4-%5")
				.arg(QString("%1").arg(value, 4, 16, QChar('0')))
				.arg(QString("%1").arg(APPID_RANGE1_MIN, 4, 16, QChar('0')))
				.arg(QString("%1").arg(APPID_RANGE1_MAX, 4, 16, QChar('0')))
				.arg(QString("%1").arg(APPID_RANGE2_MIN, 4, 16, QChar('0')))
				.arg(QString("%1").arg(APPID_RANGE2_MAX, 4, 16, QChar('0'))));

			return false;
		}
	}

	auto ied = m_configuration->iedList().value(0);
	if (!ied)
		return true;

	for(auto &device: ied->lDevices())
		for (auto &ln : device->lnList()) {
			QString msg = QString("В настройках '%1/%2/%3' не указан датасет").arg(device->inst(), ln->name());
			for(auto &rp: ln->reports())
				if (!rp->datSet()) {
					MsgBox::error(msg.arg(rp->name()));
					return false;
				}
			for(auto &gse: ln->gseControls())
				if (!gse->datSet()) {
					MsgBox::error(msg.arg(gse->name()));
					return false;
				}
		}

	return true;
}

bool Dc61850Form::loadInputGooseSettings()
{
	for (size_t i = 0; i < m_inputGooseWidgets.count(); i++) {
		auto inputGooseWidget = m_inputGooseWidgets.at(i);

        auto param = controller()->paramsRegistry().element(SP_TESTSEND, INDEX_INPUT_GOOSE_ACTIVE + i );
		if (param)
			inputGooseWidget->setActive(param->value().toUInt() ? true : false);

        param = controller()->paramsRegistry().element(SP_TESTSEND, INDEX_INPUT_GOOSE_LGOSMODE + i);
		if (param)
			inputGooseWidget->setLgosMode(param->value().toUInt());

        param = controller()->paramsRegistry().element(SP_EXCHANGESETTINGS, INDEX_INPUT_GOOSE_SETTINGS_BASE + i);
		if (param) {
			FirstGoMsgUpdRules_Type st(param->value().toUInt());
			inputGooseWidget->setControlAfterRestart(st.GoMsg_StaticDoutsCtrl_AfterRst);
			inputGooseWidget->setControlAfterReconnection(st.GoMsg_StaticDoutsCtrl__AfterDisc);
			inputGooseWidget->setRatioTimeAllowedToLive(st.GoMsg_Use_TAL_Mult);
		}

        auto inputGooseParam = controller()->paramsRegistry().parameter(SP_INPUTITEM_DFN);
		if (inputGooseParam) {
			QList<GooseSubscriberEntryCfg> dataList;
            for (size_t j = 0; j < inputGooseParam->elementsCount(); j++) {
                auto param = controller()->paramsRegistry().element(SP_INPUTITEM_DFN, i * PROFILE_SIZE + j);
				if (!param)
					continue;
				dataList << GooseSubscriberEntryCfg{ param->value().toUInt() };
			}

			inputGooseWidget->setIncomingDataList(dataList);
		}
	}

	QString iec61850Path = m_fileManager.localPath();
	QString inputGooseSettingsFileName = QString("%1/%2").arg(iec61850Path).arg(inputGooseSettingsFile);
	if (!QFileInfo::exists(inputGooseSettingsFileName))
		return true;

	QFile file(inputGooseSettingsFileName);
	if (!file.open(QIODevice::ReadOnly)) {
		MsgBox::error(QString("Не удалось открыть файл %1: %2").arg(inputGooseSettingsFileName, file.errorString()));
		return false;
	}

	QByteArray fileData = file.readAll();
	if (fileData.isEmpty())
		return true;

	QJsonParseError jsonError;
	QJsonDocument doc = QJsonDocument::fromJson(fileData, &jsonError);
	if (jsonError.error != QJsonParseError::NoError) {
		MsgBox::error(QString("Не удалось разобрать файл %1: %2").arg(inputGooseSettingsFileName, jsonError.errorString()));
		return false;
	}

	QHash<QString, IEC61850ConfigurationPtr> loadedFiles;
	QJsonArray gooseList = doc.object()[JSON_GOOSELIST].toArray(); 
	for (size_t i = 0; i < gooseList.size(); i++) {
		QJsonObject obj = gooseList[i].toObject();

		if (!obj.contains(JSON_INTERFACE_NUM))
			continue;
		int interfaceNum = obj[JSON_INTERFACE_NUM].toInt();
		InputGooseWidget *widget = m_inputGooseWidgets.value(interfaceNum);
		if (!widget)
			continue;

		QString fileName = obj[JSON_FILENAME].toString();
		if (fileName.isEmpty())
			continue;

		QString absoluteFileName = QString("%1/%2").arg(iec61850Path, fileName);
		auto it = loadedFiles.find(absoluteFileName);
		if (it == loadedFiles.end()) {
			auto config = std::make_shared<IEC61850Configuration>();
			if (!config->load(absoluteFileName)) {
				MsgBox::error(QString("Не удалось загрузить конфигурацию из файла %1: %2")
					.arg(absoluteFileName)
					.arg(config->errorString()));
				continue;
			}

			it = loadedFiles.insert(absoluteFileName, config);
		}
		
		auto config = it.value();
		auto ied = config->ied(obj[JSON_IED].toString());
		if (!ied)
			continue;

		auto device = ied->device(obj[JSON_DEVICE].toString());
		if (!device)
			continue;

		auto ln = device->ln(obj[JSON_LN].toString());
		if (!ln)
			continue;

		auto gse = ln->gseControl(obj[JSON_GSE].toString());
		if (!gse)
			continue;

		GoosePublisher publisher{ absoluteFileName, config, ied, device, ln, gse };
		widget->setPublisher(publisher);
	}

	return true;
}

bool Dc61850Form::saveInputGooseSettings()
{
	saveInputGooseParams();

	QString iec61850Path = m_fileManager.localPath();
	if (!QDir(iec61850Path).mkpath(".")) {
		MsgBox::error(QString("Не удалось создать папку %1").arg(iec61850Path));
		return false;
	}

	QString jsonFileName = QString("%1/%2").arg(iec61850Path).arg(inputGooseSettingsFile);
	if (QFile::exists(jsonFileName))
		QFile::remove(jsonFileName);

	QFile file(jsonFileName);
	if (!file.open(QIODevice::WriteOnly)) {
		MsgBox::error(QString("Не удалось открыть файл %1: %2").arg(jsonFileName, file.errorString()));
		return false;
	}

	QHash<QString, QString> savedFiles;
	QJsonArray gooseArray;
	for (int i = 0; i < m_inputGooseWidgets.count(); i++) {
		auto p = m_inputGooseWidgets.at(i)->publisher();
		if (!p.gseControl)
			continue;		

		auto it = savedFiles.find(p.fileName);
		if (it == savedFiles.end()) {
			QString fileName = QString("input_goose%1.cid").arg(savedFiles.count() + 1);
			QString absoluteFileName = QString("%1/%2").arg(iec61850Path, fileName);
			if (!p.config->save(absoluteFileName)) {
				MsgBox::error(QString("Не удалось сохранить файл %1: %2").arg(absoluteFileName).arg(p.config->errorString()));
				return false;
			}
			it = savedFiles.insert(p.fileName, fileName);
		}

		QJsonObject obj;
		obj[JSON_INTERFACE_NUM] = i;
		obj[JSON_FILENAME] = it.value();
		obj[JSON_IED] = p.ied->name();
		obj[JSON_DEVICE] = p.device->inst();
		obj[JSON_LN] = p.ln->name();
		obj[JSON_GSE] = p.gseControl->name();
		gooseArray.append(obj);
	}

	QJsonObject mainObj;
	mainObj[JSON_GOOSELIST] = gooseArray;
	if (0 > file.write(QJsonDocument(mainObj).toJson())) {
		MsgBox::error(QString("Не удалось записать файл %1: %2").arg(jsonFileName, file.errorString()));
		return false;
	}

	return true;
}

bool Dc61850Form::saveInputGooseParams()
{	
	bool result = true;
	for (size_t i = 0; i < m_inputGooseWidgets.count(); i++) {
		InputGooseWidget *widget = m_inputGooseWidgets.at(i);

		FirstGoMsgUpdRules_Type st;
		st.GoMsg_StaticDoutsCtrl_AfterRst = widget->isControlAfterRestart();
		st.GoMsg_StaticDoutsCtrl__AfterDisc = widget->isControlAfterReconnection();
		st.GoMsg_Use_TAL_Mult = widget->ratioTimeAllowedToLive();
		result &= updateParamValue(SP_EXCHANGESETTINGS, INDEX_INPUT_GOOSE_SETTINGS_BASE + i, QString::number(st.toUint()));

		result &= updateParamValue(SP_TESTSEND, INDEX_INPUT_GOOSE_ACTIVE + i, widget->isActive() ? "1" : "0");
		if (widget->lgosMode() > -1)
			result &= updateParamValue(SP_TESTSEND, INDEX_INPUT_GOOSE_LGOSMODE + i, QString::number(widget->lgosMode()));

		auto incomingDataList = widget->incomingDataList();
		for (int j = 0; j < incomingDataList.count(); j++)
			result &= updateParamValue(SP_INPUTITEM_DFN, i * PROFILE_SIZE + j, QString::number(incomingDataList.at(j).toUint()));

		auto p = widget->publisher();
		if (!p.gseControl)
			continue;

		QString goCbRef = QString("%1%2/%3$GO$%4").arg(p.ied->name(), p.device->inst(), p.ln->name(), p.gseControl->name());
		QString goDatSet = QString("%1%2/%3$").arg(p.ied->name(), p.device->inst(), p.ln->name());
		if (p.gseControl->datSet())
			goDatSet.append(p.gseControl->datSet()->name());

		result &= updateParamValue(SP_PROTPARS_DWORD, INDEX_INPUT_GOOSE_BASE + i, QString::number(p.gseControl->confRev()));
		result &= updateParamValue(SP_PROTPARS_STRING, INDEX_INPUT_GOOSE_BASE + i, p.gseControl->appID());
		result &= updateParamValue(SP_GOCBREF_NAM, INDEX_INPUT_GOOSE_BASE + i, goCbRef);
		result &= updateParamValue(SP_GODATASET_NAM, INDEX_INPUT_GOOSE_BASE + i, goDatSet);

		auto gseAP = p.gseControl->gseAP();
		if (gseAP) {
			result &= updateParamValue(SP_PROTPARS_WORD, INDEX_INPUT_GOOSE_BASE + i, QString::number(gseAP->appID().toInt(nullptr, 16)));
			result &= updateParamValue(SP_MAC_ADDR, INDEX_INPUT_GOOSE_MAC_ADDRESS_BASE + i, gseAP->macAddress().replace("-", ":"));
			result &= updateParamValue(SP_VLAN_TAG, INDEX_INPUT_GOOSE_VLAN_BASE + i, gseAP->vlanId());
			result &= updateParamValue(SP_VLAN_PRIO, INDEX_INPUT_GOOSE_VLAN_PRIORITY_BASE + i, gseAP->vlanPriority());			
		}
	}

	return result;
}

void Dc61850Form::clearSelectedItemWidget()
{
	for (auto widget : m_selectedItemWidget->findChildren<QWidget*>())
		widget->deleteLater();
	delete m_selectedItemWidget->layout();
}

bool Dc61850Form::checkJavaInstalled()
{
	QProcess process;
	QByteArray errorBuffer;
	connect(&process, &QProcess::readyReadStandardError, this, [&process, &errorBuffer]() { errorBuffer.append(process.readAllStandardError()); });
	QByteArray outputBuffer;
	connect(&process, &QProcess::readyReadStandardOutput, this, [&process, &outputBuffer]() { outputBuffer.append(process.readAllStandardOutput()); });

	// запуск программы 	
	QStringList arguments{ "-version" };
	QApplication::setOverrideCursor(Qt::WaitCursor);
	process.start("java", arguments);
	process.waitForFinished();
	QApplication::restoreOverrideCursor();

	// обработка результата работы внешней программы
	if (process.exitStatus() != QProcess::NormalExit) {
		return false;
	}
	if (process.error() != QProcess::UnknownError) {
		return false;
	}
	if (process.exitCode()) {
		return false;
	}

    QByteArray javaVersionTag = " version \"";
	return errorBuffer.contains(javaVersionTag) || outputBuffer.contains(javaVersionTag);
}

void Dc61850Form::updateSVControlsEditor(const QList<SVControlPtr>& svList)
{
	for (size_t i = 0; i < svList.count(); i++)
        if (auto p = controller()->paramsRegistry().element(SP_MODE_SV, i); p)
			svList.at(i)->setWorkMode(p->value().toInt());

	auto tableModel = static_cast<SvTableModel*>(m_svTableView->model());
	tableModel->setItemList(svList);
	m_svTableView->resizeColumnsToContents();

	for (int column = 1; column < tableModel->columnCount(); column++)
		onSvModelDataChanged(tableModel->index(SvTableModel::SmpRateRow, column));
}
