#include "DcEmbeddedAlgorithmsForm.h"

//===================================================================================================================================================
//	Подключение стандартных библиотек
//===================================================================================================================================================
#include <cstdint>

//===================================================================================================================================================
//	Подключение сторонних библиотек
//===================================================================================================================================================

//===================================================================================================================================================
//	Подключение библиотек QT
//===================================================================================================================================================
#include <QtAlgorithms>

//===================================================================================================================================================
//	Подключение модулей проекта
//===================================================================================================================================================

//===================================================================================================================================================
//	namespaces
//===================================================================================================================================================

#include <dpc/helper.h>

namespace {
	const ListEditorContainer g_ProfileList = { "1", "2", "3", "4" };

	QScrollArea* wrapWithScrollArea(QWidget *w)
	{
		QScrollArea *area = new QScrollArea;
		area->setWidgetResizable(true);
		area->setFrameShape(QFrame::NoFrame);
		area->setWidget(w);

		return area;
	}
}

namespace Text {
	const QString ActiveGroup = "Активная группа уставок";
}

//===================================================================================================================================================
//	Конструктор и деструктор класса
//===================================================================================================================================================
DcEmbeddedAlgorithmsForm::DcEmbeddedAlgorithmsForm(DcController *controller)
    : DcForm(controller, "Встроенные алгоритмы и защиты", false)
{
	//	Инициализация свойств класса
	_protect_tree = new QTreeWidget;
	_protect_tab = new QTabWidget;

	//	Надписи
	QString style_sheet = "QLabel { font-weight: bold; }";
	auto support_protect = new QLabel(tr("Доступные защиты"));
	auto active_protect = new QLabel(tr("Введенные в работу защиты"));
	auto automatic_label = new QLabel(tr("Встроенные алгоритмы автоматики"));
	support_protect->setStyleSheet(style_sheet);
	active_protect->setStyleSheet(style_sheet);
	automatic_label->setStyleSheet(style_sheet);

	//	Макет защиты
	DrawProtectTree(controller);
	connect(_protect_tree, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(TreeClicked(QTreeWidgetItem*, int)));
	auto protect_layout = new QGridLayout;
	protect_layout->addWidget(support_protect, 0, 0);
	protect_layout->addWidget(active_protect, 0, 2);
	protect_layout->addWidget(_protect_tree, 1, 0, 1, 2);
	protect_layout->addWidget(_protect_tab, 1, 2);

	//	Вкладка защиты
	auto protections = new QWidget;
	protections->setLayout(protect_layout);

	//	Вкладка автоматика
	auto automatic = new QTabWidget;
	for (auto alg : algorithmsList()) {
		alg->setController(controller);
		if (!alg->isAvailable()) continue;
		DrawAutomatic(alg.get(), automatic); }
	auto automatic_layout = new QGridLayout;
	automatic_layout->addWidget(automatic_label, 0, 0);
	automatic_layout->addWidget(automatic, 1, 0);
	auto automatic_widget = new QWidget;
	automatic_widget->setLayout(automatic_layout);

	//	Макет закладок для выбора настроек
	auto two_tabs = new QTabWidget;
	two_tabs->setTabShape(QTabWidget::Rounded);
	//two_tabs->setTabPosition(QTabWidget::South);
	two_tabs->setTabPosition(QTabWidget::North);
	if (_protect_tree->topLevelItemCount() > 0) two_tabs->addTab(protections, tr("Защита"));
	if (automatic->count() > 0) two_tabs->addTab(automatic_widget, tr("Автоматика"));

	//	Макет основного окна
	auto main_layout = new QGridLayout(centralWidget());
	main_layout->addWidget(two_tabs, 0, 0);

	return;
}

DcEmbeddedAlgorithmsForm::~DcEmbeddedAlgorithmsForm()
{
	if (_protect_tree != nullptr) delete _protect_tree;
	if (_protect_tab != nullptr) delete _protect_tab;
	return;
}

bool DcEmbeddedAlgorithmsForm::isAvailableFor(DcController * controller)
{
	for (auto alg: algorithmsList()) {
		alg->setController(controller);
		if (alg->isAvailable())
			return true;
	}

	return false;
}

void DcEmbeddedAlgorithmsForm::fillReport(DcIConfigReport * report)
{
	report->insertSection();
	//report->nextLine();

	auto device = report->device();
    auto groupParam = device->paramsRegistry().element(SP_ACTIVPROFILE, 0);
	if (groupParam) {
		DcReportTable table(device);
		table.addRow({ Text::ActiveGroup, QString::number(groupParam->value().toUInt() + 1) });
		report->insertTable(table);
	}
	
	for (auto alg : algorithmsList()) {
		alg->setController(device);
		if (!alg->isAvailable())
			continue;

		report->insertSection(alg->name());
		alg->fillReport(report);
	}
}


