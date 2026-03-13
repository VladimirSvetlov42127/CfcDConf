#include "passport_dialog.h"

#include <qgridlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qtreewidget.h>
#include <qheaderview.h>
#include <qsettings.h>

#include <dpc/helper.h>

namespace {
	const QString SettingsTag = "PassportDialog";

	QTreeWidgetItem* createModelItem(DcController *device, DcMenu *menu) {
		if (!menu->isAvailableFor(device))
			return nullptr;		

		auto item = new QTreeWidgetItem;
		item->setFlags(item->flags() | Qt::ItemIsAutoTristate);
		item->setText(0, menu->title());
        item->setData(0, Qt::UserRole, Dpc::fromPtr(menu));

		auto menuIdx = menu->formIdx();
		if (!menuIdx.isEmpty()) {
			QSettings settings;
			auto checked = settings.value(QString("%1/%2").arg(SettingsTag).arg(menuIdx), Qt::Unchecked).value<Qt::CheckState>();
			item->setCheckState(0, checked);
		}

		for (auto &it : menu->childs()) {
			auto childItem = createModelItem(device, it.second);
			if (childItem)
				item->addChild(childItem);
		}

		return item;
	}

    ConfigPassportOperation::Structure createPassportStructure(QTreeWidgetItem *item)
	{
        DcMenu *menu = Dpc::toPtr<DcMenu>(item->data(0, Qt::UserRole));
		if (!menu)
            return ConfigPassportOperation::Structure();

		auto checkState = item->checkState(0);
		if (Qt::Unchecked == checkState)
            return ConfigPassportOperation::Structure();

		auto menuIdx = menu->formIdx();
		if (!menuIdx.isEmpty() && Qt::Checked == checkState) {
			QSettings settings;
			settings.setValue(QString("%1/%2").arg(SettingsTag).arg(menuIdx), checkState);
		}
		
        QList<ConfigPassportOperation::Structure> childs;
		for (size_t i = 0; i < item->childCount(); i++) {
            auto p = createPassportStructure(item->child(i));
			if (p.section)
				childs << p;
		}

		return { menu, childs };
	}
}

DcPassportDialog::DcPassportDialog(DcController *controller, QWidget *parent) :
	QDialog(parent),
	m_treeWidget(new QTreeWidget)
{
	setWindowTitle("Паспорт конфигурации");

	m_treeWidget->setColumnCount(1);
	m_treeWidget->header()->setVisible(false);
	auto parentItem = m_treeWidget->invisibleRootItem();

	for (auto &it : DcMenu::root().childs()) {
		auto item = createModelItem(controller, it.second);
		if (item)
			parentItem->addChild(item);
	}

	auto okButton = new QPushButton("Ok");
	connect(okButton, &QPushButton::clicked, this, &DcPassportDialog::onOkButton);

	auto layout = new QGridLayout(this);
	layout->addWidget(new QLabel("Выберите разделы которые нужно включить в паспорт конфигурации"), 0, 0, 1, 3, Qt::AlignCenter);
	layout->addWidget(m_treeWidget, 1, 0, 2, 3);
	layout->addWidget(okButton, 3, 0, 1, 3, Qt::AlignRight);

	layout->setColumnStretch(1, 1);
	layout->setRowStretch(1, 1);

	resize(400, 500);
}

void DcPassportDialog::onOkButton()
{
	QSettings settings;
	settings.remove(SettingsTag);

	auto rootItem = m_treeWidget->invisibleRootItem();
    QList<ConfigPassportOperation::Structure> childs;
	for (size_t i = 0; i < rootItem->childCount(); i++) {
		auto p = createPassportStructure(rootItem->child(i));
		if (p.section)
			childs << p;
	}

    m_ps = ConfigPassportOperation::Structure{&DcMenu::root(), childs};
	accept();
}
