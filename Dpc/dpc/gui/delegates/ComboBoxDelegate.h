#pragma once

#include <qstyleditemdelegate.h>

#include <dpc/dpc_global.h>

namespace Dpc::Gui
{
	class DPC_EXPORT ComboBoxDelegate : public QStyledItemDelegate
	{
		Q_OBJECT
	public:
		struct Item
		{
			QString text;
			uint32_t value;
		};

		ComboBoxDelegate(QObject* parent = 0);
		ComboBoxDelegate(const QList<ComboBoxDelegate::Item>& items, QObject* parent = 0);
		ComboBoxDelegate(std::initializer_list<ComboBoxDelegate::Item> list, QObject* parent = 0);
		ComboBoxDelegate(const QStringList& list, QObject* parent = 0);
		ComboBoxDelegate(std::initializer_list<QString> list, QObject* parent = 0);

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
		virtual void setEditorData(QWidget* editor, const QModelIndex& index) const override;
		virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const override;
		virtual void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
        virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

		void append(const QString& text, uint32_t value, int section = -1);
		void append(const ComboBoxDelegate::Item& item, int section = -1);
		QVariant textFor(uint32_t value, const QVariant &defatultValue = QVariant(), int section = -1) const;

		QList<Item> items(int section = -1) const;
		void setItems(const QList<ComboBoxDelegate::Item>& items, int section = -1);
		void removeSection(int section);

		Qt::Orientation orientation() const { return m_orientation; }
		void setOrientation(Qt::Orientation o) { m_orientation = o; }

		//	Методы работы с исключениями
		bool excluding() { return _excluding_on; }
		void allowExcluding(bool flag) { _excluding_on = flag; }
		void setExcluding(const QList<uint32_t>& exceptions);
		void appendExcludes(const uint32_t exception);
		void appendExcludes(const QList<uint32_t>& exceptions);
		bool isExcluded(const ComboBoxDelegate::Item& item) const;
		void allowCurrentSection(bool flag) { _allowed = flag; }

	private slots:
		void onCurrentIndexChanged(int index);

	protected:
		virtual void initStyleOption(QStyleOptionViewItem* option, const QModelIndex& index) const override;		

	private:
		void init();

	private:
		// Key - секция. Если ориентация делегата вертикальная, то секция это номер строки. 
		// Если ориентация горизонтальная, то секция это номер столбца.
		// Value - cписок элементов для этой секции. 
		// При key = -1, общая секция(по умолчанию).
		QHash<int, QList<Item>> m_items;
		Qt::Orientation m_orientation;
		//
		QList<uint32_t> _excluding_list;		//	Список исключений для всех секций
		bool _excluding_on;						//	Флаг активации/деактивации исключений
		bool _allowed;						    //	Флаг разрешения отображения исключений
	};
} // namespace