//===================================================================================================================================================
//	Вспомогательные методы класса
//===================================================================================================================================================
void DcEmbeddedAlgorithmsForm::DrawAutomatic(IAlgorithm* algorithm, QTabWidget* automatic)
{
	if (!algorithm) return;
	QString name = algorithm->name();
	if (name != QString("ВНР") && (algorithm->hasSwitcher() || !algorithm->isAnyActive())) return;

	auto automatic_area = new QScrollArea;
	automatic_area->setWidgetResizable(true);
	automatic_area->setFrameShape(QFrame::NoFrame);
    automatic_area->setProperty("alg", Dpc::fromPtr(algorithm));
	automatic->addTab(automatic_area, algorithm->name());
	automatic_area->setWidget(algorithm->createWidget());
	automatic->setCurrentWidget(automatic_area);

	return;
}

void DcEmbeddedAlgorithmsForm::DrawProtectTree(DcController* controller)
{
	QList<QTreeWidgetItem*> topItems;

	//	Сортировка списка алгоритмов
    std::sort(algorithmsList().begin(), algorithmsList().end(), [](AlgorithmPtr a, AlgorithmPtr b) {return a->name().toUpper() < b->name().toUpper(); });

	//	Вывод списка алгоритмов
	for (auto alg : algorithmsList()) {
		alg->setController(controller);
		auto name = alg->name();
		if (name == QString("ВНР")) continue;
		if (!alg->isAvailable()) continue;

		if (alg->hasSwitcher()) {
			auto algItem = new QTreeWidgetItem;
			algItem->setText(0, alg->name());
			algItem->setCheckState(0, alg->isActive() ? Qt::Checked : Qt::Unchecked);
            algItem->setData(0, Qt::UserRole, Dpc::fromPtr(alg.get()));
			topItems.append(algItem);
			if (alg->groupCount() > 1) {
				bool isAllActive = true;
				bool isAllUnactive = true;
				for (size_t i = 0; i < alg->groupCount(); i++) {
					auto isActive = alg->isActive(i);
					isAllActive &= isActive;
					isAllUnactive &= !isActive;

					auto groupItem = new QTreeWidgetItem(algItem);
					groupItem->setText(0, QString("Группа уставок %1").arg(i + 1));
					groupItem->setCheckState(0, isActive ? Qt::Checked : Qt::Unchecked);
                    groupItem->setData(0, Qt::UserRole, Dpc::fromPtr(alg.get())); }
				if (isAllActive) algItem->setCheckState(0, Qt::Checked);
				else if (isAllUnactive) algItem->setCheckState(0, Qt::Unchecked);
				else algItem->setCheckState(0, Qt::PartiallyChecked); }
		}
		DrawProtectTab(alg.get()); }

	_protect_tree->setHeaderHidden(true);
	_protect_tree->addTopLevelItems(topItems);

	return;
}

void DcEmbeddedAlgorithmsForm::DrawProtectTab(IAlgorithm* algorithm)
{
	if (!algorithm) return;
	if (!algorithm->hasSwitcher()) return;

	QScrollArea* scroll_area = nullptr;
	for (size_t i = 0; i < _protect_tab->count(); i++) {
		auto w = _protect_tab->widget(i);
        if (Dpc::toPtr<IAlgorithm>(w->property("alg")) == algorithm) {
			scroll_area = dynamic_cast<QScrollArea*>(w);
			break; }
	}

	if (!algorithm->isAnyActive()) {
		if (scroll_area) {
			_protect_tab->removeTab(_protect_tab->indexOf(scroll_area));
			scroll_area->deleteLater(); }
	} else {
		if (!scroll_area) {
			scroll_area = new QScrollArea;
			scroll_area->setWidgetResizable(true);
			scroll_area->setFrameShape(QFrame::NoFrame);
            scroll_area->setProperty("alg", Dpc::fromPtr(algorithm));
			_protect_tab->addTab(scroll_area, algorithm->name()); }

		scroll_area->setWidget(algorithm->createWidget());
		_protect_tab->setCurrentWidget(scroll_area); }

	return;
}


//===================================================================================================================================================
//	Методы обработки изменений
//===================================================================================================================================================
void DcEmbeddedAlgorithmsForm::TreeClicked(QTreeWidgetItem* item, int column)
{
	auto checkState = item->checkState(0);
    auto alg = Dpc::toPtr<IAlgorithm>(item->data(0, Qt::UserRole));
	
	if (!item->parent()) {
		if (checkState != Qt::PartiallyChecked) {
			alg->setActive(checkState);
			for (size_t i = 0; i < item->childCount(); i++) {
				item->child(i)->setCheckState(0, checkState);
				alg->setActive(checkState, i); }
		}
	} else {
		auto parent = item->parent();
		alg->setActive(checkState, parent->indexOfChild(item));
		bool isAllEqual = true;
		for (size_t i = 0; i < parent->childCount(); i++) if (parent->child(i)->checkState(0) != checkState) isAllEqual = false;
		parent->setCheckState(0, isAllEqual ? checkState : Qt::PartiallyChecked); }
	
	DrawProtectTab(alg);
	
	return;
}
