#include "GoosePublishersDialog.h"

#include <qdebug.h>
#include <qtreewidget.h>
#include <qgridlayout.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qsettings.h>

#include <dpc/gui/dialogs/msg_box/MsgBox.h>

using namespace Dpc::Gui;

namespace {

} // namespace

GoosePublishersDialog::GoosePublishersDialog(const QList<GoosePublisher> &publishersList, QDialog *parent) :
	QDialog(parent),
	m_treeWidget(new QTreeWidget(this)),
	m_selectButton(new QPushButton("Выбрать", this))
{
	setWindowTitle("Goose издатели");
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	QGridLayout *treeWidgetLayout = new QGridLayout;
	QHBoxLayout *buttonsLayout = new QHBoxLayout;
	mainLayout->addLayout(treeWidgetLayout);
	mainLayout->addLayout(buttonsLayout);

	m_treeWidget->setColumnCount(1);
	m_treeWidget->setHeaderHidden(true);
	connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &GoosePublishersDialog::onTreeWidgetSelectionChanged);
	connect(m_treeWidget, &QTreeWidget::itemDoubleClicked, this, &GoosePublishersDialog::onTreeWidgetItemDoubleClicked);
	QPushButton *addButton = new QPushButton("Добавить", this);
	connect(addButton, &QPushButton::clicked, this, &GoosePublishersDialog::onAddButton);

	treeWidgetLayout->addWidget(m_treeWidget, 0, 0, 2, 2);
	treeWidgetLayout->addWidget(addButton, 0, 2);
	treeWidgetLayout->setColumnStretch(1, 1);
	treeWidgetLayout->setRowStretch(1, 1);

	m_selectButton->setEnabled(false);
	connect(m_selectButton, &QPushButton::clicked, this, &GoosePublishersDialog::onSelectButton);
	QPushButton *cancelButton = new QPushButton("Отмена", this);
	connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

	buttonsLayout->addStretch();
	buttonsLayout->addWidget(m_selectButton);
	buttonsLayout->addWidget(cancelButton);

	for (auto &pub : publishersList)
		addToTreeWidget(pub);
}

GoosePublishersDialog::~GoosePublishersDialog()
{
}

void GoosePublishersDialog::onAddButton()
{
	QSettings settings;
	auto filePathKey = QString("InputGoose/PublisherFilePath");
	auto dir = settings.value(filePathKey, "/home").toString();

	QString fileName = QFileDialog::getOpenFileName(this, "Выбрать файл", dir, "Файл конфигурации (*.cid *.icd *scd)");
	if (fileName.isEmpty())
		return;

	settings.setValue(filePathKey, QFileInfo(fileName).absolutePath());

	auto configuration = std::make_shared<IEC61850Configuration>();
	if (!configuration->load(fileName)) {
		MsgBox::error(QString("Не удалось загрузить конфигурацию из файла %1: %2")
			.arg(fileName)
			.arg(configuration->errorString()));
		return;
	}

	addToTreeWidget(GoosePublisher{ fileName, configuration });
}

void GoosePublishersDialog::onSelectButton()
{
	onTreeWidgetItemDoubleClicked(m_treeWidget->selectedItems().value(0), 0);
}

void GoosePublishersDialog::onTreeWidgetSelectionChanged()
{
	m_selectButton->setEnabled(m_treeWidget->selectedItems().value(0));
}

void GoosePublishersDialog::onTreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column)
{
	if (!item || item->type() != GSEType)
		return;

	m_selectedPublisher.fileName = item->data(0, FileNameRole).toString();
	m_selectedPublisher.config = item->data(0, ConfigRole).value<IEC61850ConfigurationPtr>();
	m_selectedPublisher.ied = item->data(0, IEDRole).value<IEDPtr>();
	m_selectedPublisher.device = item->data(0, DeviceRole).value<LDevicePtr>();
	m_selectedPublisher.ln = item->data(0, LnRole).value<LNPtr>();
	m_selectedPublisher.gseControl = item->data(0, GSEControlRole).value<GSEControlPtr>();
	accept();
}

void GoosePublishersDialog::addToTreeWidget(const GoosePublisher & pub)
{
	if (m_loadedFiles.contains(pub.fileName))
		return;

	QTreeWidgetItem *fileItem = new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), { pub.fileName });
	fileItem->setFlags(fileItem->flags() & ~Qt::ItemIsSelectable);
	m_treeWidget->addTopLevelItem(fileItem);
	fileItem->setExpanded(true);

	for (auto &ied : pub.config->iedList()) {
		QTreeWidgetItem *iedItem = nullptr;
		for (auto &dev : ied->lDevices())
			for(auto &ln: dev->lnList())
				for (auto &gseControl : ln->gseControls()) {
					if (!iedItem) {
						iedItem = new QTreeWidgetItem(fileItem, { ied->name() });
						iedItem->setFlags(fileItem->flags() & ~Qt::ItemIsSelectable);
						iedItem->setExpanded(true);
					}

					QString text = QString("%1/%2/%3").arg(dev->inst(), ln->name(), gseControl->name());
					QTreeWidgetItem *gseItem = new QTreeWidgetItem(iedItem, { text }, GSEType);

					gseItem->setData(0, FileNameRole, pub.fileName);
					gseItem->setData(0, ConfigRole, QVariant::fromValue(pub.config));
					gseItem->setData(0, IEDRole, QVariant::fromValue(ied));
					gseItem->setData(0, DeviceRole, QVariant::fromValue(dev));
					gseItem->setData(0, LnRole, QVariant::fromValue(ln));
					gseItem->setData(0, GSEControlRole, QVariant::fromValue(gseControl));
				}
	}

	m_loadedFiles.insert(pub.fileName);
}
