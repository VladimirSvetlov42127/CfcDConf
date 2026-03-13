#include "ComboBoxDelegate.h"

#include <qproxystyle.h>
#include <dpc/gui/widgets/ComboBox.h>
#include <QPainter>



namespace {
	class AlignComboBoxProxyStyle : public QProxyStyle {
	public:
		using QProxyStyle::QProxyStyle;

		void drawItemText(QPainter *painter,
			const QRect &rect,
			int /*flags*/,
			const QPalette &pal,
			bool enabled,
			const QString &text,
			QPalette::ColorRole textRole) const
		{
			QProxyStyle::drawItemText(painter, rect, Qt::AlignCenter, pal, enabled, text, textRole);
		}
	};
} // namespace

namespace Dpc::Gui
{
	ComboBoxDelegate::ComboBoxDelegate(QObject* parent) :
		QStyledItemDelegate(parent)
	{
		init();
	}

	ComboBoxDelegate::ComboBoxDelegate(const QList<ComboBoxDelegate::Item>& items, QObject* parent) :
		QStyledItemDelegate(parent)
	{
		init();
		m_items[-1] = items;
	}

	ComboBoxDelegate::ComboBoxDelegate(std::initializer_list<ComboBoxDelegate::Item> list, QObject* parent) :
		QStyledItemDelegate(parent)
	{
		init();
		for (auto&& it : list)
			m_items[-1].append(std::move(it));
	}

	ComboBoxDelegate::ComboBoxDelegate(const QStringList& list, QObject* parent) :
		QStyledItemDelegate(parent)
	{
		init();
		for (size_t i = 0; i < list.count(); i++) {
			uint32_t value = i;
			m_items[-1].append({list.at(i), value});
		}
	}

	ComboBoxDelegate::ComboBoxDelegate(std::initializer_list<QString> list, QObject* parent) :
		QStyledItemDelegate(parent)
	{
		init();
		size_t index = 0;
		for (auto&& it : list) {
			uint32_t value = index;
			m_items[-1].append({std::move(it), value});
			++index;
		}
	}

	QWidget* ComboBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		auto section = m_orientation == Qt::Vertical ? index.row() : index.column();
		auto sectionIt = m_items.find(section);
		if (sectionIt == m_items.end()) 
			sectionIt = m_items.find(-1);

		auto editor = new ComboBox(parent);
		editor->setFrame(false);
		editor->setStyle(new AlignComboBoxProxyStyle(editor->style()));
		uint32_t value = index.model()->data(index, Qt::EditRole).toUInt();

		for (const auto& item : sectionIt.value()) {
			bool skip = true;
			if (item.value == value && _allowed) skip = false;
			if (_excluding_on && isExcluded(item) && skip) continue;
			editor->addItem(item.text, item.value); }

		connect(editor, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ComboBoxDelegate::onCurrentIndexChanged);
		return editor;
	}

	void ComboBoxDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
	{
		auto value = index.model()->data(index, Qt::EditRole).toUInt();
		auto comboBox = static_cast<ComboBox*>(editor);
		comboBox->blockSignals(true);
		comboBox->setCurrentIndex(-1);
		for (size_t i = 0; i < comboBox->count(); i++)
			if (comboBox->itemData(i).toUInt() == value) {
				comboBox->setCurrentIndex(i);
				break;
			}
		comboBox->blockSignals(false);
	}

	void ComboBoxDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
	{
		auto comboBox = static_cast<ComboBox*>(editor);
		model->setData(index, comboBox->currentData(), Qt::EditRole);
	}

	void ComboBoxDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		editor->setGeometry(option.rect);
	}

    void ComboBoxDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
    {
        QStyledItemDelegate::paint(painter, option, index);
    }

	void ComboBoxDelegate::append(const QString& text, uint32_t value, int section)
	{
		m_items[section].append({ text, value });
	}

	void ComboBoxDelegate::append(const ComboBoxDelegate::Item& item, int section)
	{
		m_items[section].append(item);
	}

	QVariant ComboBoxDelegate::textFor(uint32_t value, const QVariant& defatultValue, int section) const
	{
		for (auto& it : m_items[section])
			if (it.value == value)
				return it.text;

		return defatultValue;
	}

	QList<ComboBoxDelegate::Item> ComboBoxDelegate::items(int section) const
	{
		auto findIt = m_items.find(section);
		if (findIt != m_items.end())
			return findIt.value();

		return m_items[-1];
	}

	void ComboBoxDelegate::setItems(const QList<ComboBoxDelegate::Item>& items, int section)
	{
		m_items[section] = items;
	}

	void ComboBoxDelegate::removeSection(int section)
	{
		if (section == -1)
			return;
		
		m_items.remove(section);
	}

	void ComboBoxDelegate::onCurrentIndexChanged(int index)
	{
		auto w = static_cast<QComboBox*>(sender());
		if (w) {
			emit commitData(w);
			//emit closeEditor(w); 
		}
	}

	void ComboBoxDelegate::init()
	{
		m_orientation = Qt::Vertical;
		m_items[-1] = QList<Item>();
		_excluding_on = false;
		_excluding_list = QList<uint32_t>();
		_allowed = true;
	}

	//===================================================================================================================================================
	//	Методы работы с исключениями
	//===================================================================================================================================================
	void ComboBoxDelegate::setExcluding(const QList<uint32_t>& exceptions)
	{
		_excluding_list = exceptions;
		if (!_excluding_list.isEmpty()) _excluding_on = true;
	}

	void ComboBoxDelegate::appendExcludes(const uint32_t exception)
	{
		_excluding_list.append(exception);
		_excluding_on = true;
	}

	void ComboBoxDelegate::appendExcludes(const QList<uint32_t>& items)
	{
		for (int i = 0; i < items.count(); i++) _excluding_list.append(items.at(i));
		_excluding_on = true;
	}

	bool ComboBoxDelegate::isExcluded(const ComboBoxDelegate::Item& item) const
	{
		if (!_excluding_on) return false;
		return _excluding_list.contains(item.value);
	}


	//===================================================================================================================================================
	//	Перегружаемые методы класса
	//===================================================================================================================================================
	void ComboBoxDelegate::initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const
	{
		QStyledItemDelegate::initStyleOption(option, index);
        if (index.flags() & Qt::ItemIsEditable)
            option->displayAlignment = Qt::AlignCenter;

        int section = m_orientation == Qt::Vertical ? index.row() : index.column();
        auto sectionIt = m_items.find(section);
        if (sectionIt == m_items.end())
            sectionIt = m_items.find(-1);

        auto &sectionList = sectionIt.value();
        uint32_t value = index.data(Qt::EditRole).toUInt();
        for (int i = 0; i < sectionList.count(); ++i) {
            const auto &item = sectionList.at(i);
            if (item.value == value)  {
                option->text = item.text;
                break;
            }
        }
    }

} // namespace
