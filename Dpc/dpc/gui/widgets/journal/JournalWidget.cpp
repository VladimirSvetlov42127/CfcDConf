#include "JournalWidget.h"

#include <qboxlayout.h>
#include <qapplication.h>
#include <qfiledialog.h>
#include <qtextstream.h>
#include <qpushbutton.h>
#include <qscrollbar.h>
#include <qlabel.h>

#include <dpc/gui/gui.h>

#include <dpc/gui/widgets/journal/JournalTableModel.h>
#include <dpc/gui/widgets/TableView.h>
#include <dpc/gui/widgets/CheckableComboBox.h>

namespace Dpc::Gui
{
    using namespace Dpc;
	using namespace Dpc::Gui;

    JournalWidget::JournalWidget(Journal *journal, QWidget* parent) :
		QWidget(parent),
        m_journal(journal),
		m_model(new JournalTableModel(this)), 
		m_filterModel(new JournalFilterModel(this)),
		m_view(new TableView(this)),
        m_sourceBoxLabel(new QLabel("Сообщения")),
		m_sourceBox(new CheckableComboBox)
		
    {
        connect(journal, &Journal::sourceAdded, this, &JournalWidget::onSourceAdded);
        connect(journal, &Journal::sourceRemoved, this, &JournalWidget::onSourceRemoved);
        connect(journal, &Journal::sourceSelected, this, &JournalWidget::onSourceSelected);
        connect(m_sourceBox, &CheckableComboBox::itemToogled, this, &JournalWidget::onSourceBoxChanged);
		
		m_filterModel->setSourceModel(m_model);
		//filterModel->setDynamicSortFilter(false);

		m_view->setModel(m_filterModel);
		m_view->setStyleSheet("color: #595a5a;");

		m_sourceBox->setSeparator(" | ");
		m_sourceBox->setMinimumWidth(300);
        m_sourceBox->appendItem(JournalTableModel::nullSourceText(), Journal::ISource::IndexType(), true);
        onSourceBoxChanged();

		auto infoButton = new QPushButton("Информация");
		infoButton->setIcon(JournalTableModel::infoIcon());
		infoButton->setCheckable(true);
		infoButton->setChecked(true);

		auto warnButton = new QPushButton("Предупреждения");
		warnButton->setIcon(JournalTableModel::warningIcon());
		warnButton->setCheckable(true);
		warnButton->setChecked(true);

		auto errButton = new QPushButton("Ошибки");
		errButton->setIcon(JournalTableModel::errorIcon());
		errButton->setCheckable(true);
		errButton->setChecked(true);

		auto debugButton = new QPushButton("Отладка");
		debugButton->setIcon(JournalTableModel::debugIcon());
		debugButton->setCheckable(true);
		debugButton->setChecked(false);

		auto onFiltersButton = [=] {
			uint8_t mask = 0;
            if (infoButton->isChecked()) mask |= Journal::Record::Information;
            if (warnButton->isChecked()) mask |= Journal::Record::Warning;
            if (errButton->isChecked())  mask |= Journal::Record::Error;
            if (debugButton->isChecked()) mask |= Journal::Record::Debug;
			m_filterModel->setTypeFilterMask(mask);
			moveToSelected();
		};

		connect(infoButton, &QPushButton::toggled, this, onFiltersButton);
		connect(warnButton, &QPushButton::toggled, this, onFiltersButton);
		connect(errButton, &QPushButton::toggled, this, onFiltersButton);
		connect(debugButton, &QPushButton::toggled, this, onFiltersButton);
		onFiltersButton();

		auto saveButton = new QPushButton("Сохранить");
        saveButton->setIcon(QIcon(":/icons/save1.svg"));
		connect(saveButton, &QPushButton::clicked, this, &JournalWidget::onSaveButton);

		auto clearButton = new QPushButton("Очистить");
        clearButton->setIcon(QIcon(":/icons/clean.svg"));
		connect(clearButton, &QPushButton::clicked, this, &JournalWidget::onClearButton);

		connect(m_model, &QAbstractItemModel::rowsInserted, this, &JournalWidget::onRowsInserted);

		auto buttonsLayout = new QHBoxLayout;
        buttonsLayout->addWidget(m_sourceBoxLabel);
		buttonsLayout->addWidget(m_sourceBox);
		buttonsLayout->addWidget(infoButton);
		buttonsLayout->addWidget(warnButton);
		buttonsLayout->addWidget(errButton);
		buttonsLayout->addWidget(debugButton);
		buttonsLayout->addStretch();
		buttonsLayout->addWidget(saveButton);
		buttonsLayout->addWidget(clearButton);

		auto mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(QMargins(6, 0, 6, 0));
		mainLayout->addLayout(buttonsLayout);
		mainLayout->addWidget(m_view);

		m_view->setSortingEnabled(false);
		m_view->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_view->setVerticalScrollMode(QAbstractItemView::ScrollPerItem);
		m_view->horizontalHeader()->resizeSection(JournalTableModel::DateTimeColumn, 100);
		m_view->horizontalHeader()->resizeSection(JournalTableModel::DeviceColumn, 250);
		m_view->horizontalHeader()->resizeSection(JournalTableModel::TypeColumn, 20);
		m_view->horizontalHeader()->setStretchLastSection(true);
		m_view->horizontalHeader()->setHighlightSections(false);
		m_view->setWordWrap(false);
	}

