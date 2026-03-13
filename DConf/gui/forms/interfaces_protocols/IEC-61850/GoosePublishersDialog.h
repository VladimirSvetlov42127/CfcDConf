#pragma once

#include <qdialog.h>
#include <qset.h>

#include "GoosePublisher.h"

class QTreeWidget;
class QTreeWidgetItem;

class GoosePublishersDialog : public QDialog
{
	Q_OBJECT
public:
	enum TreeItemType {
		UnknownType = 0,
		GSEType
	};

	enum DataRole {
		FileNameRole = Qt::UserRole + 1,
		ConfigRole,
		IEDRole,
		DeviceRole,
		LnRole,
		GSEControlRole
	};

	GoosePublishersDialog(const QList<GoosePublisher> &publishersList, QDialog *parent = nullptr);
	~GoosePublishersDialog();

	GoosePublisher selectedPublisher() const { return m_selectedPublisher; }

private slots:
	void onAddButton();
	void onSelectButton();
	void onTreeWidgetSelectionChanged();
	void onTreeWidgetItemDoubleClicked(QTreeWidgetItem *item, int column);

private:
	void addToTreeWidget(const GoosePublisher &pub);

private:
	QTreeWidget *m_treeWidget;
	QPushButton *m_selectButton;
	GoosePublisher m_selectedPublisher;
	QSet<QString> m_loadedFiles;
};

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr);