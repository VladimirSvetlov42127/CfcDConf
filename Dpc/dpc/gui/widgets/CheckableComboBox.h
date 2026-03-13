#pragma once

#include <qcombobox.h>

#include <dpc/dpc_global.h>

class QStandardItemModel;
class QStandardItem;

namespace Dpc::Gui
{
	class DPC_EXPORT CheckableComboBox : public QComboBox
	{
		Q_OBJECT
	public:
		CheckableComboBox(QWidget* parent = nullptr);

		QVariantList checkedItems() const;

		QString separator() const;
		void setSeparator(const QString& sep);

	signals:
		void itemToogled(const QVariant& item, bool checked);

	public slots:
		void appendItem(const QString& text, const QVariant& itemData, bool checked = false);
		void removeItem(const QVariant& itemData);
		void setChecked(const QVariant& itemData, bool checked);
		void setText(const QVariant& itemData, const QString& text);
		void select(const QVariantList& itemDataList);

    protected:
        void wheelEvent(QWheelEvent* event) override;

	private slots:
		void onItemChanged(QStandardItem* itemDataList);

	private:
		void updateDisplayText();

	private:
		QStandardItemModel* m_model;
		QString m_sep;
	};
} // namespace