	void JournalWidget::start()
	{
        connect(m_journal, &Journal::newRecords, m_model, &JournalTableModel::onNewRecords);
	}

	void JournalWidget::stop()
	{
        disconnect(m_journal, &Journal::newRecords, m_model, &JournalTableModel::onNewRecords);
    }

    void JournalWidget::setSourceBoxVisible(bool visible)
    {
        m_sourceBox->setVisible(visible);
        m_sourceBoxLabel->setVisible(visible);
        m_view->horizontalHeader()->setSectionHidden(JournalTableModel::DeviceColumn, !visible);
    }

	void JournalWidget::onClearButton()
	{
        QList<Journal::ISource::IndexType> sourceList;
		for (auto& it : m_sourceBox->checkedItems())
            sourceList << it.value<Journal::ISource::IndexType>();

		clearSources(sourceList);
	}

	void JournalWidget::onSaveButton()
	{
		QString fileName = QFileDialog::getSaveFileName(this,
			tr("DcJournal"), "DcJournal",
			tr("DcJournal (*.csv);;All Files (*)"));

		if (fileName.isEmpty())
			return;

		QFile f(fileName);
		if (!f.open(QIODevice::WriteOnly)) {
			return;
		}

		QTextStream ts(&f);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		ts.setCodec("UTF-8");
#else
		ts.setEncoding(QStringConverter::Utf8);
#endif
		QStringList strList;
		strList << "\" \"";
		for (int c = 0; c < m_view->horizontalHeader()->count(); ++c)
			strList << "\"" + m_view->model()->headerData(c, Qt::Horizontal).toString() + "\"";
		ts << strList.join("; ") + "\n";

		for (int r = 0; r < m_view->model()->rowCount(); ++r) {
			strList.clear();
			strList << "\"" + m_view->model()->headerData(r, Qt::Vertical).toString() + "\"";
			for (int c = 0; c < m_view->horizontalHeader()->count(); ++c)
			{
				if (c != JournalTableModel::TypeColumn)
					strList << "\"" + m_view->model()->data(m_view->model()->index(r, c), Qt::DisplayRole).toString() + "\"";
				else {
					int type = m_view->model()->index(r, JournalTableModel::TypeColumn).data(Qt::UserRole).toInt();
                    if (type == Journal::Record::Information)
						strList << "\" Информация \"";
                    else if (type == Journal::Record::Debug)
						strList << "\" Отладка \"";
                    else if (type == Journal::Record::Error)
						strList << "\" Ошибка \"";
                    else if (type == Journal::Record::Warning)
						strList << "\" Предупреждение \"";
				}
			}
			ts << strList.join("; ") + "\n";
		}
	}

    void JournalWidget::onSourceAdded(const Journal::ISource* source)
	{
		auto id = source->id();
		m_sourceBox->appendItem(source->name(), id);

		auto it = m_sources.find(id);
		if (it != m_sources.end())
			return;

        m_sources.insert(id, source);
        connect(source, &Journal::ISource::nameChanged, this, [=](const QString& name) { m_sourceBox->setText(id, name); });
	}

    void JournalWidget::onSourceRemoved(const Journal::ISource* source)
	{
		auto id = source->id();
		m_sourceBox->removeItem(id);
		m_sources.remove(id);
		clearSources({ id });
	}

    void JournalWidget::onSourceSelected(const Journal::ISource* source)
	{
		m_sourceBox->blockSignals(true);
        m_sourceBox->select({ Journal::ISource::IndexType(), source->id() });
		m_sourceBox->blockSignals(false);
        onSourceBoxChanged();
	}

    void JournalWidget::onSourceBoxChanged()
	{
        QList<Journal::ISource::IndexType> sourceList;
		for (auto& it : m_sourceBox->checkedItems())
            sourceList << it.value<Journal::ISource::IndexType>();

		m_filterModel->setSourceFilterList(sourceList);
		moveToSelected();
	}

	void JournalWidget::onRowsInserted()
	{
		if (m_view->verticalScrollBar()->value() == m_view->verticalScrollBar()->maximum())
			//	m_view->scrollToBottom();
			QMetaObject::invokeMethod(m_view, &QAbstractItemView::scrollToBottom, Qt::QueuedConnection);
	}

    void JournalWidget::clearSources(const QList<Journal::ISource::IndexType>& list)
	{
		m_model->clearSources(list);
		m_view->scrollToBottom();
	}

	void JournalWidget::moveToSelected()
	{
		auto selected = m_view->selectionModel()->selectedRows().value(0);
		if (selected.isValid())
			m_view->scrollTo(selected, QAbstractItemView::EnsureVisible);
		else
			m_view->scrollToBottom();
	}
} // namespace
