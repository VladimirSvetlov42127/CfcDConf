#pragma once

#include <qwidget.h>

#include <dpc/journal/Journal.h>
#include <dpc/journal/Source.h>

class QLabel;

namespace Dpc::Gui
{
    class JournalTableModel;
	class JournalFilterModel;

	class TableView;
	class CheckableComboBox;

	class DPC_EXPORT JournalWidget : public QWidget
	{
		Q_OBJECT
	public:
        JournalWidget(Dpc::Journal* journal, QWidget* parent = nullptr);

	public slots:
		void start();
		void stop();
        void setSourceBoxVisible(bool visible);

	private slots:
		void onClearButton();
		void onSaveButton();

        void onSourceAdded(const Dpc::Journal::ISource* source);
        void onSourceRemoved(const Dpc::Journal::ISource* source);
        void onSourceSelected(const Dpc::Journal::ISource* source);
        void onSourceBoxChanged();
		void onRowsInserted();

	private:
        void clearSources(const QList<Dpc::Journal::ISource::IndexType>& list);
		void moveToSelected();

	private:
        Dpc::Journal* m_journal;
		JournalTableModel* m_model;
		JournalFilterModel* m_filterModel;

		TableView* m_view;
        QLabel* m_sourceBoxLabel;
		CheckableComboBox* m_sourceBox;

        QHash<Dpc::Journal::ISource::IndexType, const Dpc::Journal::ISource*> m_sources;
	};
} // namespace
