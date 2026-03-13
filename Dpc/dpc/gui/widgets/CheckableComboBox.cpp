#include "CheckableComboBox.h"

#include <qlineedit.h>
#include <qstandarditemmodel.h>
#include <qlistview.h>
#include <qdebug.h>
#include <QWheelEvent>

namespace Dpc::Gui
{
	CheckableComboBox::CheckableComboBox(QWidget* parent) :
		QComboBox(parent),
		m_model(new QStandardItemModel(this)),
		m_sep(", ")
	{
		setEditable(true);
		lineEdit()->setReadOnly(true);
		lineEdit()->blockSignals(true);

		connect(m_model, &QStandardItemModel::itemChanged, this, &CheckableComboBox::onItemChanged);
		setModel(m_model);

		QListView* listVw = qobject_cast<QListView*>(view());
		if (listVw)
			listVw->setSpacing(2);
	}

	QVariantList CheckableComboBox::checkedItems() const
	{
		QVariantList items;
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto item = m_model->item(i);
			if (item->data(Qt::CheckStateRole) == Qt::Checked) {
				items << item->data();
			}
		}

		return items;
	}

	QString CheckableComboBox::separator() const
	{
		return m_sep;
	}

	void CheckableComboBox::setSeparator(const QString& sep)
	{
		m_sep = sep;
	}

	void CheckableComboBox::appendItem(const QString& text, const QVariant& itemData, bool checked)
	{
		auto checkValue = checked ? Qt::Checked : Qt::Unchecked;
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto it = m_model->item(i);
			if (it->data() == itemData) {
				it->setText(text);
				it->setCheckState(checkValue);
				updateDisplayText();
				return;
			}
		}

		auto it = new QStandardItem(text);
		it->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
		it->setData(itemData);
		it->setCheckState(checkValue);
		m_model->appendRow(it);
		updateDisplayText();
	}

	void CheckableComboBox::removeItem(const QVariant& itemData)
	{
		for (size_t i = 0; i < m_model->rowCount(); i++)
			if (m_model->item(i)->data() == itemData) {
				auto it = m_model->takeItem(i);
				m_model->removeRow(i);
				delete it;
				break;
			}

		updateDisplayText();
	}

	void CheckableComboBox::setChecked(const QVariant& itemData, bool checked)
	{
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto it = m_model->item(i);
			if (it->data() == itemData)
				it->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
		}
	}

	void CheckableComboBox::setText(const QVariant& itemData, const QString& text)
	{
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto it = m_model->item(i);
			if (it->data() == itemData)
				it->setText(text);
		}
	}

	void CheckableComboBox::select(const QVariantList& itemDataList)
	{
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto it = m_model->item(i);
			if (itemDataList.contains(it->data()))
				it->setCheckState(Qt::Checked);
			else
				it->setCheckState(Qt::Unchecked);
        }
    }

    void CheckableComboBox::wheelEvent(QWheelEvent *event)
    {
        event->accept();
    }

	void CheckableComboBox::onItemChanged(QStandardItem* itemDataList)
	{
		updateDisplayText();
		emit itemToogled(itemDataList->data(), itemDataList->checkState());
	}

	void CheckableComboBox::updateDisplayText()
	{
		QStringList itemsText;
		for (size_t i = 0; i < m_model->rowCount(); i++) {
			auto it = m_model->item(i);
			if (it->data(Qt::CheckStateRole) == Qt::Checked) {
				itemsText << it->text();
			}
		}

		QString text = itemsText.join(separator());
		lineEdit()->setText(text);
		lineEdit()->setCursorPosition(0);
		setToolTip(text);
		if (text.isEmpty())
			setCurrentIndex(-1);
	}
} // namespace
